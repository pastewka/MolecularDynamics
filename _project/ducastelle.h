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

#ifndef YAMD_DUCASTELLE_H
#define YAMD_DUCASTELLE_H

#include "atoms.h"
#include "neighbors.h"

/*
 * This is the embedded atom method potential described in
 *     Ducastelle, "Modules élastiques des métaux de transition", J. Phys. 31, 1055 (1970)
 *     Gupta, "Lattice relaxation at a metal surface", Phys. Rev. B 23, 6265 (1981)
 *     Cleri, Rosato, "Tight-binding potentials for transition metals and alloys", Phys. Rev. B 48, 22 (1993)
 * The default values for the parameters are the Au parameters from Cleri & Rosato's paper.
 */
double ducastelle(Atoms &atoms, const NeighborList &neighbor_list, double cutoff = 10.0, double A = 0.2061,
                  double xi = 1.790, double p = 10.229, double q = 4.036, double re = 4.079 / sqrt(2));

#endif //YAMD_GUPTA_H
