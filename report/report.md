---
title: Simulation and Analysis of the Distributed Coordination Function (DCF) of 802.11
subtitle: ECE 578, Project 1

author:
- Joshua Breeden
- Timothy Ni

date: 13 Oct 2019
   
documentclass: article
header-includes:
    \usepackage{flushend}
toc: true
toc-depth: 1
numbersections: true
---

\pagebreak

# Introduction

## Description

## Responsibilities

# Simulation Methodology

## Source code
The source code for our simulation is available on GitHub:

+ [https://github.com/jbree/CollisionSimulator](https://github.com/jbree/CollisionSimulator)


## Simulation Components

To simulate the DCF, we created an object-oriented model using C++. We decided
to separately model the sending and receiving stations, and also the
medium/channel which packets are sent through. We then used a simulation
controller to orchestrate the simulation.

The simulation is advanced using a tick-tock sequence. Tick and tock, in our
simulation, loosely represent slot beginning and end, respectively.

Our simulation's data rates, collision domains, and sender-receiver graph are
configurable via input file. The input file describes which stations belong to
which medium, and which sender stations transmit to which receivers, and at what
rate.

### DataGenerator

The _DataGenerator_ class in our simulation generates a list of arrival times
using a Poisson distribution, and quantizes those arrivals into slots.

### SimulationController

The _SimulationController_ is configures each of the simulated components based
on the input file. After configuring each of the models, the
_SimulationController_ advances the simulation timeline by invoking the `tick()`
and `tock()` methods on the individual modeled components. Using the lists of
generated arrival times, the _SimulationController_ invokes the `arrive()`
method on a _SenderStation_ to indicate that a packet is ready.

At the end of the simulation, the _SimulationController_ is responsible for
collecting and printing information about successful transmissions and
collisions, which is gathered from the modeled components.

### SenderStation

The _SimulationController_ gives packets to the _SenderStation_ via the
`arrive()` method, which causes a packet to be enqueued for transmission. Once a
packet has arrived, the _SenderStation_ uses a state machine to coordinate its
sense, backoff, transmission, and wait-for-ACK periods.

### ReceiverStation

Each _ReceiverStation_ waits for packets to be received from every _Medium_ it
listens and transmits on, and responds accordingly. If multiple messages are
received during a single slot, whether from a single or multiple media, the
_ReceiverStation_ will observe a collision ignore the inbound packets.

### Medium

It is useful to model the transmission medium to have an easy way for stations
to determine if the medium is busy. All packets must arrive between invocations
of `tick()` and `tock()`. A sender may then check with a _Medium_ after a tock
to see if it was busy in the most recently ended slot.

# Results
# Justification

