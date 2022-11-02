#include "log_duration.h"

#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

struct Temp {
    float day_temp_sum;
    size_t day_temp_size;
};

vector<float> ComputeAvgTemp(const vector<vector<float>>& measures) {
    if (measures.empty()) return {};

    vector<float> res(measures[0].size());
    vector<Temp> day_temp(measures[0].size());

    for (size_t i = 0; i < measures[0].size(); ++i) {
        size_t j;
        for (j = 0; j < measures.size(); ++j) {
            day_temp[i].day_temp_sum += (measures[j][i] > 0 ? measures[j][i] : 0);
            day_temp[i].day_temp_size += (measures[j][i] > 0 ? 1 : 0);
        }
        res[i] += (day_temp[i].day_temp_size > 0 ? day_temp[i].day_temp_sum / day_temp[i].day_temp_size : 0);
    }

    return res;
}

vector<float> GetRandomVector(int size) {
    static mt19937 engine;
    uniform_real_distribution<float> d(-100, 100);

    vector<float> res(size);
    for (int i = 0; i < size; ++i) {
        res[i] = d(engine);
    }

    return res;
}

void Test() {
    // 4 дня по 3 измерения
    vector<vector<float>> v = {
        {0, -1, -1},
        {1, -2, -2},
        {2, 3, -3},
        {3, 4, -4}
    };

    // среднее для 0-го измерения (1+2+3) / 3 = 2 (не учитывам 0)
    // среднее для 1-го измерения (3+4) / 2 = 3.5 (не учитывам -1, -2)
    // среднее для 2-го не определено (все температуры отрицательны), поэтому должен быть 0

    assert(ComputeAvgTemp(v) == vector<float>({ 2, 3.5f, 0 }));
}

int main() {
    setlocale(LC_ALL, "Russian");

    vector<vector<float>> data;
    data.reserve(5000);

    for (int i = 0; i < 5000; ++i) {
        data.push_back(GetRandomVector(5000));
    }

    vector<float> avg;
    {
        LOG_DURATION("ComputeAvgTemp"s);
        avg = ComputeAvgTemp(data);
    }

    cout << "Total mean: "s << accumulate(avg.begin(), avg.end(), 0.f) / avg.size() << endl;

    Test();

    return 0;
}
