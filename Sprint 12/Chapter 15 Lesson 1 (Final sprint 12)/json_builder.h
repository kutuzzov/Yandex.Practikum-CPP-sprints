#pragma once

#include "json.h"

#include <optional>

namespace json {

class Builder {
public:
    class DictItemContext;
    class DictKeyContext;
    class ArrayItemContext;

    Builder();
    DictKeyContext Key(std::string key);
    Builder& Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndDict();
    ArrayItemContext StartArray();
    Builder& EndArray();
    Node Build();
    Node GetNode(Node::Value value);

private:
    Node root_{ nullptr };
    std::vector<Node*> nodes_stack_;
    std::optional<std::string> key_{ std::nullopt };
};

class Builder::DictItemContext {
public:
    DictItemContext(Builder& builder);

    DictKeyContext Key(std::string key);
    Builder& EndDict();

private:
    Builder& builder_;
};

class Builder::ArrayItemContext {
public:
    ArrayItemContext(Builder& builder);

    ArrayItemContext Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndArray();
    ArrayItemContext StartArray();

private:
    Builder& builder_;
};

class Builder::DictKeyContext {
public:
    DictKeyContext(Builder& builder);

    DictItemContext Value(Node::Value value);
    ArrayItemContext StartArray();
    DictItemContext StartDict();

private:
    Builder& builder_;
};

} // namespace json