//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "route/RouteDemoRSU.h"

using namespace route;

Define_Module(route::RouteDemoRSU);

void RouteDemoRSU::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) 
    {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;

        stamp_t = 0;
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void RouteDemoRSU::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here
}

void RouteDemoRSU::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
}

// Your application has received a data message from another car or RSU code 
// for handling the message goes here, see TraciDemo11p.cc for examples
void RouteDemoRSU::onWSM(BaseFrame1609_4* wsm)
{
    RouteDemoMessage* rdm;

    rdm = (RouteDemoMessage*) wsm;
}

void RouteDemoRSU::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void RouteDemoRSU::handleSelfMsg(cMessage* msg)
{
    DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
}

// The vehicle has moved. Code that reacts to new positions goes here.
// Member variables such as currentPosition and currentSpeed are updated 
// in the parent class
void RouteDemoRSU::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
}
