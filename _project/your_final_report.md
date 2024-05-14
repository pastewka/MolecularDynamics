---
layout: default
title:  "Your final report"
parent: Project
categories: project
author : Lars Pastewka
nav_order: 1
---

# Your final report

## Documents you need to submit

In order to pass the class, you will need to submit at **the end of the examination period of the summer term 2024 (on September 13, 2024)**:

* A report, written in English or German, that describes your simulation code and results obtained with it.

* The simulation code itself. The code needs to have documentation strings that explain what each function and ideally each line of the code does. Please submit only the final parallel code. If the final code does not run in parallel, also submit a serial version of the code.

* Instructions on how to run the code for different lattice dimensions, different ω and different domain decomposition grids.

Please make sure to submit at the latest on **September 13, 2024, midnight**. All later submissions will count as fails.

## The report

The report should describe

* the simulation method,
* your implementation of the simulation method,
* results obtained with your code,
* scaling tests.

Use figures and illustrations where useful. Note that all results in the reports need to be reproducible. This means that figure captions need to clearly state the parameters for which the results shown in the figure were obtained, e.g. the lattice dimension and the parameter ω. The exercise sheets will contain instructions on a minimal set of figures to produce and show in the report. Please show us more than these. You need to convince us that your code is working and computing the right numbers, when run in serial and when run in parallel.
Note that if results are not obtained by you, e.g. if you reproduce a derivation of a formula from a book or some other source, then that source needs to be cited.
A potential outline of your report could have the following sections:

* Introduction
* Methods
* Implementation
* Results
* Conclusions

Do not structure the report along the milestones. The milestones are intended to guide you along the process of developing and testing the code. Your final report should make no reference to milestone numbers.

## How we will judge your report

We will look at the following **topical** aspects when evaluating your report:

* *Report:* Does the report contain a complete and comprehensive description of the simulation method and your implementation? Is the report complete in the sense that all exercises were completed? Are results described in the report reproducible in principle, i.e. is all data necessary to reproduce the results given? Are the results correct?
* *Figures:* Are at least all the figures that we asked for present? Are the results shown in the figures correct?
* *Software:* Are we able to run your code to reproduce a subset of the figures shown in the report?

We will also look at the following **formal** aspects when evaluating your report:

* *Report:* Is the report structured clearly? Does the report use clear language? Are results from the literature cited?
* *Figures:* Are all figures described in the text? Are they cited in the order in which they appear? Do all figures have captions? Are they complete (legend, axes labels, etc.)? Are they legible? Do figure captions describe what is seen? Is all data necessary to reproduce the figures present in the captions or text?
* *Software:* Does the code work? Is the code structured logically? Are components/functions reusable? Is the code written with generality in mind (i.e. does it work for arbitrary lattice dimensions)? Is the code documented?

## Additional resources

There are excellent resources online that describe how to write a scientific paper, essentially a report to be published in a journal. Two of those are listed below:
* Mike Ashby’s group: <http://www-mech.eng.cam.ac.uk/mmd/ashby-paper-V6.pdf>
* George Whiteside’s group: <https://gmwgroup.harvard.edu/publications/whitesides-group-writing-paper>
* Nature Physics: <http://www.nature.com/nphys/journal/v3/n9/full/nphys724.html>

