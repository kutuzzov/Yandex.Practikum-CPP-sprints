/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#pragma once

#include "json.h"
#include "transport_catalogue.h"

#include <iostream>

class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(json::Load(input))
    {}

    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
    
    void FillCatalogue(transport::Catalogue& catalogue);
    
private:
    json::Document input_;
    json::Node dummy_ = nullptr;
    
    transport::Bus FillRoute(const json::Dict& request_map) const;
    transport::Stop FillStop(const json::Dict& request_map) const;
};
