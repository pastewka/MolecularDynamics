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

#include "atoms.h"
#include "neighbors.h"
#include "xyz.h"

#include <gtest/gtest.h>

TEST(NeighborsTest, Test1) {
    Names_t names{{"H", "H", "H", "H"}};
    Positions_t positions(3, 4);
    positions << 0, 1, 0, 0,
                 0, 0, 1, -1,
                 0, 0, 0, 0;

    Atoms atoms(names, positions);
    NeighborList neighbor_list;
    auto &[seed, neighbors]{neighbor_list.update(atoms, 1.5)};

    // All atoms except 3 and 4 are neighbors of each other
    EXPECT_EQ(neighbor_list.nb_neighbors(), 10);

    EXPECT_EQ(neighbor_list.nb_neighbors(0), 3);
    EXPECT_EQ(neighbor_list.nb_neighbors(1), 3);
    EXPECT_EQ(neighbor_list.nb_neighbors(2), 2);
    EXPECT_EQ(neighbor_list.nb_neighbors(3), 2);

    EXPECT_TRUE((neighbors(Eigen::seq(seed(0), seed(1) - 1)) == Eigen::Array3i{3, 1, 2}).all());
    EXPECT_TRUE((neighbors(Eigen::seq(seed(1), seed(2) - 1)) == Eigen::Array3i{3, 0, 2}).all());
    EXPECT_TRUE((neighbors(Eigen::seq(seed(2), seed(3) - 1)) == Eigen::Array2i{0, 1}).all());
    EXPECT_TRUE((neighbors(Eigen::seq(seed(3), seed(4) - 1)) == Eigen::Array2i{0, 1}).all());
}


TEST(NeighborsTest, FirstAtomHasNoNeighbor) {
    Names_t names{{"H", "H", "H", "H"}};
    Positions_t positions(3, 4);
    positions << 0, 7, 0, 0,
                 0, 0, 7, 6,
                 0, 0, 0, 0;

    Atoms atoms(names, positions);
    NeighborList neighbor_list;
    neighbor_list.update(atoms);

    auto &[seed, neighbors]{neighbor_list.update(atoms, 5.0)};

    EXPECT_EQ(neighbor_list.nb_neighbors(), 2);

    EXPECT_EQ(neighbor_list.nb_neighbors(0), 0);
    EXPECT_EQ(neighbor_list.nb_neighbors(1), 0);
    EXPECT_EQ(neighbor_list.nb_neighbors(2), 1);
    EXPECT_EQ(neighbor_list.nb_neighbors(3), 1);

    EXPECT_TRUE((seed == Eigen::Array<int, 5, 1>{0, 0, 0, 1, 2}).all());
    EXPECT_TRUE((neighbors == Eigen::Array<int, 2, 1>{3, 2}).all());

    for (auto[i, j]: neighbor_list) {
        if (i == 2) EXPECT_EQ(j, 3);
        if (i == 3) EXPECT_EQ(j, 2);
    }
}


TEST(NeighborsTest, LastAtomHasNoNeighbor) {
    Names_t names{{"H", "H", "H"}};
    Positions_t positions(3, 3);
    positions << 0, 0, 0,
                 7, 6, 0,
                 0, 0, 0;

    Atoms atoms(names, positions);
    NeighborList neighbor_list;
    neighbor_list.update(atoms);

    auto &[seed, neighbors]{neighbor_list.update(atoms, 5.0)};

    EXPECT_EQ(neighbor_list.nb_neighbors(), 2);

    EXPECT_EQ(neighbor_list.nb_neighbors(0), 1);
    EXPECT_EQ(neighbor_list.nb_neighbors(1), 1);
    EXPECT_EQ(neighbor_list.nb_neighbors(2), 0);

    EXPECT_TRUE((seed == Eigen::Array<int, 4, 1>{0, 1, 2, 2}).all());
    EXPECT_TRUE((neighbors == Eigen::Array<int, 2, 1>{1, 0}).all());

    for (auto[i, j]: neighbor_list) {
        if (i == 0) EXPECT_EQ(j, 1);
        if (i == 1) EXPECT_EQ(j, 0);
    }
}


TEST(NeighborsTest, AtomsHaveNoNeighbors) {
    Names_t names{{"H", "H", "H", "H"}};
    Positions_t positions(3, 4);
    positions << 0, 7, 0, 0,
                 0, 0, 7, 6,
                 0, 0, 0, 0;

    Atoms atoms(names, positions);
    NeighborList neighbor_list;  // this is below the smallest distance
    neighbor_list.update(atoms);

    auto &[seed, neighbors]{neighbor_list.update(atoms, 0.5)};

    EXPECT_EQ(neighbor_list.nb_neighbors(), 0);

    EXPECT_EQ(neighbor_list.nb_neighbors(0), 0);
    EXPECT_EQ(neighbor_list.nb_neighbors(1), 0);
    EXPECT_EQ(neighbor_list.nb_neighbors(2), 0);
    EXPECT_EQ(neighbor_list.nb_neighbors(3), 0);

    int n{0};
    for (auto[i, j]: neighbor_list) {
        n++;
    }
    EXPECT_EQ(n, 0);
}


