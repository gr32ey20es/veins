package route;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple RouteDemoRSU extends DemoBaseApplLayer
{
    @class(route::RouteDemoRSU);
    @display("i=block/app2");
    string appName = default("RouteDemoRSU");
}
