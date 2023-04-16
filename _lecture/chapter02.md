---
layout: default
title: "Chapter 02"
parent: Lecture
date: 2023-04-16
categories: lecture
author: Lars Pastewka
nav_order: 02
---
---

<h2 class='chapterHead'><span class='titlemark'>Chapter 2</span><br />
<a id='x1-10002'></a>Molecular dynamics</h2>
<div class='framedenv' id='shaded*-1'><!--    l. 3    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Context:</span></span> Molecular <span class='cmti-12'>dynamics</span> follows the motion of individual atoms through a solution of Newton’s equations of motion. We need integration algorithms to be able to solve this set of coupled differential equations on a computer.</p>
</div>
<div class='framedenv' id='shaded*-1'><!--    l. 7    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Additional resources:</span></span></p>
<ul class='itemize1'>
<li class='itemize'>Chapter 3 of Allen &amp; Tildesley, Computer Simulation of Liquids</li>
<li class='itemize'>Appendix <span class='cmbx-12'>??</span> on dynamical systems.</li>
</ul>
</div>
<h3 class='sectionHead'><span class='titlemark'>2.1</span> <a id='x1-20002.1'></a>Equations of motion</h3>
<!--    l. 18    -->
<p class='noindent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=d3188a05-4143-4602-b6c2-ad15014fe21d'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=d3188a05-4143-4602-b6c2-ad15014fe21d</span></a></p>
<!--    l. 20    -->
<p class='noindent'></p>
<h4 class='subsectionHead'><span class='titlemark'>2.1.1</span> <a id='x1-30002.1.1'></a>Newton’s equations of motion</h4>
<!--    l. 25    -->
<p class='noindent'>We have already (almost) all the ingredients to carry out a molecular dynamics simulation. From our or potential energy expression \(E_{\text {pot}}(\{\vec {r}_i\})\) discussed in the previous chapter, we obtain the force \begin {equation} \vec {f}_i = -\partial E_{\text {pot}}/\partial \vec {r}_i \end {equation}<a id='x1-3001r1'></a> on each of the \(N\) atoms. Once we know the forces, we can obtain the accelerations \(\vec {a}_i\) through Newton’s third law, \begin {equation} \vec
{f}_i = m_i \vec {a}_i. \end {equation}<a id='x1-3002r2'></a> We are therefore assuming that atom \(i\) can be described as a point of mass \(m_i\)! The mass can be obtained from the periodic table of elements. Note that the mass listed in the periodic table is usually the average over all isotopes weighted by their occurrence on earth, and this mass is used for most practical purposes. For some application, in particular to understand the different behavior of Hydrogen and Deuterium, it can be necessary
to actually model the individual isotopes by using their respective mass.</p>
<!--    l. 35    -->
<p class='indent'>We further have \(\vec {a}_i = \dot {\vec {v}}_i\), where \(\vec {v}_i\) is the velocity of atom \(i\), and \(\vec {v}_i = \dot {\vec {r}}_i\). The dot superscript indicates derivative with respect to time. The set of linear differential equations to solve is therefore \begin {equation} \dot {v}_i(t) = \vec {f}_i(t)/m_i\ {\text { and} }\ \dot {\vec {r}}_i(t) = \vec {v}_i(t) \label {eq:Newton} \end {equation}<a id='x1-3003r3'></a> with the initial (boundary) conditions \(\vec {r}_i(0) =
\vec {r}_0\) and \(\vec {v}_i(0) = \vec {v}_0\). Note that the boundary condition is an integral part of the differential Eq. \eqref{eq:Newton}. The state of the system is therefore fully and uniquely determined by the positions \(\vec {r}_i\) and the velocities \(\vec {v}_i\) of all atoms. This set of positions \(\vec {r}_i\) and momenta \(\vec {p}_i = \vec {v}_i/m_i\) defines a point in <span class='cmti-12'>phase-space</span> \(\vec {\Gamma } = \{ \vec {r}_i, \vec {p}_i\}\). The evolution of
position and velocities given by Eq. \eqref{eq:Newton} can therefore be thought of as a single point moving in the \(6N\) dimensional phase-space.</p>
<div class='framedenv' id='shaded*-1'><!--    l. 42    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Code example:</span></span> For a molecular dynamics code, it is useful to have a data structure that represents the state of the simulation and stores at least positions and velocities. This data structure could also store element names (or atomic numbers), masses and forces. An example that uses <a href='https://eigen.tuxfamily.org/'>Eigen</a> arrays as the basic array container is shown below. As a general rule, the data structure should
be designed in a way that data that is processed consecutively is also stored in memory in a contiguous manner. This ensures predictable memory access patterns and efficient caching of the data necessary for computation. Instead of the <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>Atoms</span></span></span> container described below, we could be tempted to create a class <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>Atom</span></span></span> that contains the
positions, velocities, etc. of a single atom and then use an array (e.g. <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>std::vector&lt;Atom&gt;</span></span></span>) of that class as the basic data structure. However, positions are then no longer consecutive in memory: they are interlaced with other atomic data. A function (e.g. computing forces) that does not need the velocities would still load them into the cache, causing more data transfers between cache and RAM, lowering
performance. For high-performance numerical code, it is therefore <span class='cmti-12'>always</span> preferable to use <a href='https://en.wikipedia.org/wiki/AoS_and_SoA'>structures of arrays rather than arrays of structures</a>.</p>
<!--    l. 47    -->
<pre class='lstlisting' id='listing-1'><span class='label'><a id='x1-3004r1'></a><span class='cmr-6'>1</span></span><span style='color:#009900'><span class='cmtt-10'>//</span></span><span style='color:#009900'> <span class='cmtt-10'>Type </span><span class='cmtt-10'>aliases</span> 
</span><span class='label'><a id='x1-3005r2'></a><span class='cmr-6'>2</span></span><span style='color:#FF00FF'><span class='cmtt-10'>using</span></span><span style='color:#000000'> <span class='cmtt-10'>Positions_t </span><span class='cmtt-10'>= </span><span class='cmtt-10'>Eigen::Array3Xd;</span> 
</span><span class='label'><a id='x1-3006r3'></a><span class='cmr-6'>3</span></span><span style='color:#FF00FF'><span class='cmtt-10'>using</span></span><span style='color:#000000'> <span class='cmtt-10'>Velocities_t </span><span class='cmtt-10'>= </span><span class='cmtt-10'>Eigen::Array3Xd;</span> 
</span><span class='label'><a id='x1-3007r4'></a><span class='cmr-6'>4</span></span><span style='color:#FF00FF'><span class='cmtt-10'>using</span></span><span style='color:#000000'> <span class='cmtt-10'>Forces_t </span><span class='cmtt-10'>= </span><span class='cmtt-10'>Eigen::Array3Xd;</span> 
</span><span class='label'><a id='x1-3008r5'></a><span class='cmr-6'>5</span></span> 
<span class='label'><a id='x1-3009r6'></a><span class='cmr-6'>6</span></span><span style='color:#FF00FF'><span class='cmtt-10'>struct</span></span><span style='color:#000000'> <span class='cmtt-10'>Atoms </span><span class='cmtt-10'>{</span> 
</span><span class='label'><a id='x1-3010r7'></a><span class='cmr-6'>7</span></span><span style='color:#000000'>    <span class='cmtt-10'>Positions_t </span><span class='cmtt-10'>positions;</span> 
</span><span class='label'><a id='x1-3011r8'></a><span class='cmr-6'>8</span></span><span style='color:#000000'>    <span class='cmtt-10'>Velocities_t </span><span class='cmtt-10'>velocities;</span> 
</span><span class='label'><a id='x1-3012r9'></a><span class='cmr-6'>9</span></span><span style='color:#000000'>    <span class='cmtt-10'>Forces_t </span><span class='cmtt-10'>forces;</span> 
</span><span class='label'><a id='x1-3013r10'></a><span class='cmr-6'>10</span></span> 
<span class='label'><a id='x1-3014r11'></a><span class='cmr-6'>11</span></span><span style='color:#000000'>    <span class='cmtt-10'>Atoms(Positions_t </span><span class='cmtt-10'>&amp;p)</span> 
</span><span class='label'><a id='x1-3015r12'></a><span class='cmr-6'>12</span></span><span style='color:#000000'>        <span class='cmtt-10'>: </span><span class='cmtt-10'>positions{p},</span> 
</span><span class='label'><a id='x1-3016r13'></a><span class='cmr-6'>13</span></span><span style='color:#000000'>          <span class='cmtt-10'>velocities{3, </span><span class='cmtt-10'>p.cols()},</span> 
</span><span class='label'><a id='x1-3017r14'></a><span class='cmr-6'>14</span></span><span style='color:#000000'>          <span class='cmtt-10'>forces{3, </span><span class='cmtt-10'>p.cols()} </span><span class='cmtt-10'>{</span> 
</span><span class='label'><a id='x1-3018r15'></a><span class='cmr-6'>15</span></span><span style='color:#000000'>        <span class='cmtt-10'>velocities.setZero();</span> 
</span><span class='label'><a id='x1-3019r16'></a><span class='cmr-6'>16</span></span><span style='color:#000000'>        <span class='cmtt-10'>forces.setZero();</span> 
</span><span class='label'><a id='x1-3020r17'></a><span class='cmr-6'>17</span></span><span style='color:#000000'>    <span class='cmtt-10'>}</span> 
</span><span class='label'><a id='x1-3021r18'></a><span class='cmr-6'>18</span></span> 
<span class='label'><a id='x1-3022r19'></a><span class='cmr-6'>19</span></span><span style='color:#000000'>    <span class='cmtt-10'>size_t </span><span class='cmtt-10'>nb_atoms()</span> </span><span style='color:#FF00FF'><span class='cmtt-10'>const</span></span><span style='color:#000000'> <span class='cmtt-10'>{</span> 
</span><span class='label'><a id='x1-3023r20'></a><span class='cmr-6'>20</span></span><span style='color:#000000'>        </span><span style='color:#FF00FF'><span class='cmtt-10'>return</span></span><span style='color:#000000'> <span class='cmtt-10'>positions.cols();</span> 
</span><span class='label'><a id='x1-3024r21'></a><span class='cmr-6'>21</span></span><span style='color:#000000'>    <span class='cmtt-10'>}</span> 
</span><span class='label'><a id='x1-3025r22'></a><span class='cmr-6'>22</span></span><span style='color:#000000'><span class='cmtt-10'>};</span></span></pre></div>
<!--    l. 74    -->
<p class='noindent'></p>
<h4 class='subsectionHead'><span class='titlemark'>2.1.2</span> <a id='x1-40002.1.2'></a>Kinetic energy and energy conservation</h4>
<!--    l. 76    -->
<p class='noindent'>In addition to the potential energy \(E_{\text {pot}}(\{ \vec {r}_i\})\), the dynamical state of a system is characterized by its kinetic energy, \begin {equation} E_{\text {kin}}(\{ \vec {p}_i\}) = \sum _i \frac {1}{2} \frac {p_i^2}{m_i}. \end {equation}<a id='x1-4001r4'></a></p>
<div class='framedenv' id='shaded*-1'><!--    l. 81    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Note:</span></span> The <span class='cmti-12'>temperature</span> is simply a measure of the kinetic energy of the system, \(\frac {3}{2} N k_B T = E_{\text {kin}}\) where \(N\) is the number of atoms. In other words, \(E_{\text {kin}}\) measures the variance of the velocity distribution, which is Gaussian. We will learn more about this when discussing the basics of statistical mechanics.</p>
</div>
<!--    l. 85    -->
<p class='indent'>The total energy \begin {equation} H(\{ \vec {r}_i\},\{ \vec {p}_i\}) = E_{\text {kin}}(\{ \vec {p}_i\}) + E_{\text {pot}}(\{ \vec {r}_i\}) \label {eq:hamil} \end {equation}<a id='x1-4002r5'></a> is a conserved quantity during the motion of the atoms. This can be seen by showing that the derivative of the total energy with respect to time vanishes, \begin {equation} \dot {H} = \dot {E}_{\text {kin}} + \dot {E}_{\text {pot}} = \sum _i \frac {\vec {p}_i \dot {\vec {p}}_i}{m_i} + \sum _i
\frac {\partial E_{\text {pot}}}{\partial \vec {r}_i} \dot {\vec {r_i}} = \sum _i \vec {v}_i \vec {f}_i - \sum _i \vec {v}_i \vec {f}_i = 0. \end {equation}<a id='x1-4003r6'></a> \(H\) is also called the <span class='cmti-12'>Hamiltonian</span> of the system.</p>
<div class='framedenv' id='shaded*-1'><!--    l. 97    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Note:</span></span> Measuring the total energy (or any other conserved quantity!) and checking whether it is constant in a molecular dynamics simulation is a way of testing if the time step \(\Delta t\) used in the numerical integration is small enough. We will discuss numerical integration in detail below.</p>
</div>
<!--    l. 101    -->
<p class='indent'>A generalization of Newton’s equations of motion are <span class='cmti-12'>Hamilton’s equations of</span> <span class='cmti-12'>motion</span>, \begin {align} \dot {\vec {r}}_i &amp;= \frac {\partial H}{\partial \vec {p}_i} \\ \dot {\vec {p}}_i &amp;= -\frac {\partial H}{\partial \vec {r}_i}, \end {align}</p>
<!--    l. 106    -->
<p class='indent'>and it is straightforward to show that these equations reduce to Newton’s equations of motions for the Hamiltonian given by Eq. \eqref{eq:hamil}. Hamilton’s equation of motion remain valid when positions \(\vec {r}_i\) and momenta \(\vec {p}_i\) are replaced by generalized coordinates that consider constraints, such as for example the angle of a (rigid) pendulum. These equations will become important when we discuss statistical mechanics and temperature control in molecular dynamics
simulations using <span class='cmti-12'>thermostats</span>, where a generalized degree of freedom is the internal state of the heat bath that controls the temperature.</p>
<!--    l. 108    -->
<p class='noindent'></p>
<h3 class='sectionHead'><span class='titlemark'>2.2</span> <a id='x1-50002.2'></a>Integration algorithms</h3>
<!--    l. 111    -->
<p class='noindent'><a class='url' href='https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=50b66784-6560-4aea-8654-ad16004e1442'><span class='cmtt-12'>https://uni-freiburg.cloud.panopto.eu/Panopto/Pages/Embed.aspx?id=50b66784-6560-4aea-8654-ad16004e1442</span></a></p>
<!--    l. 113    -->
<p class='indent'>The main ingredient in any molecular dynamics simulation, regardless of the underlying model, is the numerical solution of Eqs. \eqref{eq:Newton}. A plethora of algorithms have been developed over the years, but for most practical purposes the Velocity-Verlet algorithm is used nowadays. For instructive purposes we will start out with a simple integration method, the Euler integration, before discussing Velocity-Verlet.</p>
<!--    l. 115    -->
<p class='noindent'></p>
<h4 class='subsectionHead'><span class='titlemark'>2.2.1</span> <a id='x1-60002.2.1'></a>Euler integration</h4>
<!--    l. 116    -->
<p class='noindent'>In order to follow the trajectories of all atoms, we need to integrate the above differential equation. On a computer, a continuous differential equation needs to be replaced by a discrete equation. Equations \eqref{eq:Newton} are continuous in time and hence need to be discretized. (Note that our system is already discrete spatially since we are dealing with mass points, but each of these points corresponds to a physical object, so this is not the result of a discretization
procedure.) The simplest numerical integration scheme is the forward Euler algorithm, in which forces and velocities are assumed to be constant over time intervals \(\Delta t\).</p>
<!--    l. 119    -->
<p class='indent'>To see this, we write the above differential equation as \begin {equation} d \vec {v}_i = \frac {\vec {f}_i(t)}{m_i}\,dt\ {\text { and} }\ d\vec {r}_i(t) = \vec {v}_i(t)\,dt \end {equation}<a id='x1-6001r7'></a> i.e., we move the differential \(d t\) of \(\dot {\vec {v}}_i = d\vec {v}/d t\) to the right hand side of the equation. We can now straightforwardly integrate the equation from time \(t\) to time \(t + \Delta t\) while assuming that \(\vec {f}_i\) and \(\vec {v}_i\) remain
constant. This yields \begin {align} \vec {v}_i(t+\Delta t) - \vec {v}_i(t) &amp;= \frac {\vec {f}_i(t)}{m_i} \Delta t \label {eq:eulerexplicita} \\ \vec {r}_i(t+\Delta t) - \vec {r}_i(t) &amp;= \vec {v}_i(t) \Delta t \label {eq:eulerexplicitb} \end {align}</p>
<!--    l. 128    -->
<p class='indent'>which is obviously only a good approximation for small \(\Delta t\)! This algorithm is called Euler integration.</p>
<!--    l. 130    -->
<p class='indent'>The same equation can be derived by Taylor-expanding \(\vec {r}_i(t+\Delta t)\) up to first order in \(\Delta t\). The integration error of this algorithm is hence \(O(\Delta t^2)\). The Euler algorithm is not reversible, i.e. starting from time \(t+\Delta t\) and integrating backwards one ends up with a different result at time \(t\). Applying the Euler algorithm with timestep \(-\Delta t\) gives \begin {align} \vec {v}_i(t) - \vec {v}_i(t+\Delta t) &amp;= -\frac {\vec
{f}_i(t+\Delta t)}{m_i} \Delta t \\ \vec {r}_i(t) - \vec {r}_i(t+\Delta t) &amp;= -\vec {v}_i(t+\Delta t) \Delta t \end {align}</p>
<!--    l. 138    -->
<p class='indent'>These equations cannot be re-arranged to give Eqs. \eqref{eq:eulerexplicita} and \eqref{eq:eulerexplicitb}. Forward Euler integration is generally not a good algorithm and requires very small time steps.</p>
<!--    l. 143    -->
<p class='noindent'></p>
<h4 class='subsectionHead'><span class='titlemark'>2.2.2</span> <a id='x1-70002.2.2'></a>Leap-frog integration</h4>
<!--    l. 144    -->
<p class='noindent'>Leap-frog assumes positions are defined at times \(t_i\) and velocities at times \(t_i+\Delta t/2\), and can be derived from an argument similar to the one given above. Specifically, we combine the results of a Taylor expansion \(\pm \Delta t/2\), yielding \begin {align} \vec {v}_i(t+\Delta t/2) - \vec {v}_i(t-\Delta t/2) &amp;= \frac {\vec {f}_i(t)}{m_i} \Delta t \label {eq:leapfrog1} \\ \vec {r}_i(t+\Delta t) - \vec {r}_i(t) &amp;= \vec {v}_i(t+\Delta t/2) \Delta t. \end {align}</p>
<!--    l. 151    -->
<p class='indent'>Note that Eq. \eqref{eq:leapfrog1} is similar to Eq. \eqref{eq:eulerexplicita}, except the force is evaluated at the mid-point. The resulting algorithm is reversible. Applying the Leap-frog algorithm with timestep \(-\Delta t\) gives \begin {align} \vec {v}_i(t-\Delta t/2) - \vec {v}_i(t+\Delta t/2) &amp;= -\frac {\vec {f}_i(t)}{m_i} \Delta t \\ \vec {r}_i(t) - \vec {r}_i(t+\Delta t) &amp;= -\vec {v}_i(t+\Delta t/2) \Delta t \end {align}</p>
<!--    l. 157    -->
<p class='indent'>Bring the terms on the left hand side to the right and vice-versa, and you arrive at the original equations for forward integration. Leap-frog is therefore <span class='cmti-12'>reversible</span>.</p>
<!--    l. 159    -->
<p class='noindent'></p>
<h4 class='subsectionHead'><span class='titlemark'>2.2.3</span> <a id='x1-80002.2.3'></a>Verlet integration</h4>
<!--    l. 161    -->
<p class='noindent'>Let us now Taylor expand \(\vec {r}_i(t\pm \Delta t)\) up to third order in \(\pm \Delta t\), \begin {equation} \label {eq:taylor_tplus} \vec {r}_i(t\pm \Delta t) = \vec {r}_i(t) \pm \vec {v}_i(t) \Delta t + \frac {1}{2m_i} \vec {f}_i(t) \Delta t^2 \pm \frac {1}{6} \dot {\dot {\dot {\vec {r}}}}_i(t) \Delta t^3 + O(\Delta t^4). \end {equation}<a id='x1-8001r8'></a> Note that only the odd exponents see the sign of \(\pm \Delta t\). The sum of this equation for expansion in \(+\Delta t\)
and \(-\Delta t\) gives the positions update, \begin {equation} \vec {r}_i(t+\Delta t) + \vec {r}_i(t-\Delta t) = 2\vec {r}_i(t) + \frac {1}{m_i} \vec {f}_i(t) \Delta t^2 + O(\Delta t^4). \label {eq:verlet} \end {equation}<a id='x1-8002r9'></a> Eq. \eqref{eq:verlet} is called the Verlet algorithm. Instead of requiring the positions \(\{ \vec {r}_i(t)\}\) and velocities \(\{ \vec {v}_i(t)\}\) it requires the positions of the current \(\{ \vec {r}_i(t)\}\) and past \(\{ \vec {r}_i(t-\Delta t)\}\) times
for the integration.</p>
<!--    l. 174    -->
<p class='indent'>The difference between the expansion for \(+\Delta t\) and \(-\Delta t\) yields the velocities, \begin {equation} \vec {r}_i(t+\Delta t) - \vec {r}_i(t-\Delta t) = 2\vec {v}_i(t) \Delta t + O(\Delta t^3). \end {equation}<a id='x1-8003r10'></a> Note that in order to compute the velocities at time t in the regular Verlet algorithm, we need to know the positions at time \(t + \Delta t\). Verlet and Leap-Frog are identical algorithms, since Leap-Frog stores the velocities at the intermediate
time \(t+\Delta t/2\). It is usually useful to be able to know both, positions and velocities, at time \(t\). This problem is solved by the Velocity-Verlet algorithm, described in the following section.</p>
<!--    l. 181    -->
<p class='noindent'></p>
<h4 class='subsectionHead'><span class='titlemark'>2.2.4</span> <a id='x1-90002.2.4'></a>Velocity-Verlet integration</h4>
<!--    l. 183    -->
<p class='noindent'>Let us now also Taylor expand \(\vec {r}_i(t)\) up to third order in \(\Delta t\) at \(\vec {r}_i(t+\Delta t)\), i.e. we integrate backwards in time from \(t + \Delta t\) to \(t\). This gives \begin {equation} \vec {r}_i(t) = \vec {r}_i(t+\Delta t) - \vec {v}_i(t+\Delta t) \Delta t + \frac {1}{2m_i} \vec {f}_i(t+\Delta t) \Delta t^2 - \frac {1}{6} \dot {\dot {\dot {\vec {r}}}}_i(t) \Delta t^3 + O(\Delta t^3) \label {eq:taylor_r} \end {equation}<a id='x1-9001r11'></a>
Equation \eqref{eq:taylor˙tplus} is the positions update of the Velocity-Verlet algorithm. The sum of Eq. \eqref{eq:taylor˙tplus} and Eq. \eqref{eq:taylor˙r} gives the velocity update in the Velocity-Verlet algorithm: \begin {align} \vec {r}_i(t+\Delta t) &amp;= \vec {r}_i(t) + \vec {v}_i(t)\Delta t + \frac {1}{2m_i} \vec {f}_i(t) \Delta t^2\\ \vec {v}_i(t+\Delta t) &amp;= \vec {v}_i(t) + \frac {1}{2m_i} \left (\vec {f}_i(t) + \vec {f}_i(t+\Delta t) \right ) \Delta t, \end {align}</p>
<!--    l. 193    -->
<p class='indent'>Note that this algorithm is often split in the form of a predictor-corrector scheme since this saves computation time and the necessity to keep past forces around. The predictor step is \begin {align} \vec {v}_i(t+\Delta t/2) &amp;= \vec {v}_i(t) + \frac {1}{2m_i} \vec {f}_i(t) \Delta t \label {eq:vvpred1} \\ \vec {r}_i(t+\Delta t) &amp;= \vec {r}_i(t) + \vec {v}_i(t+\Delta t/2) \Delta t \label {eq:vvpred2} \end {align}</p>
<!--    l. 201    -->
<p class='indent'>where \(\vec {v}_i(t+\Delta t/2)\) is the predicted velocity. After this we compute new forces, \(\vec {f}_i(t+\Delta t)\). We then correct the velocities via \begin {equation} \vec {v}_i(t+\Delta t) = \vec {v}_i(t+\Delta t/2) + \frac {1}{2m_i} \vec {f}_i(t+\Delta t) \Delta t \end {equation}<a id='x1-9002r12'></a> The Velocity-Verlet algorithm is the integration algorithm used in most molecular dynamics codes. It has the additional properties that is it <span class='cmti-12'>symplectic</span>, which means it conserves phase-space volume. We will come back to what this mean when talking about statistical mechanics.</p>
<div class='framedenv' id='shaded*-1'><!--    l. 208    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Code example:</span></span> We can implement the velocity-verlet algorithm in a few lines of C++ code using vectorized <span class='obeylines-h'><span class='verb'><span class='cmtt-12'>Eigen</span></span></span> operations. The prediction step</p>
<!--    l. 210    -->
<pre class='lstlisting' id='listing-2'><span class='label'><a id='x1-9003r1'></a><span class='cmr-6'>1</span></span><span style='color:#FF00FF'><span class='cmtt-10'>void</span></span><span style='color:#000000'> <span class='cmtt-10'>verlet_step1(Atoms </span><span class='cmtt-10'>&amp;atoms,</span> </span><span style='color:#FF00FF'><span class='cmtt-10'>double</span></span><span style='color:#000000'> <span class='cmtt-10'>timestep,</span> 
</span><span class='label'><a id='x1-9004r2'></a><span class='cmr-6'>2</span></span><span style='color:#000000'>                  </span><span style='color:#FF00FF'><span class='cmtt-10'>double</span></span><span style='color:#000000'> <span class='cmtt-10'>mass) </span><span class='cmtt-10'>{</span> 
</span><span class='label'><a id='x1-9005r3'></a><span class='cmr-6'>3</span></span><span style='color:#000000'>    <span class='cmtt-10'>atoms.velocities </span><span class='cmtt-10'>+= </span><span class='cmtt-10'>0.5 </span><span class='cmtt-10'>* </span><span class='cmtt-10'>atoms.forces </span><span class='cmtt-10'>* </span><span class='cmtt-10'>timestep </span><span class='cmtt-10'>/ </span><span class='cmtt-10'>mass;</span> 
</span><span class='label'><a id='x1-9006r4'></a><span class='cmr-6'>4</span></span><span style='color:#000000'>    <span class='cmtt-10'>atoms.positions </span><span class='cmtt-10'>+= </span><span class='cmtt-10'>atoms.velocities </span><span class='cmtt-10'>* </span><span class='cmtt-10'>timestep;</span> 
</span><span class='label'><a id='x1-9007r5'></a><span class='cmr-6'>5</span></span><span style='color:#000000'><span class='cmtt-10'>}</span></span></pre>
<!--    l. 217    -->
<p class='indent'>implements Eq. \eqref{eq:vvpred1}. We then compute new forces and correct the velocities via</p>
<!--    l. 218    -->
<pre class='lstlisting' id='listing-3'><span class='label'><a id='x1-9008r1'></a><span class='cmr-6'>1</span></span><span style='color:#FF00FF'><span class='cmtt-10'>void</span></span><span style='color:#000000'> <span class='cmtt-10'>verlet_step2(Atoms </span><span class='cmtt-10'>&amp;atoms,</span> </span><span style='color:#FF00FF'><span class='cmtt-10'>double</span></span><span style='color:#000000'> <span class='cmtt-10'>timestep,</span> 
</span><span class='label'><a id='x1-9009r2'></a><span class='cmr-6'>2</span></span><span style='color:#000000'>                  </span><span style='color:#FF00FF'><span class='cmtt-10'>double</span></span><span style='color:#000000'> <span class='cmtt-10'>mass) </span><span class='cmtt-10'>{</span> 
</span><span class='label'><a id='x1-9010r3'></a><span class='cmr-6'>3</span></span><span style='color:#000000'>    <span class='cmtt-10'>atoms.velocities </span><span class='cmtt-10'>+= </span><span class='cmtt-10'>0.5 </span><span class='cmtt-10'>* </span><span class='cmtt-10'>atoms.forces </span><span class='cmtt-10'>* </span><span class='cmtt-10'>timestep </span><span class='cmtt-10'>/ </span><span class='cmtt-10'>mass;</span> 
</span><span class='label'><a id='x1-9011r4'></a><span class='cmr-6'>4</span></span><span style='color:#000000'><span class='cmtt-10'>}</span></span></pre></div>
<div class='framedenv' id='shaded*-1'><!--    l. 229    -->
<p class='noindent'><span class='underline'><span class='cmbx-12'>Note:</span></span> The timestep in MD simulations has to be on the order of femtoseconds, in order to resolve the fastest atomic vibrations. For example, in simulations with metals and Embedded Atom Method (EAM) potentials, \(\Delta t=1\) fs is typically a safe choice. How can we check that the timestep is sensible? One possibility is to simply let a configuration in time using the Velocity-Verlet algorithm. This is sometimes called
the micro-canonical or NVE ensemble. (NVE because number of atoms, volume and energy is constant.) We then record the evolution of the total (kinetic plus potential) energy, which should be constant. Due to the approximations described above, discrete time integration schemes introduce numerical errors. If \(\Delta t\) is larger than a critical value, the integration error grows unstable and causes a noticeable drift of the total energy. The figures below show the results of such a simulation. A system of
\(108000\) Au atoms was simulated for \(100\) ps with various values of \(\Delta t\). The \(y\)-axis shows the difference between the current and initial values of the total energy. The data was smoothened to suppress high-frequency fluctuations in the figure. For this system, even \(5\) fs would still be an acceptable time step.</p>
<div class='center'><!--    l. 239    -->
<p class='noindent'></p>
<!--    l. 240    -->
<p class='noindent'><img alt='PIC' height='390' src='figures/etot_vs_time_as_function_of_timestep_Grochola_Au_NVE_ensemble_01.png' width='390' /> <img alt='PIC' height='390' src='figures/etot_vs_time_as_function_of_timestep_Grochola_Au_NVE_ensemble_02.png' width='390' /></p>
</div>
</div>
<h2 class='likechapterHead'><a id='x1-10000'></a>Bibliography</h2>
