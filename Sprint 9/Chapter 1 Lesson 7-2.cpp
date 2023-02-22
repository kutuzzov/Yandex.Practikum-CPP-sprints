#include <iostream>
#include <string>
#include <cmath>

using namespace std;

struct Circle {
    double x;
    double y;
    double r;
};

struct Dumbbell {
    Circle circle1;
    Circle circle2;
    string text;
};

struct DumbbellHash {
    size_t operator() (const Dumbbell& dumbbell) {
        size_t h_x = d_hasher_(dumbbell.circle1.x);
        size_t h_y = d_hasher_(dumbbell.circle1.y);
        size_t h_r = d_hasher_(dumbbell.circle1.r);
        size_t hash_circle1 = h_x + h_y * 37 + h_r * (37 * 37);
        
        h_x = d_hasher_(dumbbell.circle2.x);
        h_y = d_hasher_(dumbbell.circle2.y);
        h_r = d_hasher_(dumbbell.circle2.r);
        size_t hash_circle2 = h_x + h_y * 37 + h_r * (37 * 37);
        
        size_t hash_text = text_hasher_(dumbbell.text);
        
        return (hash_circle1 * static_cast<size_t>(pow(37, 4)) +
                hash_circle2 * static_cast<size_t>(pow(37, 1)) +
                hash_text * static_cast<size_t>(pow(37, 0)));
    }

private:
    std::hash<double> d_hasher_;
    std::hash<string> text_hasher_;
};

int main() {
    DumbbellHash hash;
    Dumbbell dumbbell{{10, 11.5, 2.3}, {3.14, 15, -8}, "abc"s};
    cout << "Dumbbell hash "s << hash(dumbbell);
}
