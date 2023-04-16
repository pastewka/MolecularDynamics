---
layout: default
title: "Chapter 01"
parent: Lecture
date: 2023-04-16
categories: lecture
author: Lars Pastewka
nav_order: 01
---
---

<h2 class='chapterHead'><span class='titlemark'>Chapter 1</span><br />
<a id='x1-10001'></a>Introduction</h2>
<div class='framedenv' id='shaded*-1'><!--    l. 4    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Context:</span></span> We start by introducing the concept of the potential energy and the interatomic force. Those are the central ingredients to the molecular dynamics simulation method.</p>
</div>
<div class='framedenv' id='shaded*-1'><!--    l. 8    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Additional resources:</span></span></p>
<ul class='itemize1'>
<li class='itemize'>Chapter 1 of Allen &amp; Tildesley, Computer Simulation of Liquids</li>
<li class='itemize'>Chapters 1 and 2 of <a href='https://doi.org/10.1080/23746149.2022.2093129'>Interatomic potentials: Achievements and challenges</a></li>
</ul>
</div>
<h3 class='sectionHead'><span class='titlemark'>1.1</span> <a id='x1-20001.1'></a>Structure of matter at the atomic scale</h3>
<!--    l. 17    -->
<p class='noindent'>All matter is build out of quark and leptons, or perhaps even smaller particles, but for the sake of modeling the real material world the atom is the fundamental unit. Atoms can be described by nuclei and electrons or through “coarse-grained” models that ignore the fact that there are electrons. Both types of models are useful for describing materials, and the latter ones will be extensively used in this class.</p>
<!--    l. 19    -->
<p class='indent'>Atoms in solids can arrange in different configurations that are called crystals when there is long-ranged order or glasses when there is not. (All solid matter typically has short-ranged order that is determined by the chemical bonds between atoms.) Atoms in solids are immobile and self-diffusion is limited. Conversely, liquids and gases are disordered (like glasses) but have mobile constituent atoms. Macroscopic object typically contain a lot of atoms – on the order of Avogadro’s
constant \(N_A\approx 6\times 10^{23}\). The atomic-scale simulation techniques discussed in this class can at the time of this writing (the 2020s) casually treat on order of \(\sim 10^7\) atoms. This can be pushed to \(10^{13}\) <a href='#XEckhardt2013-oh'>Eckhardt et al.</a> (<a href='#XEckhardt2013-oh'>2013</a>); <a href='#XTchipev2019-fl'>Tchipev et al.</a> (<a href='#XTchipev2019-fl'>2019</a>) if you use the biggest computers available to us (in combination with
computationally-efficient models). Of course, this boundary moves towards larger systems as computer technology evolves.</p>
<!--    l. 23    -->
<p class='indent'>Nowadays, we can even observe matter at atomic scales and “see” individual atoms. The <a href='https://www.sfb-transregio103.de/'>collaborative research center 103</a> has produced an extremely instructive video on the structure of specific type of alloys, dubbed “superalloy”, that is used in e.g. turbine blades. Enjoy the ride from the blade to the atom. This class is about modeling matter at the smallest scales that you see in this video.</p>
<!--    l. 25    -->
<p class='indent'><a class='url' href='https://www.youtube.com/embed/wYHch5QIWTQ'><span class='cmtt-12'>https://www.youtube.com/embed/wYHch5QIWTQ</span></a></p>
<!--    l. 27    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>1.2</span> <a id='x1-30001.2'></a>Interatomic forces and the potential energy</h3>
<!--    l. 29    -->
<p class='noindent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=a12a8064-84e7-47f6-80ab-ad15014bae8a'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=a12a8064-84e7-47f6-80ab-ad15014bae8a</span></a></p>
<!--    l. 31    -->
<p class='indent'>Atoms interact via forces. As Feynman put it in his famous lectures on physics, the fundamental truth about man’s understanding of the physical world is “that all things are made of atoms – little particles that move around in perpetual motion, attracting each other when they are a little distance apart, but repelling upon being squeezed into one another” <a href='#XFeynman1964Book'>Feynman et al.</a> (<a href='#XFeynman1964Book'>1964</a>). Indeed this statement summarizes
the essence of the molecular dynamics simulation method.</p>
<!--    l. 33    -->
<p class='indent'>As the simplest example why atoms attract each other, let us consider the example of simple salt, e.g. Na-Cl that we all have sitting in our kitchen. Na-Cl in its solid form is an ionic crystal. Na atoms have approximately a charge of \(q_{\text {Na}}=+1|e|\), where \(e\) is the electron charge, and Cl atoms have a charge of approximately \(q_{\text {Na}}=-1|e|\). The Coulomb interaction between these atoms is a fundamental force of nature. Basic physical principles tell us, that the
interaction energy between a Na and a Cl atom is given by \begin {equation} V_{\text {Coulomb}}(r;q_{\text {Na}},q_{\text {Cl}}) = \frac {1}{4\pi \varepsilon _0} \frac {q_{\text {Na}} q_{\text {Cl}}}{r}. \end {equation}<a id='x1-3001r1'></a> We also know that this energy is pair-wise additive, allowing us to write down the Coulomb interaction energy for Na-Cl consisting of \(N\) atoms, \begin {equation} E_{\text {Coulomb}}(\{\vec {r}_i\}) = \sum _{i=1}^N \sum _{j=i+1}^N V_{\text {Coulomb}}(r_{ij};q_i, q_j)
= \frac {1}{4\pi \varepsilon _0} \sum _{i=1}^N \sum _{j=i+1}^N \frac {q_i q_j}{r_{ij}} \label {eqn:coulomb} \end {equation}<a id='x1-3002r2'></a> where \(q_i\) is the charge on atom \(i\) and \(r_{ij}=|\vec {r}_i - \vec {r}_j|\) the distance between atom \(i\) and \(j\). Note that we have introduced — in passing — a central quantity of the molecular dynamics method, the atomic positions \(\vec {r}_i\) and Eq. \eqref{eqn:coulomb} indicates that the interaction energy depends on the positions of all
atoms.</p>
<!--    l. 44    -->
<p class='indent'>The Coulomb interaction has a singularity at \(r\to 0\). The attractive force between opposite charges becomes infinitely large. The salt crystal does not collapse because atoms are, as Feynman puts it, “repelling upon being squeezed into one another”. While the attraction between our Na and Cl atoms are described by a fundamental force of nature, it is more difficult to understand the origin of this repulsion. Hand-wavingly, it goes back to the fact that electrons are Fermions and
electrons from the electron shells of Na and Cl therefore cannot exist at the same point in space (and the same spin state). This is the Pauli exclusion principle, and the resulting repulsive force is called Pauli repulsion.</p>
<!--    l. 46    -->
<p class='indent'>Different models for the Pauli repulsion exist. While the Coulomb interaction is a fundamental force of nature, these models are approximations to the true quantum physics that is the origin of the repulsive form. Two common forms are exponential repulsion, \begin {equation} E_{\text {rep,exp}}(\{\vec {r}_i\}) = \sum _{i=1}^N \sum _{j=i+1}^N A e^{-r/\rho }, \end {equation}<a id='x1-3003r3'></a> or an algebraic repulsion of the form \begin {equation} E_{\text {rep,12}}(\{\vec {r}_i\}) =
\sum _{i=1}^N \sum _{j=i+1}^N A r^{-12}. \end {equation}<a id='x1-3004r4'></a> Note that \(A\) and \(\rho \) are <span class='cmti-12'>parameters</span>, that need to be somehow determined. This can be done with the help of either experimental data or <span class='cmti-12'>first-principles</span> calculations, that treat the electrons explicitly. These parameters depend on the atom types under consideration and, in contrast to the parameter that show up in the Coulomb interaction (the permittivity
\(\varepsilon _0\)), they are not universal.</p>
<!--    l. 56    -->
<p class='indent'>For our Na-Cl model, we combine Coulomb interaction with an exponential repulsion, to give the total energy \begin {equation} E_{\text {pot}}(\{\vec {r}_i\}) = \sum _{i=1}^N \sum _{j=i+1}^N \left (A_{ij} e^{-r_{ij}/\rho _{ij}} + \frac {1}{4\pi \varepsilon _0} \frac {q_i q_j}{r_{ij}}\right ). \label {eqn:NaCl} \end {equation}<a id='x1-3005r5'></a> This energy is called the <span class='cmti-12'>potential energy</span> and is the central property of an atomic-scale model. With
Eq. \eqref{eqn:NaCl}, we have also encountered our first atomic-scale model for a real material, called the <span class='cmti-12'>Born-Mayer</span> potential. Potentials that can be decomposed as Eq. \eqref{eqn:NaCl} into pair-wise terms are called <span class='cmti-12'>pair potentials</span>. They are often written as \begin {equation} E_{\text {pot}}(\{\vec {r}_i\}) = \sum _{i=1}^N \sum _{j=i+1}^N V(r_{ij}), \end {equation}<a id='x1-3006r6'></a> with \begin {equation} V(r_{ij}) = A_{ij}
e^{-r_{ij}/\rho _{ij}} + \frac {1}{4\pi \varepsilon _0} \frac {q_i q_j}{r_{ij}} \end {equation}<a id='x1-3007r7'></a> for the above potential. The quantity \(V(r_{ij})\) is called the pair interaction energy.</p>
<!--    l. 71    -->
<p class='indent'>The most famous pair-potential is likely the <span class='cmti-12'>Lennard-Jones</span> potential. (Unlike Born and Mayer, Lennard-Jones is a single person.) Its pair interaction energy is given by \begin {equation} V(r_{ij}) = 4\varepsilon \left [\left (\frac {\sigma }{r}\right )^{12} - \left (\frac {\sigma }{r}\right )^6\right ]. \end {equation}<a id='x1-3008r8'></a> The repulsive term \(\propto r^{-12}\) is one of the models for Pauli repulsion discussed above. The attractive term
\(\propto r^{-6}\) arises from <span class='cmti-12'>London dispersion interactions</span>. Dispersion forces exist between all atoms, even uncharged molecules or noble gases. They are widely employed for the nonbonded portion of what are called <span class='cmti-12'>valence</span> <span class='cmti-12'>force-fields</span>. Simple Lennard-Jones systems are often used to study generic phenomena found in real materials, e.g. the glass transition or plasticity of amorphous materials. However, there are
limitations to pair potentials, and more sophisticated potential energy models have been developed over the past decades to address shortcomings of simple potentials. In particular, the assumption of <span class='cmti-12'>pair additive</span> intrinsic to the equations of this chapter is often not valid. We will discuss how to lift these limitations in Chapter <span class='cmbx-12'>??</span>.</p>
<div class='framedenv' id='shaded*-1'><!--    l. 77    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Note:</span></span> A repulsive term of the form \(r^{-12}\) is advantageous from a simulation point of view, since it is faster to compute than an exponential. This has helped popularize the Lennard-Jones potential in the early days of molecular dynamics simulations.</p>
</div>
<!--    l. 81    -->
<p class='indent'>Writing the potential energy of a system of particles allows the derivation of the forces acting on these particles: \begin {equation} \vec {f}_k = -\frac {\partial }{\partial \vec {r}_k} E_{\text {pot}}(\{\vec {r}_i\}). \label {eq:forces} \end {equation}<a id='x1-3009r9'></a> These forces are the essential ingredient to <span class='cmti-12'>molecular dynamics</span>, as they determine the motion of the atoms, in accordance to Newton’s second law.</p>
<!--    l. 88    -->
<p class='indent'>The potential energy itself describes what is called the <span class='cmti-12'>potential energy</span> <span class='cmti-12'>landscape</span>. The potential energy landscape depends on \(3N\) degrees of freedom (as compared to the landscape we experience while walking, which depends on \(2\) degrees of freedom); it is therefore an object that is complex to visualize. Simplifying some of its aspects is the core of <span class='cmti-12'>molecular statics</span>. For example, it is often
important to identify the ground-state of a system; this is the most stable configuration of a material and has the lowest possible potential energy. There is usually some crystal that is lower in energy than the energy of a glass with the same stoichiometry. Yet, in many real-world engineering applications, the materials are not in their crystalline ground-state: the most common material we encounter with this property may be window glass. In molecular statics, we therefore seek to enumerate those
<span class='cmti-12'>local</span> <span class='cmti-12'>minima</span> of the potential energy landscape and the energy barriers between them.</p>
<!--    l. 90    -->
<p class='indent'>Since the dynamics of a molecular system is determined by the forces acting on the individual atoms, we only need to specify the potential energy up to a constant, which disappears in the derivative Eq. \eqref{eq:forces}. We can therefore measure the potential energy with respect to any reference configuration. This reference configuration is often the atomized state of the material, where all the constituent atoms sit individually in vacuum and are not interacting with each other.
If this reference situation is assigned the energy \(0\), then the potential energy is generally negative, because if it was positive the system would spontaneously atomize. (Remember, any physical system evolves to a state of lower energy.)</p>
<h2 class='likechapterHead'><a id='x1-4000'></a>Bibliography</h2>
<div class='thebibliography'>
<p class='bibitem'><span class='biblabel'><a id='XEckhardt2013-oh'></a><span class='bibsp'>   </span></span>W. Eckhardt, A. Heinecke, R. Bader, M. Brehm, N. Hammer, H. Huber, H.-G. Kleinhenz, J. Vrabec, H. Hasse, M. Horsch, M. Bernreuther, C. W. Glass, C. Niethammer, A. Bode, and H.-J. Bungartz. 591 TFLOPS multi-trillion particles simulation on SuperMUC. In <span class='cmti-12'>Supercomputing</span>, pages 1–12. Springer
Berlin Heidelberg, 2013. URL <a class='url' href='https://doi.org/10.1007/978-3-642-38750-0_1'><span class='cmtt-12'>https://doi.org/10.1007/978-3-642-38750-0_1</span></a>.</p>
<p class='bibitem'><span class='biblabel'><a id='XFeynman1964Book'></a><span class='bibsp'>   </span></span>R. P. Feynman, R. B. Leighton, and M. Sands. <span class='cmti-12'>The Feynman Lectures</span> <span class='cmti-12'>on Physics</span>. Addison-Wesley, New York, 1964.</p>
<p class='bibitem'><span class='biblabel'><a id='XTchipev2019-fl'></a><span class='bibsp'>   </span></span>N. Tchipev, S. Seckler, M. Heinen, J. Vrabec, F. Gratl, M. Horsch, M. Bernreuther, C. W. Glass, C. Niethammer, N. Hammer, B. Krischok, M. Resch, D. Kranzlmüller, H. Hasse, H.-J. Bungartz, and P. Neumann. TweTriS: Twenty trillion-atom simulation. <span class='cmti-12'>Int. J. High Perform. Comput. Appl.</span>,
33(5):838–854, Sept. 2019. URL <a class='url' href='https://doi.org/10.1177/1094342018819741'><span class='cmtt-12'>https://doi.org/10.1177/1094342018819741</span></a>.</p>
</div>
