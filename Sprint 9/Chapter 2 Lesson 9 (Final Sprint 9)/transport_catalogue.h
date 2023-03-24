#pragma once

#include "geo.h"

#include <iostream>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <optional>
#include <unordered_set>
#include <set>

namespace transport {

struct Bus {
    std::string number;
    std::vector<std::string> stops;
    bool circular_route;
};

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    std::set<std::string> buses;
    std::unordered_map<std::string, int> stop_distances;
};

struct RouteInfo {
    size_t stops_count;
    size_t unique_stops_count;
    double route_length;
    double curvature;
};

class Catalogue {
public:
    void AddRoute(Bus& bus);
    void AddStop(Stop& stop);
    const Bus* FindRoute(const std::string& route_number) const;
    Stop* FindStop(const std::string& stop_name) const;
    const RouteInfo RouteInformation(const std::string& route_number) const;
    size_t UniqueStopsCount(const std::string& route_number) const;
    const std::set<std::string> GetBusesOnStop(const std::string& stop_name) const;
    void SetDistance(Stop* from, Stop* to, int distance);
    int GetDistance(const Stop* from, const Stop* to) const;
    
private:
    std::deque<Bus> all_buses_;
    std::deque<Stop> all_stops_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
};

} // namespace transport
