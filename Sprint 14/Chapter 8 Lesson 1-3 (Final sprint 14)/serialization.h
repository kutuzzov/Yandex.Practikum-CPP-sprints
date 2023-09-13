#pragma once

#include "json.h"
#include "transport_catalogue.pb.h"
#include "svg.pb.h"
#include "map_renderer.pb.h"
#include "transport_catalogue.h"
#include "request_handler.h"

namespace serialization {

void Serialize(const transport::Catalogue& db, const renderer::MapRenderer& renderer, const transport::Router& router, std::ostream& out);
std::tuple<transport::Catalogue, renderer::MapRenderer, transport::Router, graph::DirectedWeightedGraph<double>, std::map<std::string, graph::VertexId>> Deserialize(std::istream& input);

void SerializeStops(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db);
void SerializeStopDistances(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db);
void SerializeBuses(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db);
void SerializeRenderSettings(const renderer::MapRenderer& renderer, proto_transport::TransportCatalogue& proto_db);
proto_map::Point SerializePoint(const svg::Point& point);
proto_map::Color SerializeColor(const svg::Color& color);
proto_map::Rgb SerializeRgb(const svg::Rgb& rgb);
proto_map::Rgba SerializeRgba(const svg::Rgba& rgba);
void SerializeRouter(const transport::Router& router, proto_transport::TransportCatalogue& proto_db);
proto_transport::RouterSettings SerializeRouterSettings(const transport::Router& router, proto_transport::TransportCatalogue& proto_db);
proto_graph::Graph SerializeGraph(const transport::Router& router, proto_transport::TransportCatalogue& proto_db);

void DeserializeStops(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db);
void DeserializeStopDistances(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db);
void DeserializeBuses(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db);
renderer::MapRenderer DeserializeRenderSettings(renderer::RenderSettings& render_settings, const proto_transport::TransportCatalogue& proto_db);
svg::Point DeserializePoint(const proto_map::Point& proto_point);
svg::Color DeserializeColor(const proto_map::Color& proto_color);
transport::Router DeserializeRouterSettings(const proto_transport::TransportCatalogue& proto_db);
graph::DirectedWeightedGraph<double> DeserializeGraph(const proto_transport::TransportCatalogue& proto_db);
std::map<std::string, graph::VertexId> DeserializeStopIds(const proto_transport::TransportCatalogue& proto_db);

} // serialization
