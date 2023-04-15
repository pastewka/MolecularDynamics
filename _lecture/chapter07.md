---
layout: default
title: "Chapter 07"
parent: Lecture
date: 2023-04-15
categories: lecture
author: Lars Pastewka
nav_order: 07
---

<h2 class='chapterHead'><span class='titlemark'>Chapter 7</span><br />
<a id='x1-10007'></a>Domain decomposition</h2>
<div class='framedenv' id='shaded*-1'><!--    l. 3    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Context:</span></span> Parallelization in molecular dynamics typically occurs through <span class='cmti-12'>domain</span> <span class='cmti-12'>decomposition</span>. The simulation domain is divided into subdomains, each of which runs within an MPI process. This distributes the workload among different compute units. Communications occurs only at the interface of the subdomain, either to exchange atoms between subdomains or to communicate
<span class='cmti-12'>ghost atoms</span> that are required for the computation of correct forces in short-range interatomic potentials.</p>
</div>
<h3 class='sectionHead'><span class='titlemark'>7.1</span> <a id='x1-20007.1'></a>Simulation domain</h3>
<!--    l. 9    -->
<p class='noindent'>Our atomic system has so far lived in an infinite space consisting of vaccum. We have made no reference to a simulation domain and the code developed up to Milestone 07 makes not reference to such a domain. We now introduce domain decomposition and for this need a simulation domain, i.e. a region of space \(\Omega \) in which our atoms can reside. This domain can be periodic, which we will discuss in more detail in the next chapter.</p>
<!--    l. 11    -->
<p class='indent'>We will assume that the simulation has its origin at \((0,0,0)\) and is spanned by three linearly independent vectors \(\vec {a}_1\), \(\vec {a}_2\) and \(\vec {a}_3\). Any atomic position can then be expressed as \begin {equation} \vec {r}_i = s_{i,1} \vec {a}_1 + s_{i,2}\vec {a}_2 + s_3 \vec {a}_{i,3} \end {equation}<a id='x1-2001r1'></a> with \(s_\alpha \in [0,1)\). \(s_\alpha \) must remain in this interval since we do not allow atoms outside of the simulation domain. The vector
\(\vec {s}_i\) is the scaled position of the atom \(i\). Using the <span class='cmti-12'>domain matrix</span> \(\underline {h}=(\vec {a}_1, \vec {a}_2, \vec {a}_3)\), we can express this more compactly as \(\vec {r}_i=\underline {h}\cdot \vec {s}_i\). Conversely, we obtain the scaled positions from \(\vec {s}_i=\underline {h}^{-1}\cdot \vec {r}_i\).</p>
<!--    l. 17    -->
<p class='indent'>In what follows, we assume rectilinear domains, i.e. \(\vec {a}_1=(L_x, 0, 0)\), \(\vec {a}_2=(0, L_y, 0)\) and \(\vec {a}_3=(0, 0, L_z)\) where \(L_x\), \(L_y\) and \(L_z\) are the linear dimensions of the domain. The methods that are described in the following are straightforwardly extended to arbitrary (tilted) domains.</p>
<!--    l. 19    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>7.2</span> <a id='x1-30007.2'></a>Decomposition into Cartesian domains</h3>
<!--    l. 21    -->
<p class='noindent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=cee045e7-7cdb-4cda-ad4a-ad6b007de343'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=cee045e7-7cdb-4cda-ad4a-ad6b007de343</span></a></p>
<!--    l. 23    -->
<p class='indent'>We decompose the full system into \(N_x\times N_y\times N_z\) subdomains. For a rectilinear domain, this means teach subdomain has linear dimensions of \(L_x/N_x\), \(L_y/N_y\) and \(L_z/N_z\). Each subdomain propagates its own atoms. When atoms leave the subdomain, they are transferred to the respective neighboring domain. We call this process <span class='cmti-12'>atom</span> <span class='cmti-12'>exchange</span>.</p>
<!--    l. 25    -->
<p class='indent'>Domain decomposition algorithms for MD simulations have started to appear in the literature around 1990. Some of the earliest references to this type of algorithm are <a href='#Xbruge_concurrent_1990'>Brugè and Fornili</a> (<a href='#Xbruge_concurrent_1990'>1990</a>); <a href='#Xliem_molecular_1991'>Liem et al.</a> (<a href='#Xliem_molecular_1991'>1991</a>); <a href='#Xchynoweth_simulation_1991'>Chynoweth et al.</a> (<a href='#Xchynoweth_simulation_1991'>1991</a>); <a href='#Xpinches_large_1991'>Pinches et al.</a> (<a href='#Xpinches_large_1991'>1991</a>); <a href='#Xbrown_domain_1993'>Brown et al.</a> (<a href='#Xbrown_domain_1993'>1993</a>); <a href='#Xplimpton_fast_1995'>Plimpton</a> (<a href='#Xplimpton_fast_1995'>1995</a>).</p>
<!--    l. 27    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>7.3</span> <a id='x1-40007.3'></a>Ghost atoms</h3>
<!--    l. 29    -->
<p class='noindent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=03a83e2f-54d9-4423-acb8-ad6b007de308'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=03a83e2f-54d9-4423-acb8-ad6b007de308</span></a></p>
<!--    l. 31    -->
<p class='indent'>The atoms within each subdomain are not sufficient to compute the forces upon these atoms. In order to compute forces for atoms near the domain boundary, we need to transfer atoms that sit outside of the subdomain from the neighboring subdomains. These atoms are called ghost atoms. All atoms up to a distance \(r_\text {G}\) from the subdomain boundary are transferred. For a Lennard-Jones potential, \(r_\text {G}=r_c\) but for the EAM potential discussed here \(r_\text {G}=2 r_c\). This is
because a force in the EAM potential is affected by an atom that sits twice the cutoff radius \(r_c\) away.</p>
<!--    l. 33    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>7.4</span> <a id='x1-50007.4'></a>Communication pattern</h3>
<!--    l. 35    -->
<p class='noindent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=2ff96a92-b994-4cee-84b6-ad6b008fc0d3'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=2ff96a92-b994-4cee-84b6-ad6b008fc0d3</span></a></p>
<!--    l. 37    -->
<p class='indent'>The basic communication pattern involves two <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>MPI_Sendrecv</span></span></span> commands per Cartesian direction. The atoms that are send (either exchanged or as ghost atoms) must be serialized into a send buffer. Given that serialization has occured into the buffers <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>send_left</span></span></span> and <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>send_right</span></span></span>, the communication pattern looks as follows:</p>
<!--    l. 38    -->
<pre class='lstlisting' id='listing-1'><span class='label'><a id='x1-5001r1'></a><span class='cmr-6'>1</span></span><span style='color:#000000'><span class='cmtt-10'>MPI_Sendrecv</span></span><span style='color:#000000'><span class='cmtt-10'>(&amp;</span></span><span style='color:#000000'><span class='cmtt-10'>send_left</span></span><span style='color:#000000'><span class='cmtt-10'>,</span></span><span style='color:#000000'> <span class='cmtt-10'>left_, </span><span class='cmtt-10'>&amp;recv_right, </span><span class='cmtt-10'>right_, </span><span class='cmtt-10'>comm_)};</span> 
</span><span class='label'><a id='x1-5002r2'></a><span class='cmr-6'>2</span></span><span style='color:#000000'><span class='cmtt-10'>MPI_Sendrecv</span></span><span style='color:#000000'><span class='cmtt-10'>(&amp;</span></span><span style='color:#000000'><span class='cmtt-10'>send_right</span></span><span style='color:#000000'><span class='cmtt-10'>,</span></span><span style='color:#000000'> <span class='cmtt-10'>right_, </span><span class='cmtt-10'>&amp;recv_left, </span><span class='cmtt-10'>left_, </span><span class='cmtt-10'>comm_)};</span></span></pre>
<!--    l. 42    -->
<p class='indent'>Here <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>comm_</span></span></span> contains the MPI communicator and <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>left_</span></span></span> and <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>right_</span></span></span> the MPI ranks of the processes that host the subdomain to the left and the right, respectively, of the current subdomain. The buffers <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>recv_left</span></span></span> and <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>recv_right</span></span></span> hold the serialized atomic information received from the left and right, respectively. This information needs to be deserialized into the respective atom data type.</p>
<h2 class='likechapterHead'><a id='x1-6000'></a>Bibliography</h2>
<div class='thebibliography'>
<p class='bibitem'><span class='biblabel'><a id='Xbrown_domain_1993'></a><span class='bibsp'>   </span></span>D. Brown, J. H. R. Clarke, M. Okuda, and T. Yamazaki. A domain decomposition parallelization strategy for molecular dynamics simulations on distributed memory machines. <span class='cmti-12'>Comput. Phys. Comm.</span>, 74(1):67–80, 1993. URL <a class='url' href='https://doi.org/10.1016/0010-4655(93)90107-N'><span class='cmtt-12'>https://doi.org/10.1016/0010-4655(93)90107-N</span></a>.</p>
<p class='bibitem'><span class='biblabel'><a id='Xbruge_concurrent_1990'></a><span class='bibsp'>   </span></span>F. Brugè and S. L. Fornili. Concurrent molecular dynamics simulation of spinodal phase transition on transputer arrays. <span class='cmti-12'>Comput. Phys. Comm.</span>, 60(1):31–38, 1990. URL <a class='url' href='https://doi.org/10.1016/0010-4655(90)90076-D'><span class='cmtt-12'>https://doi.org/10.1016/0010-4655(90)90076-D</span></a>.</p>
<p class='bibitem'><span class='biblabel'><a id='Xchynoweth_simulation_1991'></a><span class='bibsp'>   </span></span>S. Chynoweth, U. C. Klomp, and L. E. Scales. Simulation of organic liquids using pseudo-pairwise interatomic forces on a toroidal transputer array. <span class='cmti-12'>Comput. Phys. Comm.</span>, 62(2):297–306, 1991. URL <a class='url' href='https://doi.org/10.1016/0010-4655(91)90102-Q'><span class='cmtt-12'>https://doi.org/10.1016/0010-4655(91)90102-Q</span></a>.</p>
<p class='bibitem'><span class='biblabel'><a id='Xliem_molecular_1991'></a><span class='bibsp'>   </span></span>S. Y. Liem, D. Brown, and J. H. R. Clarke. Molecular dynamics simulations on distributed memory machines. <span class='cmti-12'>Comput. Phys. Comm.</span>, 67(2):261–267, 1991. URL <a class='url' href='https://doi.org/10.1016/0010-4655(91)90021-C'><span class='cmtt-12'>https://doi.org/10.1016/0010-4655(91)90021-C</span></a>.</p>
<p class='bibitem'><span class='biblabel'><a id='Xpinches_large_1991'></a><span class='bibsp'>   </span></span>M. R. S. Pinches, D. J. Tildesley, and W. Smith. Large scale molecular dynamics on parallel computers using the link-cell algorithm. <span class='cmti-12'>Molecular</span> <span class='cmti-12'>Simulation</span>, 6(1-3):51–87, 1991. URL <a class='url' href='https://doi.org/10.1080/08927029108022139'><span class='cmtt-12'>https://doi.org/10.1080/08927029108022139</span></a>.</p>
<p class='bibitem'><span class='biblabel'><a id='Xplimpton_fast_1995'></a><span class='bibsp'>   </span></span>S. Plimpton. Fast parallel algorithms for short-range molecular dynamics. <span class='cmti-12'>J. Comput. Phys.</span>, 117(1):1–19, 1995. URL <a class='url' href='https://doi.org/10.1006/jcph.1995.1039'><span class='cmtt-12'>https://doi.org/10.1006/jcph.1995.1039</span></a>.</p>
</div>
