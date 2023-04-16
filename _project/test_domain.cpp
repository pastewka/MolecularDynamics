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

#include <mpi.h>
#include <gtest/gtest.h>

#include "atoms.h"
#include "domain.h"
#include "ducastelle.h"
#include "neighbors.h"
#include "units.h"
#include "verlet.h"
#include "xyz.h"

// This test works at most with 4 mpi processes
TEST(DomainDecomposition, enable_disable) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (!(size == 1 || size == 2 || size == 4)) {
        return;
    }

    // Test that enabling decomposes the domain and disabling it returns to the original state. The system here
    // consists of four atoms arranged in a line.
    Atoms atoms(4);
    atoms.positions.col(0) << 1, 1, 1;
    atoms.positions.col(1) << 3, 1, 1;
    atoms.positions.col(2) << 5, 1, 1;
    atoms.positions.col(3) << 7, 1, 1;

    atoms.masses.setRandom();
    atoms.velocities.setRandom();
    atoms.forces.setRandom();

    Atoms global_atoms{atoms};

    // Domain decomposition with boundary at 2
    Domain comm(MPI_COMM_WORLD, {8, 2, 2}, {size, 1, 1}, {0, 0, 0});

    // Is the total (global) number of atoms correct?
    ASSERT_EQ(atoms.nb_atoms(), 4);

    comm.enable(atoms);

    // Is the total (local) number of atoms correct?
    ASSERT_EQ(atoms.nb_atoms(), 4 / comm.size());

    // Check whether masses, positions, velocities and forces have been transferred correctly
    for (int i = 0; i < atoms.nb_atoms(); ++i) {
        auto j{i + comm.rank() * atoms.nb_atoms()};  // global index
        EXPECT_EQ(atoms.masses(i), global_atoms.masses(j));
        EXPECT_EQ(atoms.positions(0, i), global_atoms.positions(0, j));
        EXPECT_EQ(atoms.positions(1, i), global_atoms.positions(1, j));
        EXPECT_EQ(atoms.positions(2, i), global_atoms.positions(2, j));
        EXPECT_EQ(atoms.velocities(0, i), global_atoms.velocities(0, j));
        EXPECT_EQ(atoms.velocities(1, i), global_atoms.velocities(1, j));
        EXPECT_EQ(atoms.velocities(2, i), global_atoms.velocities(2, j));
        EXPECT_EQ(atoms.forces(0, i), global_atoms.forces(0, j));
        EXPECT_EQ(atoms.forces(1, i), global_atoms.forces(1, j));
        EXPECT_EQ(atoms.forces(2, i), global_atoms.forces(2, j));
    }

    comm.disable(atoms);

    // Is the total (global) number of atoms correct?
    ASSERT_EQ(atoms.nb_atoms(), 4);

    // Check whether masses, positions, velocities and forces have been transferred correctly
    for (int i = 0; i < atoms.nb_atoms(); ++i) {
        EXPECT_EQ(atoms.masses(i), global_atoms.masses(i));
        EXPECT_EQ(atoms.positions(0, i), global_atoms.positions(0, i));
        EXPECT_EQ(atoms.positions(1, i), global_atoms.positions(1, i));
        EXPECT_EQ(atoms.positions(2, i), global_atoms.positions(2, i));
        EXPECT_EQ(atoms.velocities(0, i), global_atoms.velocities(0, i));
        EXPECT_EQ(atoms.velocities(1, i), global_atoms.velocities(1, i));
        EXPECT_EQ(atoms.velocities(2, i), global_atoms.velocities(2, i));
        EXPECT_EQ(atoms.forces(0, i), global_atoms.forces(0, i));
        EXPECT_EQ(atoms.forces(1, i), global_atoms.forces(1, i));
        EXPECT_EQ(atoms.forces(2, i), global_atoms.forces(2, i));
    }

}

// This test works at most with 4 mpi processes
TEST(DomainDecomposition, exchange_atoms) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (!(size == 1 || size == 2 || size == 4)) {
        return;
    }

    for (int dim{0}; dim < 3; ++dim) {
        // Test of the `exchange_atoms` method that communicates atomic positions to neighboring cells.
        Atoms atoms(4);
        atoms.positions.setOnes();
        atoms.masses.setRandom();
        atoms.velocities.setRandom();
        atoms.forces.setRandom();

        // Domain decomposition with boundary at 2 and periodicity
        Eigen::Array3d domain_length{2 * Eigen::Array3d::Ones()};
        Eigen::Array3i decomposition{Eigen::Array3i::Ones()};
        Eigen::Array3i periodicity{Eigen::Array3i::Zero()};
        domain_length(dim) = 8;
        decomposition(dim) = size;
        periodicity(dim) = 1;
        Domain comm(MPI_COMM_WORLD, domain_length, decomposition, periodicity);

        // Is the total (global) number of atoms correct?
        ASSERT_EQ(atoms.nb_atoms(), 4);

        // Enable domain decompositio
        comm.enable(atoms);

        // All atoms are now on rank 0. We move them outside of the boundary to understand
        // if exchange_atoms is able to move them to the correct MPI-process.
        if (comm.rank() == 0) {
            // Is the total (local) number of atoms correct?
            ASSERT_EQ(atoms.nb_atoms(), 4);
            ASSERT_EQ(comm.nb_local(), atoms.nb_atoms());

            atoms.positions(dim, 0) = 1;
            atoms.positions(dim, 1) = 3;
            atoms.positions(dim, 2) = 5;
            atoms.positions(dim, 3) = 7;
        }
        else {
            // Is the total (local) number of atoms correct?
            ASSERT_EQ(atoms.nb_atoms(), 0);
            ASSERT_EQ(comm.nb_local(), atoms.nb_atoms());
        }

        // Communicate atoms.
        comm.exchange_atoms(atoms);

        // Is the total (local) number of atoms correct?
        ASSERT_EQ(atoms.nb_atoms(), 4 / comm.size());
        ASSERT_EQ(comm.nb_local(), atoms.nb_atoms());

        // Now we move the leftmost atom to the left such that is leaves the domain.
        if (comm.coordinate(dim) == 0) {
            atoms.positions(dim, 0) -= 1.5;
        }

        // Communicate atoms.
        comm.exchange_atoms(atoms);

        // Check number of atoms on each MPI process.
        if (comm.size() > 1) {
            if (comm.coordinate(dim) == 0) {
                ASSERT_EQ(atoms.nb_atoms(), 4 / comm.size() - 1);
            } else if (comm.coordinate(dim) == comm.size() - 1) {
                ASSERT_EQ(atoms.nb_atoms(), 4 / comm.size() + 1);
            }
        }

        // Check that all atoms are within their respective local domains.
        for (auto &&position: atoms.positions.colwise()) {
            EXPECT_GE(position(dim), comm.rank() * comm.domain_length(dim) / comm.size());
            EXPECT_LT(position(dim), (comm.rank() + 1) * comm.domain_length(dim) / comm.size());
        }

        // Check that exactly one atom is at position domain_length - 0.5.
        int n = 0;
        for (auto &&position: atoms.positions.colwise()) {
            if (std::abs(position(dim) - (comm.domain_length(dim) - 0.5)) < 1e-6) {
                n++;
            }
        }
        int rn;
        MPI_Allreduce(&n, &rn, 1, MPI_INT, MPI_SUM, comm.communicator());
        EXPECT_EQ(rn, 1);
    }
}

// This test works at most with 4 mpi processes
TEST(DomainDecomposition, update_ghosts) {
    constexpr double domain_length{8};

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (!(size == 1 || size == 2 || size == 4)) {
        return;
    }

    for (int dim{0}; dim < 3; ++dim) {
        // Test of the `exchange_atoms` method that communicates atomic positions to neighboring cells.
        Atoms atoms(4);
        atoms.positions.setOnes();
        atoms.positions(dim, 0) = 1;
        atoms.positions(dim, 1) = 3;
        atoms.positions(dim, 2) = 5;
        atoms.positions(dim, 3) = 7;

        atoms.masses.setRandom();
        atoms.velocities.setRandom();
        atoms.forces.setRandom();

        // Domain decomposition with boundary at domain length and periodicity
        Eigen::Array3d domain_length{2 * Eigen::Array3d::Ones()};
        Eigen::Array3i decomposition{Eigen::Array3i::Ones()};
        Eigen::Array3i periodicity{Eigen::Array3i::Zero()};
        domain_length(dim) = 8;
        decomposition(dim) = size;
        periodicity(dim) = 1;
        Domain comm(MPI_COMM_WORLD, domain_length, decomposition, periodicity);

        // Is the total (global) number of atoms correct?
        ASSERT_EQ(atoms.nb_atoms(), 4);

        comm.enable(atoms);

        // Is the total (local) number of atoms correct?
        ASSERT_EQ(atoms.nb_atoms(), 4 / comm.size());
        ASSERT_EQ(comm.nb_local(), atoms.nb_atoms());

        // Communicate ghosts.
        comm.update_ghosts(atoms, 2.5);
        ASSERT_EQ(comm.nb_local(), 4 / comm.size());
        ASSERT_EQ(atoms.nb_atoms(), 4 / comm.size() + 2);
    }
}

class DomainDecompositionTest : public testing::TestWithParam<Eigen::Array3i> {
};

TEST_P(DomainDecompositionTest, Ducastelle_energy_and_forces) {
    constexpr double cutoff = 5.0;  // Cutoff for the Ducastelle potential

    // Get size of communicator group (number of processes)
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout << "Running test for decomposition " << GetParam().transpose() << std::endl;

    if (GetParam().prod() != size) {
        std::cout << "MPI communicator size of " << size << " is incompatible with this decomposition, skipping."
                  << std::endl;
        // Skip test if decomposition is incompatible with number of processes
        return;
    }

    // Read gold cluster and construct Atoms object
    auto[names, positions]{read_xyz("cluster_923.xyz")};
    Atoms atoms{names, positions};

    // We abuse the masses as a unique index so we can find atoms again when decomposed
    atoms.masses = Masses_t::LinSpaced(atoms.nb_atoms(), 0, atoms.nb_atoms() - 1);

    // Compute energy on a single process
    NeighborList neighbor_list;
    neighbor_list.update(atoms, cutoff);
    double epot_ref{ducastelle(atoms, neighbor_list, cutoff)};
    Forces_t forces_ref{atoms.forces};

    // Get minimum and maximum x-position and move cluster into the domain (that starts at x=0)
    constexpr double epsilon = 0.1;  // distance to domain boundary on left and right
    double minx{atoms.positions.row(0).minCoeff()}, maxx{atoms.positions.row(0).maxCoeff()};
    double miny{atoms.positions.row(1).minCoeff()}, maxy{atoms.positions.row(1).maxCoeff()};
    double minz{atoms.positions.row(2).minCoeff()}, maxz{atoms.positions.row(2).maxCoeff()};
    atoms.positions.row(0) -= minx - epsilon;
    atoms.positions.row(1) -= miny - epsilon;
    atoms.positions.row(2) -= minz - epsilon;

    // Domain decomposition with boundary at domain length
    Domain comm(MPI_COMM_WORLD,
                {maxx - minx + 2 * epsilon, maxy - miny + 2 * epsilon, maxz - minz + 2 * epsilon},
                GetParam(), {0, 0, 0});
    comm.enable(atoms);

    // We need to communicate ghost buffers for the correct energy; we need a boundary of twice the cutoff for the
    // EAM potential. (Note: Energies are correct with just the cutoff as a boundary.)
    comm.update_ghosts(atoms, 2 * cutoff);

    // Compute energy of decomposed system
    neighbor_list.update(atoms, cutoff);
    atoms.energies.setZero();
    atoms.forces.setZero();
    ducastelle(atoms, neighbor_list, cutoff);

    // We only sum the energy of the process-local atoms
    double epot{atoms.energies(Eigen::seqN(0, comm.nb_local())).sum()};

    // Sum energies of each process
    double epot_sum;
    MPI_Allreduce(&epot, &epot_sum, 1, MPI_DOUBLE, MPI_SUM, comm.communicator());

    EXPECT_NEAR(epot_ref, epot_sum, 1e-10);

    // Check forces on each process; only the process local atoms have valid forces
    for (int i{0}; i < comm.nb_local(); ++i) {
        Eigen::Index unique_index{static_cast<Eigen::Index>(atoms.masses(i))};
        EXPECT_NEAR(atoms.forces(0, i), forces_ref(0, unique_index), 1e-10);
        EXPECT_NEAR(atoms.forces(1, i), forces_ref(1, unique_index), 1e-10);
        EXPECT_NEAR(atoms.forces(2, i), forces_ref(2, unique_index), 1e-10);
    }
}

TEST_P(DomainDecompositionTest, Ducastelle_moving_cluster) {
    constexpr double cutoff = 5.0;  // Cutoff for the Ducastelle potential

    // Get size of communicator group (number of processes)
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout << "Running test for decomposition " << GetParam().transpose() << std::endl;

    if (GetParam().prod() != size) {
        std::cout << "MPI communicator size of " << size << " is incompatible with this decomposition, skipping."
                  << std::endl;
        // Skip test if decomposition is incompatible with number of processes
        return;
    }

    // Read gold cluster and construct Atoms object
    auto[names, positions]{read_xyz("cluster_923.xyz")};
    Atoms atoms{names, positions};

    // Compute energy on a single process
    NeighborList neighbor_list;
    neighbor_list.update(atoms, cutoff);
    double epot_ref{ducastelle(atoms, neighbor_list, cutoff)};

    // Get minimum and maximum x-position and move cluster into the domain (that starts at x=0)
    constexpr double vacuum = 2.5;  // distance to domain boundary on left and right
    double minx{atoms.positions.row(0).minCoeff()}, maxx{atoms.positions.row(0).maxCoeff()};
    double miny{atoms.positions.row(1).minCoeff()}, maxy{atoms.positions.row(1).maxCoeff()};
    double minz{atoms.positions.row(2).minCoeff()}, maxz{atoms.positions.row(2).maxCoeff()};
    atoms.positions.row(0) -= minx - vacuum;
    atoms.positions.row(1) -= miny - vacuum;
    atoms.positions.row(2) -= minz - vacuum;

    // We set constant velocities and zero forces so the cluster simply translates.
    atoms.velocities.setOnes();
    atoms.velocities *= 2;

    // Domain decomposition with periodic boundary at domain length
    Domain comm(MPI_COMM_WORLD,
                {maxx - minx + 2 * vacuum, maxy - miny + 2 * vacuum, maxz - minz + 2 * vacuum},
                GetParam(), {1, 1, 1});
    comm.enable(atoms);
    atoms.forces.setZero();

    for (int step{0}; step < 5; ++step) {
        // Position update.
        verlet_step1(atoms, 1.0);

        // We need to communicate ghost buffers for the correct energy; we need a boundary of twice the cutoff for the
        // EAM potential. (Note: Energies are correct with just the cutoff as a boundary.)
        comm.exchange_atoms(atoms);
        comm.update_ghosts(atoms, 2 * cutoff);

        // Compute energy of decomposed system
        neighbor_list.update(atoms, cutoff);
        atoms.energies.setZero();
        atoms.forces.setZero();
        ducastelle(atoms, neighbor_list, cutoff);
        atoms.forces.setZero();  // constant velocity

        // We only sum the energy of the process-local atoms
        double epot{atoms.energies(Eigen::seqN(0, comm.nb_local())).sum()};

        // Sum energies of each process
        double epot_sum;
        MPI_Allreduce(&epot, &epot_sum, 1, MPI_DOUBLE, MPI_SUM, comm.communicator());

        // Check that the energy does not change since the cluster is translating.
        ASSERT_NEAR(epot_ref, epot_sum, 1e-10);

        // Velocity correction. (Will do nothing here.)
        verlet_step2(atoms, 1.0);
    }
}

INSTANTIATE_TEST_SUITE_P(CycleThroughDecompositions, DomainDecompositionTest,
                         testing::Values(Eigen::Array3i{1, 1, 1},
                                         Eigen::Array3i{2, 1, 1},
                                         Eigen::Array3i{1, 2, 1},
                                         Eigen::Array3i{1, 1, 2},
                                         Eigen::Array3i{4, 1, 1},
                                         Eigen::Array3i{1, 4, 1},
                                         Eigen::Array3i{1, 1, 4},
                                         Eigen::Array3i{2, 1, 2},
                                         Eigen::Array3i{1, 2, 2},
                                         Eigen::Array3i{8, 1, 1},
                                         Eigen::Array3i{4, 2, 1},
                                         Eigen::Array3i{2, 2, 2}));
