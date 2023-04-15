---
layout: default
title: "Chapter 06"
parent: Lecture
date: 2023-04-15
categories: lecture
author: Lars Pastewka
nav_order: 06
---

<h2 class='chapterHead'><span class='titlemark'>Chapter 6</span><br />
<a id='x1-10006'></a>Parallel computers and the Message Passing Interface</h2>
<div class='framedenv' id='shaded*-1'><!--    l. 3    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Context:</span></span> This chapter sets the stage for discussing parallelization of the molecular dynamics simulation method introduced in the previous chapters. We first need to talk about parallel hardware architectures and how to program for them. The specific programming model that we will employ is known under the term <span class='cmti-12'>Single Program Multiple Data</span>. The Message Passing Interface (MPI) is a library that
facilitates programming for massively parallel machines under this programming model.</p>
</div>
<h3 class='sectionHead'><span class='titlemark'>6.1</span> <a id='x1-20006.1'></a>Parallel hardware architectures</h3>
<!--    l. 9    -->
<p class='noindent'>Parallel hardware has become ubiquitous over the past decade. Most central processing units (CPUs) in computers, phones or other hardware have multiple cores that can execute instructions in parallel. Massively parallel computing systems combine multiple CPUs into nodes that share a common memory. These nodes are then combined into the full compute system through a network interconnect.</p>
<!--    l. 11    -->
<p class='indent'>Parallel architecture are often hierachical and have parallelization at different levels. Notable is vectorization at the core-level, share memory parallelization for multicore architectures and distributed memory parallelization for large computing systems that communicate via an interconnect (a network connection).</p>
<!--    l. 13    -->
<p class='indent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=6439ba0b-8da2-4cb6-94da-ab9d009f062e'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=6439ba0b-8da2-4cb6-94da-ab9d009f062e</span></a></p>
<!--    l. 15    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>6.2</span> <a id='x1-30006.2'></a>Scaling consideration</h3>
<!--    l. 17    -->
<p class='noindent'>Software that runs on parallel computers needs to scale. Scaling describes how the time to returning the result changes as the number of available compute units (cores) changes. The simplest model for scaling assumes that our code can be divided into a fraction \(f_s\) that needs to be executed on a single core while a fraction \(f_p\) scales perfectly, i.e. its execute time is \(\propto 1/p\) where \(p\) is the number of available processes or cores. (Note that \(f_s+f_p=1\) since they
are fractions.) This leads to Amdahl’s law that describes the speedup \(S\) as a function of \(p\): \begin {equation} S = p f_p + f_s p \end {equation}<a id='x1-3001r1'></a></p>
<!--    l. 22    -->
<p class='indent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=95d5d79d-4259-4865-af14-ab9d0099e7cb'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=95d5d79d-4259-4865-af14-ab9d0099e7cb</span></a></p>
<!--    l. 24    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>6.3</span> <a id='x1-40006.3'></a>Programming model</h3>
<!--    l. 26    -->
<p class='noindent'>The Message Passing Interface (MPI) is an application programming interface (API) for distributed memory parallelization. (A code parallelized with MPI also works on shared memory machines!) The programming model underlying MPI is called single program multiple data (SPMD): The identical program is executed multiple times but operates on different datums.</p>
<!--    l. 28    -->
<p class='indent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=88d86075-f904-4e83-bae6-ab9d00a3e197'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=88d86075-f904-4e83-bae6-ab9d00a3e197</span></a></p>
<!--    l. 30    -->
<p class='noindent'></p>
<h4 class='subsectionHead'><span class='titlemark'>6.3.1</span> <a id='x1-50006.3.1'></a>Example: Monte-Carlo estimate of the number \(\pi \)</h4>
<!--    l. 32    -->
<p class='noindent'>As the simplest example of a parallelization, we consider a Monte-Carlo estimate of the number \(\pi \).</p>
<!--    l. 34    -->
<p class='indent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=18ce5d9a-50cf-4834-8a22-aba100bf31cb'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=18ce5d9a-50cf-4834-8a22-aba100bf31cb</span></a></p>
<h2 class='likechapterHead'><a id='x1-6000'></a>Bibliography</h2>
