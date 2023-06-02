#include <iostream>
#include <map>
#include <queue>
#include <set>

using namespace std;

struct Booking {
    const int64_t time;
    const int client_id;
    const int room_count;
};

class HotelManager {
public:
    void Book(const int64_t time, const string& hotel_name, const int client_id, const int room_count) {
        hotels_clients_[hotel_name].push({ time, client_id, room_count });
        for (auto& [name, structure] : hotels_clients_) {
            while (IsOutDate(time, structure.front().time)) {
                structure.pop();
            }
        }
    }

    int ComputeClientCount(const string& hotel_name) {
        set<int> clients;
        if (hotels_clients_.count(hotel_name)) {
            queue<Booking> copy = hotels_clients_.at(hotel_name);
            for (size_t i = 0; i < hotels_clients_.at(hotel_name).size(); ++i) {
                clients.insert(copy.front().client_id);
                copy.pop();
            }
        }
        return static_cast<int>(clients.size());
    }

    int ComputeRoomCount(const string& hotel_name) {
        int count = 0;
        if (hotels_clients_.count(hotel_name)) {
            queue<Booking> copy = hotels_clients_.at(hotel_name);
            for (size_t i = 0; i < hotels_clients_.at(hotel_name).size(); ++i) {
                count += copy.front().room_count;
                copy.pop();
            }
        }
        return count;
    }

private:
    map<string, queue<Booking>> hotels_clients_;

    bool IsOutDate(int64_t current_time, int64_t time) {
        return !(current_time >= 86400 ? (current_time - 86400) < time : true && time <= current_time);
    }
 };

int main() {
    HotelManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;

        if (query_type == "BOOK") {
            int64_t time;
            cin >> time;
            string hotel_name;
            cin >> hotel_name;
            int client_id, room_count;
            cin >> client_id >> room_count;
            manager.Book(time, hotel_name, client_id, room_count);
        }
        else {
            string hotel_name;
            cin >> hotel_name;
            if (query_type == "CLIENTS") {
                cout << manager.ComputeClientCount(hotel_name) << "\n";
            }
            else if (query_type == "ROOMS") {
                cout << manager.ComputeRoomCount(hotel_name) << "\n";
            }
        }
    }

    return 0;
}
