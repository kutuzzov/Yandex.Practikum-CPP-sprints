#include "serialization.h"

#include <fstream>

namespace serialization {

void Serialize(transport::Catalogue& db, std::ostream& out) {
	proto_transport::TransportCatalogue proto_db;

	SerializeStops(db, proto_db);
	SerializeStopDistances(db, proto_db);
	SerializeBuses(db, proto_db);
	
	proto_db.SerializeToOstream(&out);
}

std::tuple<transport::Catalogue, renderer::MapRenderer, transport::Router> Deserialize(std::istream& input) {
	proto_transport::TransportCatalogue proto_db;
	proto_db.ParseFromIstream(&input);

	transport::Catalogue db;
	renderer::MapRenderer renderer{};
	transport::Router router{};

	DeserializeStops(db, proto_db);
	DeserializeStopDistances(db, proto_db);
	DeserializeBuses(db, proto_db);

	return { std::move(db), std::move(renderer), std::move(router) };
}

void SerializeStops(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db) {
	const auto all_stops = db.GetSortedAllStops();
	for (const auto& stop : all_stops) {
		proto_transport::Stop proto_stop;
		proto_stop.set_name(stop.second->name);
		proto_stop.mutable_coordinates()->set_lat(stop.second->coordinates.lat);
		proto_stop.mutable_coordinates()->set_lng(stop.second->coordinates.lng);
		for (const auto& bus : stop.second->buses_by_stop) {
			proto_stop.add_buses_by_stop(bus);
		}
		*proto_db.add_stops() = std::move(proto_stop);
	}
}

void SerializeStopDistances(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db) {
	for (const auto& [stop_pair, distance] : db.GetStopDistances()) {
		proto_transport::StopDistanses proto_stop_distances;
		proto_stop_distances.set_from(stop_pair.first->name);
		proto_stop_distances.set_to(stop_pair.second->name);
		proto_stop_distances.set_distance(distance);

		*proto_db.add_stop_distances() = std::move(proto_stop_distances);
	}
}

void SerializeBuses(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db) {
	const auto all_buses = db.GetSortedAllBuses();
	for (const auto& bus : all_buses) {
		proto_transport::Bus proto_bus;
		proto_bus.set_number(bus.second->number);
		for (const auto* stop : bus.second->stops) {
			*proto_bus.mutable_stops()->Add() = stop->name;
		}
		proto_bus.set_is_circle(bus.second->is_circle);
		*proto_db.add_buses() = std::move(proto_bus);
	}
}

void DeserializeStops(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db) {
	for (size_t i = 0; i < proto_db.stops_size(); ++i) {
		const proto_transport::Stop& proto_stop = proto_db.stops(i);
		db.AddStop(proto_stop.name(), { proto_stop.coordinates().lat(), proto_stop.coordinates().lng() });
	}
}

void DeserializeStopDistances(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db) {
	for (size_t i = 0; i < proto_db.stop_distances_size(); ++i) {
		const proto_transport::StopDistanses& proto_stop_distances = proto_db.stop_distances(i);
		const transport::Stop* from = db.FindStop(proto_stop_distances.from());
		const transport::Stop* to = db.FindStop(proto_stop_distances.to());
		db.SetDistance(from, to, proto_stop_distances.distance());
	}
}

void DeserializeBuses(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db) {
	for (size_t i = 0; i < proto_db.buses_size(); ++i) {
		const proto_transport::Bus& proto_bus = proto_db.buses(i);
		std::vector<const transport::Stop*> stops(proto_bus.stops_size());
		for (size_t j = 0; j < stops.size(); ++j) {
			stops[j] = db.FindStop(proto_bus.stops(j));
		}
		db.AddRoute(proto_bus.number(), stops, proto_bus.is_circle());
	}
}

} // serialization