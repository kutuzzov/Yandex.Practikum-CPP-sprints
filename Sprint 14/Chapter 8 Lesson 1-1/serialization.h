#pragma once

#include "json.h"
#include "transport_catalogue.pb.h"
#include "transport_catalogue.h"
#include "request_handler.h"

namespace serialization {

void Serialize(transport::Catalogue& db, std::ostream& output);
std::tuple<transport::Catalogue, renderer::MapRenderer, transport::Router> Deserialize(std::istream& input);

void SerializeStops(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db);
void SerializeStopDistances(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db);
void SerializeBuses(const transport::Catalogue& db, proto_transport::TransportCatalogue& proto_db);

void DeserializeStops(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db);
void DeserializeStopDistances(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db);
void DeserializeBuses(transport::Catalogue& db, const proto_transport::TransportCatalogue& proto_db);

} // serialization