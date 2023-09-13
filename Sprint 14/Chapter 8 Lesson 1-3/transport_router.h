#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport {

class Router {
public:
    //Router() = default;

    Router(const int bus_wait_time, const double bus_velocity)
        : bus_wait_time_(bus_wait_time)
        , bus_velocity_(bus_velocity) {}

    Router(const Router& settings, const Catalogue& catalogue) {
        bus_wait_time_ = settings.bus_wait_time_;
        bus_velocity_ = settings.bus_velocity_;
        BuildGraph(catalogue);
    }
    
    Router(const Router& settings, graph::DirectedWeightedGraph<double> graph, std::map<std::string, graph::VertexId> stop_ids)
        : bus_wait_time_(settings.bus_wait_time_)
        , bus_velocity_(settings.bus_velocity_)
        , graph_(graph)
        , stop_ids_(stop_ids) {
           router_ = std::make_unique<graph::Router<double>>(graph_);
       }
    
    const graph::DirectedWeightedGraph<double>& BuildGraph(const Catalogue& catalogue);
    const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    const graph::DirectedWeightedGraph<double>& GetGraph() const;
    void SetGraph(const graph::DirectedWeightedGraph<double> graph, const std::map<std::string, graph::VertexId> stop_ids);
    const int GetBusWaitTime() const;
    const double GetBusVelocity() const;
    const Router GetRouterSettings() const;
    const std::map<std::string, graph::VertexId> GetStopIds() const;

private:
    int bus_wait_time_ = 0;
    double bus_velocity_ = 0.0;

    graph::DirectedWeightedGraph<double> graph_;
    std::map<std::string, graph::VertexId> stop_ids_;
    std::unique_ptr<graph::Router<double>> router_;
};

} // namespace transport
