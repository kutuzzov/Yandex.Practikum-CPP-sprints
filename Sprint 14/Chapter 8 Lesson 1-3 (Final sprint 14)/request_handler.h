#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <sstream>
#include <optional>

class RequestHandler {
public:
    RequestHandler(const transport::Catalogue& catalogue, const renderer::MapRenderer& renderer, const transport::Router& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , router_(router)
    {
    }

    std::optional<transport::BusStat> GetBusStat(const std::string_view bus_number) const;
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;
    bool IsBusNumber(const std::string_view bus_number) const;
    bool IsStopName(const std::string_view stop_name) const;
    const std::optional<graph::Router<double>::RouteInfo> GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    const graph::DirectedWeightedGraph<double>& GetRouterGraph() const;

    svg::Document RenderMap() const;

private:
    const transport::Catalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const transport::Router& router_;
};