/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include "request_handler.h"

const transport::BusInfo RequestHandler::GetBusStat(const std::string& route_number) const {
    transport::BusInfo route_info{};
    const transport::Bus* bus = catalogue_.FindRoute(route_number);
    
    if (!bus) throw std::invalid_argument("bus not found");
    if (bus->circular_route) route_info.stops_count = bus->stops.size();
    else route_info.stops_count = bus->stops.size() * 2 - 1;
    
    int route_length = 0;
    double geographic_length = 0.0;
    
    for (auto iter = bus->stops.begin(); iter + 1 != bus->stops.end(); ++iter) {
        auto from = catalogue_.FindStop(*iter);
        auto to = catalogue_.FindStop((*(iter + 1)));
        if (bus->circular_route) {
            route_length += catalogue_.GetDistance(from, to);
            geographic_length += ComputeDistance(catalogue_.FindStop(*iter)->coordinates,
                                                 catalogue_.FindStop(*(iter + 1))->coordinates);
        }
        else {
            route_length += catalogue_.GetDistance(from, to) + catalogue_.GetDistance(to, from);
            geographic_length += ComputeDistance(catalogue_.FindStop(*iter)->coordinates,
                                                 catalogue_.FindStop(*(iter + 1))->coordinates) * 2;
        }
    }
    if (bus->circular_route) {
        auto from = catalogue_.FindStop(*(bus->stops.end() - 1));
        auto to = catalogue_.FindStop(*(bus->stops.begin()));
        route_length += catalogue_.GetDistance(from, to);
        geographic_length += ComputeDistance(from->coordinates, to->coordinates);
    }
    
    route_info.unique_stops_count = catalogue_.UniqueStopsCount(route_number);
    route_info.route_length = route_length;
    route_info.curvature = route_length / geographic_length;
    
    return route_info;
}

const std::set<std::string> RequestHandler::GetBusesByStop(const std::string& stop_name) const {
    return catalogue_.FindStop(stop_name)->buses;
}

const json::Node RequestHandler::PrintStop(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!catalogue_.FindStop(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

const json::Node RequestHandler::PrintRoute(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& route_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!catalogue_.FindRoute(route_number)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = GetBusStat(route_number).curvature;
        
        result["route_length"] = GetBusStat(route_number).route_length;
        result["stop_count"] = static_cast<int>(GetBusStat(route_number).stops_count);
        result["unique_stop_count"] = static_cast<int>(GetBusStat(route_number).unique_stops_count);
    }
    return json::Node{ result };
}

void RequestHandler::ProcessRequests() const {
    json::Array result;
    const json::Array& arr = requests_.GetStatRequests().AsArray();
    for (auto& request : arr) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map).AsMap());
        }
        if (type == "Bus") {
            result.push_back(PrintRoute(request_map).AsMap());
        }
    }
    json::Print(json::Document{ result }, std::cout);
}
