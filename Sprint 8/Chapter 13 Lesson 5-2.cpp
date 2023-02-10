#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <vector>
#include <execution>
#include <future>

using namespace std;

template <typename Strings>
void PrintStrings(const Strings& strings) {
    for (string_view s : strings) {
        cout << s << " ";
    }
    cout << endl;
}

template<typename ExecutionPolicy, typename ForwardRange, typename Function>
void ForEach(ExecutionPolicy&& policy, ForwardRange& range, Function function) {
    if constexpr (is_same_v<decay_t<ExecutionPolicy>, execution::sequenced_policy>
                  || is_same_v<typename iterator_traits<typename ForwardRange::iterator>::iterator_category,
                        random_access_iterator_tag>) for_each(policy, range.begin(), range.end(), function);
    else {
        const size_t part_count = 4;
        const size_t part_length = range.size() / part_count;
        auto part_begin = range.begin();
        auto part_end = next(part_begin, part_length);

        vector<future<void>> futures;
        for (size_t i = 0; i < part_count; ++i) {
            if (i == part_count - 1) part_end = range.end();
            else part_end = next(part_begin, part_length);

            futures.push_back(async([function, part_begin, part_end] {
                for_each(part_begin, part_end, function);
                }));
            part_begin = part_end;
        }
    }
}

template<typename ForwardRange, typename Function>
void ForEach(ForwardRange& range, Function function) {
    ForEach(execution::seq, range, function);
}

int main() {
    auto reverser = [](string& s) { reverse(s.begin(), s.end()); };

    list<string> strings_list = { "cat", "dog", "code" };

    ForEach(strings_list, reverser);
    PrintStrings(strings_list);
    // tac god edoc

    ForEach(execution::seq, strings_list, reverser);
    PrintStrings(strings_list);
    // cat dog code

    // единственный из вызовов, где должна работать ваша версия
    // из предыдущего задания
    ForEach(execution::par, strings_list, reverser);
    PrintStrings(strings_list);
    // tac god edoc

    vector<string> strings_vector = { "cat", "dog", "code" };

    ForEach(strings_vector, reverser);
    PrintStrings(strings_vector);
    // tac god edoc

    ForEach(execution::seq, strings_vector, reverser);
    PrintStrings(strings_vector);
    // cat dog code

    ForEach(execution::par, strings_vector, reverser);
    PrintStrings(strings_vector);
    // tac god edoc

    return 0;
}
