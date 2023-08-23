/*
 * Copyright 2021 Lars Pastewka
 *
 * ### MIT license
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>
#include <numeric>

#include "neighbors.h"

NeighborList::NeighborList() : seed_{1}, neighbors_{1} {}

const std::tuple<const Eigen::ArrayXi &, const Eigen::ArrayXi &>
NeighborList::update(const Atoms &atoms, double cutoff) {
    // Shorthand for atoms.positions.
    auto &&r{atoms.positions};

    // Avoid computing if atoms is empty
    if (r.size() == 0) {
      seed_.resize(0);
      neighbors_.resize(0);
      return {seed_, neighbors_};
    }

    // Origin stores the bottom left corner of the enclosing rectangles and
    // lengths the three Cartesian lengths.
    Eigen::Array3d origin{3}, lengths{3}, padding_lengths{3};

    // This is the number of cells/grid points that fit into the enclosing
    // rectangle. The grid is such that a sphere of diameter *cutoff* fits into
    // each cell.
    Eigen::Array3i nb_grid_pts{3};

    // Compute box that encloses all atomic positions. Make sure that box
    // lengths are exactly divisible by the interaction range. Also compute the
    // number of cells in each Cartesian direction.
    origin = r.rowwise().minCoeff();
    lengths = r.rowwise().maxCoeff() - origin;
    nb_grid_pts = (lengths / cutoff).ceil().cast<int>();

    // Set to 1 if all atoms are in-plane
    nb_grid_pts = (nb_grid_pts <= 0).select(1, nb_grid_pts);

    // Pad
    padding_lengths = nb_grid_pts.cast<double>() * cutoff - lengths;
    origin -= padding_lengths / 2;
    lengths += padding_lengths;

    // Compute cell indices. The follow array contains the cell index for each
    // atom.
    Eigen::ArrayXi atom_to_cell{
        coordinate_to_index(((r.colwise() - origin).colwise() *
                             (nb_grid_pts.cast<double>() / lengths))
                                .floor()
                                .cast<int>(),
                            nb_grid_pts)};

    // We now sort the cell indices. This will allow us to search for the atoms
    // that sit in neighboring cells.
    Eigen::ArrayXi sorted_atom_indices{atom_to_cell.size()};

    // Fill array `sorted_atom_indices` with consecutive numbers, i.e. 0, 1, 2,
    // 3, 4, 5, ...
    std::iota(sorted_atom_indices.begin(), sorted_atom_indices.end(), 0);

    // Sort the array `sorted_atom_indices` by cell index. This yields an array
    // of atom indices, but now sorted by cell, i.e. all atoms within cell 0 are
    // at the beginning of the array, followed by all atoms in cell 1 etc..
    // Example:
    //     sorted_atom_indices:                2 4 9 6 7 8 0 1 3 9
    //     atom_to_cell(sorted_atom_indices):  0 0 0 0 1 1 1 2 2 3
    //                                         ^       ^     ^   ^
    //     cell_index:                         0       1     2   3
    //     entry_index:                        0       4     7   9
    std::sort(sorted_atom_indices.begin(), sorted_atom_indices.end(),
              [&](int i, int j) { return atom_to_cell[i] < atom_to_cell[j]; });

    // We now build an array that points to the first entry within a certain
    // cell in the `sorted_atom_indices` array. We use a std::vector because we
    // need to dynamically grow this array.
    std::vector<std::tuple<int, int>> binned_atoms{};
    int cell_index{atom_to_cell(sorted_atom_indices(0))};
    int entry_index{0};

    // This stores the index of the first entry for each cell.
    binned_atoms.push_back({cell_index, entry_index});

    // We now loop over the sorted atom indices and check when the cell index
    // changes.
    for (int i{1}; i < sorted_atom_indices.size(); ++i) {
        if (atom_to_cell(sorted_atom_indices(i)) != cell_index) {
            cell_index = atom_to_cell(sorted_atom_indices(i));
            entry_index = i;
            binned_atoms.push_back({cell_index, entry_index});
        }
    }

    // We are now in a position to build a neighbor list in linear order. We are
    // doing a bit of optimization here. Since we have a dynamically growing
    // list, we don't want to resize every time we add a neighbor. We are
    // therefore doubling the size when necessary and then resizing once (to a
    // shorter array) when the list has been build.
    seed_.resize(atoms.nb_atoms() + 1);

    int n{0};
    auto cutoffsq{cutoff * cutoff};

    // Constructing index shift vectors to look for neighboring cells
    auto neighborhood = []() {
        Eigen::Array<int, 3, 27> neighborhood;
        auto n_it = neighborhood.colwise().begin();
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y)
                for (int z = -1; z <= 1; ++z) {
                    *n_it = Eigen::Vector3i{x, y, z};
                    ++n_it;
                }
        return neighborhood;
    }();

    for (int i{0}; i < atoms.nb_atoms(); ++i) {
        seed_(i) = n;

        Eigen::Array3i cell_coord{
            (nb_grid_pts.cast<double>() * (r.col(i) - origin) / lengths)
                .floor()
                .cast<int>()};

        // Loop over neighboring cells.
        for (auto &&shift : neighborhood.colwise()) {
            Eigen::Array3i neigh_cell_coord{cell_coord + shift.array()};

            // Skip if cell is out of bounds
            if ((neigh_cell_coord < 0).any() ||
                (neigh_cell_coord >= nb_grid_pts).any())
                continue;

            int cell_index{coordinate_to_index(neigh_cell_coord, nb_grid_pts)};

            // Find first entry within the cell neighbor list.
            auto cell{std::lower_bound(binned_atoms.begin(), binned_atoms.end(),
                                       cell_index,
                                       [&](const auto &i, const auto &j) {
                                           return std::get<0>(i) < j;
                                       })};

            if (cell == binned_atoms.end() || std::get<0>(*cell) != cell_index)
                continue;

            for (int j{std::get<1>(*cell)};
                 j < atom_to_cell.size() &&
                 atom_to_cell(sorted_atom_indices(j)) == cell_index;
                 ++j) {
                auto neighi{sorted_atom_indices(j)};

                // Exclude the atom from being its own neighbor
                if (neighi == i)
                    continue;

                auto distance_sq =
                    (r.col(i) - r.col(neighi)).matrix().squaredNorm();

                if (distance_sq <= cutoffsq) {
                    if (n >= neighbors_.size()) {
                        neighbors_.conservativeResize(2 * neighbors_.size());
                    }
                    neighbors_(n) = neighi;
                    n++;
                }
            }
        }
    }
    seed_(atoms.nb_atoms()) = n;
    neighbors_.conservativeResize(n);

    return {seed_, neighbors_};
}
