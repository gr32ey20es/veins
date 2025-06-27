#include "route/RouteDemo.h"
#include "route/RouteDemoMessage_m.h"

using namespace veins;
using namespace route;

Define_Module(route::RouteDemo);

void RouteDemo::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;

        stamp_t = 0;
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void RouteDemo::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here

}

// Your application has received a beacon message from another car or RSU
// code for handling the message goes here
void RouteDemo::onBSM(DemoSafetyMessage* bsm)
{
    
}

// Your application has received a data message from another car or RSU
// code for handling the message goes here, see TraciDemo11p.cc for examples
void RouteDemo::onWSM(BaseFrame1609_4* wsm)
{
    
}

// Your application has received a service advertisement from another car or
// RSU code for handling the message goes here, see TraciDemo10p.cc for examples
void RouteDemo::onWSA(DemoServiceAdvertisment* wsa)
{

}

// This method is for self messages (mostly timers). It is important to 
// call the DemoBaseApplLayer function for BSM and WSM transmission
void RouteDemo::handleSelfMsg(cMessage* msg)
{
    DemoBaseApplLayer::handleSelfMsg(msg);

}

// The vehicle has moved. Code that reacts to new positions goes here.
// Member variables such as currentPosition and currentSpeed are updated 
// in the parent class
void RouteDemo::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    uint8_t stamp_tmp;
    RouteDemoMessage* rdm;

    stamp_tmp= stamp_t++;

    // Every 16 updates
    if (stamp_t >> 4 != stamp_tmp >> 4)
    {
        rdm = new RouteDemoMessage ();
        populateWSM (rdm);
        rdm->setRoadId (mobility->getRoadId ().c_str ());

        //delete (rdm);
        sendDelayedDown (rdm, 1);
    }
}

