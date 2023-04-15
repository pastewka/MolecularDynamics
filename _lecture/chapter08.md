---
layout: default
title: "Chapter 08"
parent: Lecture
date: 2023-04-15
categories: lecture
author: Lars Pastewka
nav_order: 08
---

<h2 class='chapterHead'><span class='titlemark'>Chapter 8</span><br />
<a id='x1-10008'></a>Periodicity and pressure</h2>
<div class='framedenv' id='shaded*-1'><!--    l. 3    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Context:</span></span> Now that we have introduced the notion of the simulation domain, we can introduce <span class='cmti-12'>periodicity</span>. In a periodic domain, atoms on the right interact with atoms on the left. Periodic domains are commonly used to represent bulk solid or fluid materials and eliminate the effect of interfaces or surfaces. They are sometimes referred to as <span class='cmti-12'>representative volume elements
(RVEs)</span>. The state of the RVE depends on its volume (or the full domain matrix) and we can introduce the conjugate variable, the pressure (or stress tensor).</p>
</div>
<h3 class='sectionHead'><span class='titlemark'>8.1</span> <a id='x1-20008.1'></a>Periodicity</h3>
<!--    l. 9    -->
<p class='noindent'>All energy expressions here are written as \(E_\text {pot}(\{ {\vec {r}}_{i} \})\). If there is no periodic interaction across the domain boundaries, the energies depends on positions only. For periodic interactions, they depend explicity on the domain vectors \(\vec {a}_{1}\), \(\vec {a}_{2}\) and \(\vec {a}_{3}\) or the corresponding matrix \(\underline {h}\): \(E_\text {pot} = E_\text {pot}(\underline {h};\{ \vec {s}_{i} \})\), where we have expressed the positions in the respective
scaled coordinates.</p>
<!--    l. 14    -->
<p class='indent'>Periodicity is implicitly contained in the way the potential is written. E.g. for a pair potential, \begin {equation} E_\text {pot}( \{ \vec {r}_{i} \}) = \frac {1}{2} \sum _{i=1}^N \sum _{j=1}^N \phi (r_{ij}) = \sum _{i &lt; j} \phi (r_{ij}), \label {eq:epotpair2} \end {equation}<a id='x1-2001r1'></a> the energy depends only on relative positions \(\vec {r}_{ij}\). This distance vector contains the information of the simulation domain implicitly. All potential energy expressions must be
a function of relative positions, never of absolute positions, since this would violate momentum conservation (Newton’s second law). (This is only true in the absence of external field, such as gravity of magnetic fields, that break this symmetry.) Note that in terms of the matrix \(\underline {h}\) and the scaled positions, the potential energy becomes \begin {equation} E_\text {pot}(\underline {h};\{ \vec {s}_{i} \}) = \sum _{i &lt; j} \phi (|\underline {h}\cdot \vec {s}_{ij}|), \end {equation}<a id='x1-2002r2'></a> where \(\vec {s}_{ij}=\vec {s}_i - \vec {s}_j\) is the scaled distance between the atoms \(i\) and \(j\).</p>
<!--    l. 25    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>8.2</span> <a id='x1-30008.2'></a>Ghost atoms</h3>
<!--    l. 27    -->
<p class='noindent'>There are different ways to realize periodic domains in MD simulation codes. A common way is to incorporate periodicity into the neighbor list, which is then build to include neighbors across domain boundaries with the correct distance vector \(\vec {r}_{ij}\). The potential energy of the system is then given by Eq. \eqref{eq:epotpair2}.</p>
<!--    l. 29    -->
<p class='indent'>An alternative is to construct a supercell of the domain, i.e. replicate the domain such that the repeating periodic image is explicitly present. In practice, this is equivalent to creating a region of ghost atoms around the domain and this is naturally incorporated into the portion of the code handling the domain decomposition (that was described in the previous chapter). In this case, the potential energy is given by \begin {equation} E_\text {pot}( \{ \vec {r}_{i} \}) = \frac {1}{2}
\sum _{i=1}^N \sum _{j=1}^{N + N_\text {G}} \phi (r_{ij}), \end {equation}<a id='x1-3001r3'></a> where it is important to realize that the sum over \(i\) runs over all atoms in the domain, while the sum over \(j\) runs over the \(N\) domain atoms as well as the \(N_\text {G}\) ghost atoms with index \(j\in [N+1,N_\text {G}]\). This strategy is for example found in the widely used MD code <span class='cmcsc-10x-x-120'>LAMMPS</span> (<a href='#Xplimpton_fast_1995'>Plimpton</a>, <a href='#Xplimpton_fast_1995'>1995</a>; <a href='#XThompson2022-qg'>Thompson et al.</a>, <a href='#XThompson2022-qg'>2022</a>).</p>
<!--    l. 35    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>8.3</span> <a id='x1-40008.3'></a>Pressure and stress</h3>
<!--    l. 37    -->
<p class='noindent'>For a periodic domain (an RVE) we can ask what the pressure of the system is. From a thermodynamic perspective, the (potential energy contribution to the) pressure is given by \begin {equation} P = -\frac {\partial E_\text {pot}}{\partial V}, \end {equation}<a id='x1-4001r4'></a> where \(V=\det \underline {h}\) is the volume of the domain. This expression is only valid at zero temperature; at finite temperature we need to use the free energy \(A\) rather than the potential energy
\(E_\text {pot}\) and this yields an additional kinetic contribution to the pressure.</p>
<!--    l. 43    -->
<p class='indent'>When working with solids, we are often interested in the full stress tensor \(\underline {\sigma }\) of the system rather than just the pressure, \(P=-\tr \underline {\sigma }/3\). Let us assume our RVE undergoes a deformation. This means the domain matrix is taken from \(\underline {h}\) to \(\underline {h}^\prime = \underline {F}\cdot \underline {h}\) where \(\underline {F}\) is called the <span class='cmti-12'>deformation gradient</span>. The Cauchy stress is then given by \begin
{equation} \underline {\sigma } = \frac {1}{V} \left . \frac {\partial E_\text {pot}(\underline {h};\{ \vec {s}_{i} \})}{\partial \underline {F}} \right |_{\underline {F}=\underline {1}}, \label {eq:stress-zero-temperature} \end {equation}<a id='x1-4002r5'></a> where the derivative with respect to the deformation gradient \(\underline {F}\) is taken component-wise. (Again, at finite temperature the relevant thermodynamic potential is the free energy \(A\).)</p>
<!--    l. 51    -->
<p class='indent'>Evaluating the stress for Eq. \eqref{eq:epotpair2} yields \begin {equation} \underline {\sigma } = \frac {1}{V} \sum _{i &lt; j} \left .\frac {\partial \phi (|\underline {F}\cdot \vec {r}_{ij}|)}{\partial \underline {F}} \right |_{\underline {F}=\underline {1}} = \frac {1}{V} \sum _{i &lt; j} \frac {\partial \phi }{\partial r_{ij}} \vec {r}_{ij} \otimes \vec {r}_{ij} \label {eq:stress1} \end {equation}<a id='x1-4003r6'></a> where \(\otimes \) is the outer product, \([\vec {a}\otimes
\vec {b}]_{ij} = a_i b_j\). We can write this as \begin {equation} \underline {\sigma } = \frac {1}{V} \sum _{i &lt; j} \vec {r}_{ij} \otimes \vec {f}_{ij} \label {eq:stress2} \end {equation}<a id='x1-4004r7'></a> where \(\vec {f}_{ij}\) is the force between atoms \(i\) and \(j\). Equation Eq. \eqref{eq:stress2} can be directly used to compute the stress of a pair-potential in molecular dynamics. An identical expression also holds for EAM potentials.</p>
<h2 class='likechapterHead'><a id='x1-5000'></a>Bibliography</h2>
<div class='thebibliography'>
<p class='bibitem'><span class='biblabel'><a id='Xplimpton_fast_1995'></a><span class='bibsp'>   </span></span>S. Plimpton. Fast parallel algorithms for short-range molecular dynamics. <span class='cmti-12'>J. Comput. Phys.</span>, 117(1):1–19, 1995. URL <a class='url' href='https://doi.org/10.1006/jcph.1995.1039'><span class='cmtt-12'>https://doi.org/10.1006/jcph.1995.1039</span></a>.</p>
<p class='bibitem'><span class='biblabel'><a id='XThompson2022-qg'></a><span class='bibsp'>   </span></span>A. P. Thompson, H. M. Aktulga, R. Berger, D. S. Bolintineanu, W. M. Brown, P. S. Crozier, P. J. in <span class='tcrm-1200'>'</span>t Veld, A. Kohlmeyer, S. G. Moore, T. D. Nguyen, R. Shan, M. J. Stevens, J. Tranchida, C. Trott, and S. J. Plimpton. LAMMPS - a flexible simulation tool for particle-based
materials modeling at the atomic, meso, and continuum scales. <span class='cmti-12'>Comput. Phys. Comm.</span>, 271:108171, 2022. URL <a class='url' href='https://doi.org/10.1016/j.cpc.2021.108171'><span class='cmtt-12'>https://doi.org/10.1016/j.cpc.2021.108171</span></a>.</p>
</div>
