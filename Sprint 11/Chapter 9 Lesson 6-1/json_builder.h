#pragma once

#include "json.h"

#include <optional>

namespace json {

class Builder {
public:
    Builder();
    Builder& Key(std::string key);
    Builder& Value(Node::Value value);
    Builder& StartDict();
    Builder& StartArray();
    Builder& EndDict();
    Builder& EndArray();
    Node Build();
    Node GetNode(Node::Value value);

private:
    Node root_{nullptr};
    std::vector<Node*> nodes_stack_;
    std::optional<std::string> key_{std::nullopt};
};

} // namespace json
