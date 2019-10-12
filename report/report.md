---
title: Simulation and Analysis of the Distributed Coordination Function (DCF) of 802.11
subtitle: ECE 578, Project 1

author:
- Joshua Breeden
- Timothy Ni

date: 13 Oct 2019

# documentclass: IEEEtran
# header-includes:
#     \usepackage{flushend}
# toc: true
# toc-depth: 1
numbersections: true
geometry: margin=2cm
---

# Introduction

## Description

The goal of this project was to create a simulation of the Distributed
Coordination Function (DCF) of 802.11 for the purpose of analyzing performance
characteristics. Specifically, we were looking for data explaining the
throughput, fairness, and collision-propensity of different network topologies
both with and without virtual carrier sensing.


## Responsibilities

For this project, we shared responsibility for coding and documentation. We met
once to kick-off the project and discussed our approach to solving the problem.
After that, we used git to collaborate on code and kept in contact via text
message.

Timothy took responsibility for Poisson traffic generation, instantiating the
simulation controller and sending arriving packets to the model. He also used
Matlab to generate graphs of our simulated data.

Joshua took responsibility for coding the models of the simulation by
implementing state machines to follow the specified protocols.

# Simulation Methodology

## Source code
The source code for our simulation is available on GitHub:

+ [https://github.com/jbree/CollisionSimulator](https://github.com/jbree/CollisionSimulator)


## Simulation Components

To simulate the DCF, we created an object-oriented model using C++. We decided
to separately model the sending and receiving stations, and also the
medium/channel which packets are sent through. We then use a simulation
controller to orchestrate the simulation.

The simulation is advanced using a tick-tock sequence. Tick and tock, in our
simulation, loosely represent slot beginning and end, respectively.

Our simulation's data rates, collision domains, and sender-receiver graph are
configurable via input file. The input file describes which stations belong to
which medium, which sender stations transmit to which receivers, and at what
rate.

The following subsections describe in further detail each of the components of
the simulation.

### DataGenerator

The _DataGenerator_ class in our simulation generates a list of arrival times
using a Poisson arrival process, then quantizes those arrivals into slots. The
arrival time generator accepts a specified rate ($\lambda$) to use in the
calculations. We used the `random()` function from the Standard C Library to
generate uniformly distributed numbers after providing the current time as a
random seed value


### SimulationController

The _SimulationController_ configures each of the simulated components based on
an input file. After configuring each of the models, the _SimulationController_
advances the simulation timeline by invoking the `tick()` and `tock()` methods
on the individual modeled components. Using the lists of generated arrival
times, the _SimulationController_ invokes the `arrive()` method on a
_SenderStation_ to indicate that a packet is ready at the appropriate slot.

At the end of the simulation, the _SimulationController_ is responsible for
collecting and printing information about successful transmissions and
collisions, which is gathered from counters within the modeled components.

### SenderStation

The _SimulationController_ gives packets to the _SenderStation_ via the
`arrive()` method, which causes a packet to be enqueued for transmission. Each
_SenderStation_ sits idle until a packet has arrived for transmission. Once a
packet has arrived, the _SenderStation_ uses a state machine to advance through
its sense, backoff, transmission, and wait-for-ACK phases.

When the _SenderStation_ has completed its sense and backoff periods, it sends
the packet to a _Medium_ via the _Medium_'s `transmit()` method, then begins
waiting for ACK.

### Medium

We found it useful to model the transmission medium because it gave us an easy
way for stations to determine if the medium is busy. All packets must arrive
between invocations of `tick()` and `tock()`. A sender may then check with a
_Medium_ after a tock to see if it was busy in the most recently ended slot.

### ReceiverStation

Similar to the _SenderStation_, the _ReceiverStation_ class also uses a state
machine to advance through each of its phases. In our design, the station is
either "Idle" while it waits for or receives a packet, or it is "Acking".

If multiple messages are received during a single slot, whether via a single or
multiple media, the _ReceiverStation_ will count a collision and ignore the
inbound packets.

# Graphs of Results

## Throughput per Node

![](img/1a.png){width=50%}\ ![](img/1b.png){width=50%}

![](img/1c.png){width=50%}\ ![](img/1d.png){width=50%}

## Overall Throughput

In addition to the requested graphs, we felt it would be useful to provide
graphs of the combined throughput from both nodes.

![](img/combined-eq.png){width=50%}\ ![](img/combined-ne.png){width=50%}

## Collisions

![2a](img/2a.png){width=50%}\ ![2b](img/2b.png){width=50%}

![2c](img/2c.png){width=50%}\ ![2d](img/2d.png){width=50%}

## Fairness

![](img/3a.png){width=50%}\ ![](img/3b.png){width=50%}

# Justification

## Throughput in _Topology A_

We expect a maximum throughput of 12Mbps. The total number of theoretical
packets (discounting sensing, backoff, and ACK time) is given by the following
equation:

$$ \frac{12\text{MB}}{\text{sec}} \times \frac{1 \text{ frame}}{1500\text{B}}
\times \frac{1\text{B}}{8\text{b}} = 1000 \text{ frames per second (fps)} $$

Knowing this, we can conclude that two stations with combined packet arrivals
greater than or equal to 1000fps will necessarily become backlogged.

Thus, for the simulations operating with equal lambda values up to 500fps such
as the ones shown in graphs 1a and 1c, the trend is that throughput continues to
increase as lambda is increased. In the cases where node $A$'s lambda value is
twice of node $B$’s, the average arrival rate between nodes can exceed 500fps.
As a result, there is an observable impact on the throughput. Viewing the graph
displaying the combined throughput of simulations where node $A$'s lambda value
is twice node $B$'s, the graph plateaus after 300fps. This demonstrates that for
the values observed within the simulation, communication becomes fully congested
somewhere after an arrival rate of 600fps for node $A$ and 300fps for node $B$.

This observed throughput ceiling matches our expectations. The observed
throughput _must_ be less than the theoretical limit, since the observed limit
encounters collisions and acknowledgement overhead, whereas the described
theoretical limit does not.

## Throughput in _Topology B_

_Topology B  _ is where we see a big difference in throughput depending on whether
VCS is enabled. With VCS disabled, the throughput ceiling appears to be
~7.3Mbps. With VCS enabled, throughput appears to be ~10Mbps. One of the
surprising things about enabling VCS was how small the performance penalty is to
handle the hidden terminal case.

In the $\lambda_A = \lambda_C = 500\text{fps}$ case, we observed throughput of
~7.3Mbps. Without VCS, we expected throughput in this topology to be much lower.
We were surprised at how high the actual throughput was, and we believe that
exponential backoff is what makes this work as well as it does. Unfortunately,
despite the fact that the efficiency of the channel is ~60%, the fairness index
is very far from ideal, as we will discuss next.


## Hidden Terminals and Fairness

It appears that when virtual carrier sensing is not enabled, channel usage is
very unfair. When stations $A$ and $C$ first try to transmit, they both
experience collisions until their $CW_0$ becomes large. Eventually, one of the
stations is successful. When station $A$, is successful, its $CW_0$ is reset,
and it is likely to have additional successes while $C$ continues to wait with
its large backoff time. Even when additional collisions occur between the two,
$A$ is more likely to find eventual success, since its lower $CW_0$ value will
give it a better chance of transmitting sooner. So, we observe that in a
topology with hidden terminals, networks without VCS enabled will be very
unfair.

Comparing graphs 1c and 1d, the green line representing throughput for the
hidden terminal, sans-VCS case when $\lambda_A = 2\lambda_C$ shows that, once
the channel becomes saturated, node $A$ and node $C$ no longer transmit in a
ratio matching their relative $\lambda$. In our simulation runs, node $C$ had a
throughput rate of ~3400kbps with an arrival rate of 300fps. Node $A$, where
arrival rate $\lambda_A = 600\text{fps}$, has throughput of ~3800kbps. The fact
that the node $A$ is not operating at twice the throughput rate of $C$, where
$\lambda_C = 300\text{fps}$ indicates that, at these frame rates, the channel is
saturated.

We observe that with VCS disabled, when $\lambda_A = \lambda_C$ and the channel
is below saturation the fairness index remained close to 1. We expect that, as
arrival rate exceeds the saturation rate, fairness index should become
unpredictable and unfair due to the previously explained phenomenon.

Once we simulated the $\lambda_A = 2\lambda_C$ scenario with VCS enabled, we saw
that the fairness index was ~2 when the channel was below saturation. As the
arrival frame rate surpassed the channel saturation point, the fairness index
dropped to ~1. Since, below saturation, the arrival framerate from $A$ is twice
that of $C$, we expect that the throughput rate would also be double, and the
fairness index follows. However, as the channel is saturated, $A$’s throughput
becomes bounded by the channel, and both $A$ and $C$ are restricted to
approximately the same throughput rate.

## Collisions

Before producede simulation results, we already had a strong suspicion VCS would
help reduce collisions for topologies involving hidden terminals with its use of
RTS and CTS packets. Taking a look at the collision graphs of the simulations,
indeed an improvement can be seen in graphs 2a-d for hidden terminals when VCS
is enabled. Even in _Topology A_, it seems like VCS is not detrimental to
performance.

When VCS is disabled in _Topology B_, the collisions rate is very high. Even at
lower arrival rates, the collision rate peaks by the name $\lambda = 200$. We
think that it reaches the peak at this point because, at this arrival rate with
this topology, the stations are already being backlogged due to collisions, so
they are saturating the channel with retransmissions.

When VCS is enabled in _Topology B_, the number of collisions higher when the
channel has not yet reached saturation. At lower $\lambda$s, the collision rate
is lower due to the small amount of random traffic which is less likely to
overlap. Once the arrival rate is high enough to saturate the channel,
collisions are very unlikely because the CTS packets combined with exponential
backoff prevent collisions. However, when the channel frequently goes idle for
short periods, the collision rate increases. The window for collisions to occur
when both channels are idle is four slots wide: 2 while the hidden terminal
($C$, for example) is sending its RTS, one while the receiver is waiting to
respond, and then one more slot when the receiver begings its $CTS$. If $A$
starts transmitting during any of these slots, a collision occurs. We believe
this is the reason for the increased collision rate prior to saturation.

The collision rate in _Topology A_ appears to increase with the arrival rate up
to the point where arrivals saturate the channel. Since given the same arrival
rate, the frequency of collisions is determined by the number of transmitters
and the size of their collision window, we find this result sensible.

# Summary

In summary, CSMA/CA with VCS enabled seems to provide the best all-around
performance in the various simulations we tried. It gives a lower number of
collisions, higher throughput, and more fairness in every scenario with a hidden
terminal, while providing performance similar to the non-VCS mode in _Topology
A_.