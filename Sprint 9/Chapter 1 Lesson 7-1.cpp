#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

template <typename Hash>
int FindCollisions(const Hash& hasher, istream& text) {
    int collisions = 0;
    unordered_map<size_t, unordered_set<string>> container;
    string word;
    
    while (text >> word) {
        size_t hash = hasher(word);
        auto [inserted_word, collision_bool] = container[hash].insert(move(word));
        collisions += collision_bool && container.at(hash).size() > 1;
    }
    
    return collisions;
}

struct DummyHash {
    size_t operator()(const string&) const {
        return 42;
    }
};

int main() {
    DummyHash dummy_hash;
    hash<string> good_hash;

    {
        istringstream stream("I love C++"s);
        cout << FindCollisions(dummy_hash, stream) << endl;
    }
    {
        istringstream stream("I love C++"s);
        cout << FindCollisions(good_hash, stream) << endl;
    }
}
