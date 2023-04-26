---
layout: default
title:  "eV/Å units"
parent: Notes
categories: notes
author : Lars Pastewka
nav_order: 7
---

# eV/Å units

## Units

For the EAM potential you need to switch from working with Lennard-Jones units to working with real physical units. We outline here briefly how unit conversion works. Our system of units requires units for length $$[l]$$, time $$[t]$$, and mass $$[m]$$. All other units are compound units, for example velocities $$[v]=[l]/[t]$$, accelerations $$[a]=[l]/[t]^2$$, forces $$[f]=[m][l]/[t]^2$$ and energies $$[E]=[m][l]^2/[t]^2$$. Note that the unit of force can also be expressed through energy and length, $$[f]=[E]/[l]$$ and it is common to use this denomination in molecular calculations.

## eV/Å

### Energy, lengths and forces

In molecular calculations we typically fix the units of energy $$[E]$$ and length $$[l]$$. This is because we know what typical energy and length scales look like. Molecular calculations use different sets of unit system. We will here use $$[E]=\text{eV}$$ and $$[l]=\text{Å}$$, because $$1~\text{eV}$$ is roughly the energy stored in a metallic or covalent bond and $$1~\text{Å}$$ is roughly the distance between atoms. Note that $$k_B T$$ at room temperature $$T=300~\text{K}$$ is $$k_B T\approx 25~\text{meV}$$.

Other energy units that are used in molecular calculations are $$\text{kJ/mol}$$ or $$\text{kcal/mol}$$. Please forget immediately that something like $$\text{kcal/mol}$$ even exists. To convert from $$\text{eV}$$ to SI units, we simply insert the respective units. Remember $$e=1.6\cdot 10^{-19}~\text{C}$$, then $$1~\text{eV}\approx 1.6\cdot 10^{-19} C J/C=1.6\cdot 10^{-19} J$$.

Forces in molecular calculations are typically reported in units of $$\text{eV}/\text{Å}$$, which is the natural force unit for this system of units. Note that in SI units this becomes $$1~\text{eV}/\text{Å}\approx 1.6\cdot 10^{-19}/10^{10}~\text{N}=1.6~\text{nN}$$.

### Fixing the mass unit

We have now fixed energy and length units. This means we can decide on a third unit, which then fixes our system of units. Let this third unit be the mass, which we set to $$[m]=\text{g/mol}$$. (This units is useful because atomic masses are tabulated in this unit in the periodic table.) We can then rewrite $$[E]=[m][l]^2/[t]^2$$ to $$[t]=[l]\sqrt{[m]/[E]}$$. Inserting our favorite system of units yields $$[t]=Å \sqrt{\text{g}/(\text{eV}~\text{mol})}$$. Remember that $$\text{mol}=6\cdot 10^{23}$$ (it is just a number!), hence $$[t]=10^{-10}~\text{m} \sqrt{10^{-3}~\text{kg}/(1.6\cdot 10^{-19} J \times 6\cdot 10^{23})}=1.018\cdot 10^{-14}~\text{s}=10.18~\text{fs}$$. This means that if you use $$\text{g/mol}$$ for your unit of mass, a unit time step in your calculation has the actual length of $$10.18~\text{fs}$$.

### Fixing the time unit

Since the time step is something we actively adjust as a user, it is often convenient to fix the time step rather than the mass. A good choice is $$[t]=1~\text{fs}$$ since this is around the time step that you need to discretize the equations of motion. Again from the units of the energy we obtain the mass units as $$[m]=[E][t]^2/[l]^2$$. Inserting our system of units yields $$[m]=1.6\cdot 10^{-19}~\text{J} \times 10^{-30}~\text{s}^2 / (10^{-20}~\text{m})=1.6\cdot 10^{-29}~\text{kg}$$. This is a rather unwieldy number that we want to express in terms of $$\text{g}/\text{mol}$$. This gives $$[m]=1.6\cdot 10^{-29}~\text{kg} \times \text{mol}/\text{mol} = 1.6\cdot 10^{-29} \times 10^{3}\times 6\cdot 10^{23} ~\text{g}/\text{mol}=0.009649~\text{g}/\text{mol}$$. If we have masses in $$\text{g}/\text{mol}$$, we need to divide those by $$0.009649$$ (multiply them by a factor of $$103.6$$) to get a time step of $$1~\text{fs}$$.
