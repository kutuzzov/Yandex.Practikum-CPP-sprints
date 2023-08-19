#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>

// Serialization

template <typename T>
void Serialize(T val, std::ostream& out) {
    out.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

inline void Serialize(const std::string& str, std::ostream& out) {
    Serialize(str.size(), out);
    out.write(str.data(), str.size());
}

template <typename T>
void Serialize(const std::vector<T>& val, std::ostream& out) {
    Serialize(val.size(), out);
    for (const auto& v : val) {
        Serialize(v, out);
    }
}

template <typename T1, typename T2>
void Serialize(const std::map<T1, T2>& val, std::ostream& out) {
    Serialize(val.size(), out);
    for (const auto& v : val) {
        Serialize(v.first, out);
        Serialize(v.second, out);
    }
}

// Deserialization

template <typename T>
void Deserialize(std::istream& in, T& val) {
    in.read(reinterpret_cast<char*>(&val), sizeof(val));
}

inline void Deserialize(std::istream& in, std::string& str) {
    size_t n;
    Deserialize(in, n);
    str.resize(n);
    in.read(str.data(), n);
}

template <typename T>
void Deserialize(std::istream& in, std::vector<T>& val) {
    size_t n;
    Deserialize(in, n);
    val.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        T value;
        Deserialize(in, value);
        val.push_back(std::move(value));
    }
}

template <typename T1, typename T2>
void Deserialize(std::istream& in, std::map<T1, T2>& val) {
    size_t n;
    Deserialize(in, n);
    for (size_t i = 0; i < n; ++i) {
        T1 key;
        T2 value;
        Deserialize(in, key);
        Deserialize(in, value);
        val.insert(
            std::make_pair(
                std::move(key),
                std::move(value)
            )
        );
    }
}

// Поскольку функции шаблонные, их реализации будем писать прямо в h-файле.
// Не забудьте объявить нешаблонные функции как inline.
