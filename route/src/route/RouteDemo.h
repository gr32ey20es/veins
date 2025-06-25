#pragma once
#include "route/route.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

using namespace veins;
namespace route { class RouteDemo; }

class ROUTE_API route::RouteDemo : public veins::DemoBaseApplLayer
{
private:
    uint8_t stamp_t;

public:
    void initialize(int stage) override;
    void finish() override;

protected:
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
};
