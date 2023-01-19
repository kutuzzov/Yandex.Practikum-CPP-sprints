#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <execution>

#include "log_duration.h"

using namespace std;

string GenerateQuery(mt19937& generator, int max_length, int space_rate) {
    const int length = uniform_int_distribution(1, max_length)(generator);
    string query(length, ' ');
    for (char& c : query) {
        const int rnd = uniform_int_distribution(0, space_rate - 1)(generator);
        if (rnd > 0) {
            c = 'a' + (rnd - 1);
        }
    }
    return query;
}

template <typename Solver>
void Test(string_view mark, string_view s, Solver solver) {
    int result;
    {
        LOG_DURATION(mark);
        result = solver(s);
    }
    cout << result << endl;
}

#define TEST(solver) Test(#solver, s, solver)

int CountWords(string_view str) {
    return count(execution::par, str.begin(), str.end(), ' ') + 1;
}

int main() {
    // должно вывести 3
    cout << CountWords("pretty little octopus"sv) << endl;

    mt19937 generator;

    const string s = GenerateQuery(generator, 50'000'000, 4);

    TEST(CountWords);

    return 0;
}
