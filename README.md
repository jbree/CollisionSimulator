# CollisionSimulator

## Checkout and build

Build requires that you checkout git submodules:

    git clone --recurse-submodules [REPO_URL]

Then build using CMake

    mkdir build
    cd build
    cmake path/to/project/source

## Explanation

The basic idea is to model each of the significant pieces of the network in an
object-oriented fashion:

+ A station which sends/receives
+ A medium across which stations send/receive
+ A Packet/frame which will be sent and received.

In order to effectively simulate, the station (and the medium?) must operate on
a simulation clock. Some operations must happen at the start of a simulation
frame, and some must happen at the end of a simulation frame so that the frame
is prepared for the next one. To accomplish this, we use `tick()` and `tock()`
to synchronize the operations of each of the `Simulatable` types.

Stepping out one level, a simulation controller will be necessary to initialize
each of the pieces of the simulation and to provide the simulation impetus, in
the form of Poisson-generated packets, arriving at a `Station` for transmission.


## Notes

A `Station` senses a `Medium` before transmission.

A `Medium` may accept multiple packets simultaneously. It is up to a `Station`
to detect collisions.


 ## Questions

+ Do we consider propagation delay to be negligible?

+ Do stations retransmit if they don't see collision-free ACK?

+ In problem 2, when B sends an ACK, do both A and C receive?

+ How quickly are collisions detected, can they be detected by sender before arrival, or during arrival of a larger packet

+ clarify how poisson distribution generation works


## Collaboration

Tim
+ Figure out poisson
+ simulation controller
+ reading in scenario data

Joshua
+ station state machine
+ medium
+ dumping out simulation data