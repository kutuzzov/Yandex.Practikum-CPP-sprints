#pragma once

#include <regex>
#include <string>
#include <string_view>

class Date {
public:
    Date() {
    }
    Date(int year, int month, int day)
        : year_(year)
        , month_(month)
        , day_(day) {
    }

    // тут правильно использовать string_view, но regex его пока не поддерживает
    static Date FromString(const std::string& str) {
        static const std::regex date_regex(R"/(([0-9]{4})-([0-9]{2})-([0-9]{2}))/");
        std::smatch m;

        if (!std::regex_match(str, m, date_regex)) {
            return {};
        }

        return Date(std::stoi(m[1].str()), std::stoi(m[2].str()), std::stoi(m[3].str()));
    }

    static Date FromString(std::string_view str) {
        return FromString(std::string(str));
    }

    static int ComputeDistance(Date from, Date to) {
        auto date_to_seconds = [](Date date) {
            tm timestamp;
            timestamp.tm_sec = 0;
            timestamp.tm_min = 0;
            timestamp.tm_hour = 0;
            timestamp.tm_mday = date.day_;
            timestamp.tm_mon = date.month_ - 1;
            timestamp.tm_year = date.year_ - 1900;
            timestamp.tm_isdst = 0;
            return mktime(&timestamp);
        };

        static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
        return static_cast<int>((date_to_seconds(to) - date_to_seconds(from)) / SECONDS_IN_DAY);
    }

private:
    int year_ = 0, month_ = 0, day_ = 0;
};
