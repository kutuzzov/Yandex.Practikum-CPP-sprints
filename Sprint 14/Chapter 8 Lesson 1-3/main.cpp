#include <fstream>
#include <iostream>
#include <string_view>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        JsonReader json_input(std::cin);
        transport::Catalogue catalogue;
        json_input.FillCatalogue(catalogue);

        const auto& routing_settings = json_input.FillRoutingSettings(json_input.GetRoutingSettings());
        const transport::Router router = { routing_settings, catalogue };
        const auto& render_settings = json_input.GetRenderSettings();
        const renderer::MapRenderer renderer = json_input.FillRenderSettings(render_settings);
        const auto& serialization_settings = json_input.GetSerializationSettings();
        
        std::ofstream fout(serialization_settings.AsDict().at("file"s).AsString(), std::ios::binary);
        if (fout.is_open()) {
            serialization::Serialize(catalogue, renderer, router, fout);
        }
}
    else if (mode == "process_requests"sv) {
        JsonReader json_input(std::cin);
        std::ifstream db_file(json_input.GetSerializationSettings().AsDict().at("file"s).AsString(), std::ios::binary);
        if (db_file) {
            auto [catalogue, renderer, router, graph, stop_ids] = serialization::Deserialize(db_file);
            const auto& stat_requests = json_input.GetStatRequests();
            router.SetGraph(graph, stop_ids);
            RequestHandler rh = { catalogue, renderer, router };
            
            json_input.ProcessRequests(stat_requests, rh);
        }
    }
    else {
        PrintUsage();
        return 1;
    }
}
