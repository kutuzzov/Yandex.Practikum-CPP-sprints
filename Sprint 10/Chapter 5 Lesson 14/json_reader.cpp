/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "json_reader.h"

const json::Node& JsonReader::GetBaseRequests() const {
    if (!input_.GetRoot().AsMap().count("base_requests")) return dummy_;
    return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetStatRequests() const {
    if (!input_.GetRoot().AsMap().count("stat_requests")) return dummy_;
    return input_.GetRoot().AsMap().at("stat_requests");
}

transport::Bus JsonReader::FillRoute(const json::Dict& request_map) const {
    transport::Bus bus;
    bus.number = request_map.at("name").AsString();
    for (auto& stop : request_map.at("stops").AsArray()) {
        bus.stops.push_back(stop.AsString());
    }
    bus.circular_route = request_map.at("is_roundtrip").AsBool();
    
    return bus;
}

transport::Stop JsonReader::FillStop(const json::Dict& request_map) const {
    transport::Stop stop;
    stop.name = request_map.at("name").AsString();
    stop.coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    auto stop_distances = request_map.at("road_distances").AsMap();
    for (auto& [stop_name, dist] : stop_distances) {
        stop.stop_distances.emplace(stop_name, dist.AsInt());
    }
    return stop;
}

void JsonReader::FillCatalogue(transport::Catalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto stop = FillStop(request_stops_map);
            catalogue.AddStop(stop);
        }
    }
    for (auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsMap();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto bus = FillRoute(request_bus_map);
            catalogue.AddRoute(bus);
        }
    }
}
