# Trace through Veins

BaseConnectionManager - interference\
\|\
BasePhyLayer - analogue, decider, antenna\
\|  \\ \
\|&nbsp; PhyLayer80211p\
\|&nbsp; \|\
\|&nbsp; Mac1609_4 - channelIdle\
\|\
BaseMobility\
||\
TraCIScenarioManager

# Omnet++

# Sumo

# TraCI

TraCI does maintain a flow by the flow's (maybe, head) position and some other\
stuff (for example, orientation). Just a guess, TraCI implements this using a\
matrix with (time) scale as an index, so that it can display a bunch of objects

# Veins
## TraCIScenarioManager.cc

- initialize()\
connectAndStartTrigger = new cMessage("connect");\
executeOneTimestepTrigger = new cMessage("step");\
loop at

- overall\
Doesn't handle messages from other modules\
Manages the information given by TraCI

## How does the first message appear?
- DemoBaseApplLayer.cc (0)
- TraCIDemo11p.cc      (1)
- TraCIDemoRSU11p.cc   (2)
- BaseLayer.cc         (3)
- BaseMobility.cc      (4)

> 0:sendDown <- 1:handlePositionUpdate <- 0:receiveSignal <- 4:updatePos.  *car\
> 0:sendDelayedDown <- 2:onWSM <- 0:handleLowerMsg <- 3:handleMessage      *rsu

## Maybe, changeRoute is an important function!
Yeah, TraCICommandInterface.cc - most important reference!

> changeVehicleRoute must contain the current roadId - 0th element

# Testing
We can simply use opp_test, a tool in OMNET, to test any simple element that\
does not require to execute simultaneously with other modules. But, It's not\
sufficient for modules or other large components\
So, just a guess, the gdb's commandfile could handle this situation

