#pragma once
#include <string>
#include <ctime>
using namespace std;

enum class Gender     { MALE, FEMALE };
enum class GenderPref { BOYS_ONLY, GIRLS_ONLY, HYBRID };
enum class RideStatus { OPEN, FULL, COMPLETED, CANCELED };

enum class Area {
    CLIFTON = 0, DHA, GULSHAN, NAZIMABAD, NORTH_NAZIMABAD,
    FEDERAL_B_AREA, GULISTAN_E_JOHAR, MALIR, SADDAR,
    PECHS, IBA_CITY_CAMPUS, IBA_MAIN_CAMPUS
};

inline constexpr int AREA_COUNT = 12;

inline string areaToStr(Area a) {
    switch (a) {
        case Area::CLIFTON:          return "Clifton";
        case Area::DHA:              return "DHA";
        case Area::GULSHAN:          return "Gulshan";
        case Area::NAZIMABAD:        return "Nazimabad";
        case Area::NORTH_NAZIMABAD:  return "North Nazimabad";
        case Area::FEDERAL_B_AREA:   return "Federal B Area";
        case Area::GULISTAN_E_JOHAR: return "Gulistan-e-Johar";
        case Area::MALIR:            return "Malir";
        case Area::SADDAR:           return "Saddar";
        case Area::PECHS:            return "PECHS";
        case Area::IBA_CITY_CAMPUS:  return "IBA City";
        case Area::IBA_MAIN_CAMPUS:  return "IBA Main";
        default:                     return "Unknown";
    }
}

inline bool isCampus(Area a) {
    return a == Area::IBA_CITY_CAMPUS || a == Area::IBA_MAIN_CAMPUS;
}

inline string prefStr(GenderPref p) {
    switch (p) {
        case GenderPref::BOYS_ONLY:  return "Boys Only";
        case GenderPref::GIRLS_ONLY: return "Girls Only";
        default:                     return "Hybrid";
    }
}

inline string statusStr(RideStatus s) {
    switch (s) {
        case RideStatus::OPEN:      return "Open";
        case RideStatus::FULL:      return "Full";
        case RideStatus::COMPLETED: return "Completed";
        case RideStatus::CANCELED:  return "Canceled";
        default:                    return "Unknown";
    }
}

inline Area intToArea(int i) {
    return (i >= 0 && i < AREA_COUNT) ? static_cast<Area>(i) : Area::IBA_CITY_CAMPUS;
}
inline int areaToInt(Area a) { return static_cast<int>(a); }

inline int toMins(const string& t) {
    if (t.size() < 5) return 0;
    return stoi(t.substr(0, 2)) * 60 + stoi(t.substr(3, 2));
}

inline string nowTime() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", lt->tm_hour, lt->tm_min);
    return buf;
}

inline string nowDate() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday);
    return buf;
}

inline string dayOffset(int days) {
    time_t t = time(nullptr) + (time_t)days * 86400;
    tm* lt = localtime(&t);
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday);
    return buf;
}

inline bool isPast(const string& date, const string& time) {
    string nd = nowDate();
    if (date < nd) return true;
    if (date > nd) return false;
    return toMins(time) <= toMins(nowTime());
}
