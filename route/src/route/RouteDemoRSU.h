#pragma once
#include "route/route.h"
#include "route/RouteDemoMessage_m.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

using namespace veins;
namespace route { class RouteDemoRSU; }

struct CarInfo
{
    std::string roadId;
};
#define struct CarInfo CarInfo

class ROUTE_API route::RouteDemoRSU : public veins::DemoBaseApplLayer
{
private:
    uint8_t stamp_t;
    CarInfo cars_i[100];

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
