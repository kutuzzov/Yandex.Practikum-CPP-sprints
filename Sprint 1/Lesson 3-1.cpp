#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int count;
    cin >> count;
    vector<pair<int, string>> people {};
    for (int i = 0; i < count; ++i) {
        string name;
        int age;
        cin >> name >> age;
        people.push_back({age, name});  
    }
    
    sort(people.begin(), people.end());
    reverse(people.begin(), people.end());
    
    for (const auto& element : people) {
        cout << element.second << " "s << endl;
    }
}
