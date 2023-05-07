#pragma once

#include "geo.h"
#include "domain.h"

#include <iostream>
#include <deque>
#include <stdexcept>
#include <optional>
#include <unordered_set>

namespace transport {

class Catalogue {
public:
    void AddRoute(Bus& bus);
    void AddStop(Stop& stop);
    const Bus* FindRoute(const std::string& route_number) const;
    Stop* FindStop(const std::string& stop_name) const;
    size_t UniqueStopsCount(const std::string& route_number) const;
    void SetDistance(Stop* from, Stop* to, int distance);
    int GetDistance(const Stop* from, const Stop* to) const;
    
private:
    std::deque<Bus> all_buses_;
    std::deque<Stop> all_stops_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
};

} // namespace transport
