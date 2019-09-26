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

`Station`s expect input from the simulation controller in a way resembling the
following order:

    for(int i(0); i < simulationTicks; i++) {
        station.tick();
        station.arrive(packet);
        station.tock();
    }

Additionally, each tick/tock only represents a single 10µs (1-slot) increment.


 ## Questions

+ Do we consider propagation delay to be negligible?

+ If a station becomes ready to transmit and the channel is idle for initial
DIFS, is the backoff counter used?

+ If channel is busy at any point during DIFS sense, does station immediately
start backoff, or wait out the full DIFS and then backoff?

+ Is the only way for the sender to detect collision to not receive an ACK?

+ How quickly are collisions detected, can they be detected by sender before
arrival, or during arrival of a larger packet

+ clarify how poisson distribution generation works


## Observations

Because of DIFS sensing time, I don't think it's possible to collide with an
ACK.


## Collaboration

Tim
+ Figure out poisson
+ simulation controller
+ reading in scenario data

Joshua
+ station state machine
+ medium
+ dumping out simulation data