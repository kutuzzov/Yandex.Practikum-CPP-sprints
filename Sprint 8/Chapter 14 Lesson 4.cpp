#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>
#include <execution>
#include <mutex>

#include "log_duration.h"

using namespace std;

string GenerateWord(mt19937& generator, int max_length) {
    const int length = uniform_int_distribution(1, max_length)(generator);
    string word;
    word.reserve(length);
    for (int i = 0; i < length; ++i) {
        word.push_back(uniform_int_distribution(static_cast<int>('a'), static_cast<int>('z'))(generator));
    }
    return word;
}

template <template <typename> typename Container>
Container<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
    vector<string> words;
    words.reserve(word_count);
    for (int i = 0; i < word_count; ++i) {
        words.push_back(GenerateWord(generator, max_length));
    }
    return Container(words.begin(), words.end());
}

template <typename Strings, typename Predicate, typename Function>
void Test(string_view mark, const Strings& strings, Predicate predicate, Function function) {
    LOG_DURATION(mark);
    const auto result = function(strings, predicate);
    cout << result.size() << " " << result[5].substr(0, 5) << endl;
}

#define TEST(function) \
    Test(#function, strings, predicate, function<vector<string>, decltype(predicate)>)

template <typename Container, typename Predicate>
vector<typename Container::value_type> CopyIfUnordered(const Container& container,
    Predicate predicate) {
    vector<typename Container::value_type> result;
    result.reserve(container.size());
    mutex m;
    for_each(
        execution::par,
        container.begin(), container.end(),
        [&predicate, &m, &result](const auto& value) {
            if (predicate(value)) {
                typename Container::value_type* temp;
                {
                    lock_guard guard(m);
                    temp = &result.emplace_back(move(value));
                }
                *temp = value;
            }
        }
    );

    return result;
}

int main() {
    vector<int> numbers(1'000);
    iota(numbers.begin(), numbers.end(), 0);

    const vector<int> even_numbers = CopyIfUnordered(numbers, [](int number) {
        return number % 2 == 0;
        });
    for (const int number : even_numbers) {
        cout << number << " "s;
    }
    cout << endl;
    // выведет все чётные числа от 0 до 999

    mt19937 generator;

    const auto strings = GenerateDictionary<vector>(generator, 50'000, 3000);
    auto predicate = [](const string& s) {
        return count(s.begin(), s.end(), 'a') < 100;
    };

    TEST(CopyIfUnordered);
}
