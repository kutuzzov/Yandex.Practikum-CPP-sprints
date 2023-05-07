#include "transport_catalogue.h"

namespace transport {

void Catalogue::AddRoute(Bus& bus) {
    all_buses_.push_back(bus);
    for (const auto& route_stop : bus.stops) {
        for (auto& stop_ : all_stops_) {
            if (stop_.name == route_stop) stop_.buses.insert(bus.number);
        }
    }
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();
}

void Catalogue::AddStop(Stop& stop) {
    all_stops_.push_back(stop);
    stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();
}

const Bus* Catalogue::FindRoute(const std::string& route_number) const {
    return busname_to_bus_.count(route_number) ? busname_to_bus_.at(route_number) : nullptr;
}

Stop* Catalogue::FindStop(const std::string& stop_name) const {
    return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
}

size_t Catalogue::UniqueStopsCount(const std::string& route_number) const {
    std::unordered_set<std::string> unique_stops;
    for (const auto& stop : busname_to_bus_.at(route_number)->stops) {
        unique_stops.insert(stop);
    }
    return unique_stops.size();
}

void Catalogue::SetDistance(Stop* from, Stop* to, int distance) {
    from->stop_distances[to->name] = distance;
}

int Catalogue::GetDistance(const Stop* from, const Stop* to) const {
    if (from->stop_distances.count(to->name)) return from->stop_distances.at(to->name);
    else if (to->stop_distances.count(from->name)) return to->stop_distances.at(from->name);
    else return 0;
}

} // namespace transport
