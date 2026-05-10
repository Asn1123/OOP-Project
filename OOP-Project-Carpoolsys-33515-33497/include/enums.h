#pragma once
#include <string>
#include <ctime>

enum class Gender     { MALE, FEMALE };
enum class GenderPref { BOYS_ONLY, GIRLS_ONLY, HYBRID };
enum class RideStatus { OPEN, FULL, COMPLETED, CANCELED };

enum class Area {
    CLIFTON = 0, DHA, GULSHAN, NAZIMABAD, NORTH_NAZIMABAD,
    FEDERAL_B_AREA, GULISTAN_E_JOHAR, MALIR, SADDAR,
    PECHS, DEFENCE, IBA_CITY_CAMPUS, IBA_MAIN_CAMPUS
};

inline constexpr int AREA_COUNT = 13;

inline std::string areaToStr(Area a) {
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
        case Area::DEFENCE:          return "Defence";
        case Area::IBA_CITY_CAMPUS:  return "IBA City";
        case Area::IBA_MAIN_CAMPUS:  return "IBA Main";
        default:                     return "Unknown";
    }
}

inline bool isIBACampus(Area a) {
    return a == Area::IBA_CITY_CAMPUS || a == Area::IBA_MAIN_CAMPUS;
}

inline std::string genderPrefToStr(GenderPref p) {
    switch (p) {
        case GenderPref::BOYS_ONLY:  return "Boys Only";
        case GenderPref::GIRLS_ONLY: return "Girls Only";
        default:                     return "Hybrid";
    }
}

inline std::string rideStatusToStr(RideStatus s) {
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

inline int timeToMins(const std::string& t) {
    if (t.size() < 5) return 0;
    return std::stoi(t.substr(0, 2)) * 60 + std::stoi(t.substr(3, 2));
}

inline std::string nowTime() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", lt->tm_hour, lt->tm_min);
    return buf;
}

inline std::string nowDate() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday);
    return buf;
}

inline std::string dateOffset(int days) {
    time_t t = time(nullptr) + (time_t)days * 86400;
    tm* lt = localtime(&t);
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday);
    return buf;
}

inline bool isDateTimePast(const std::string& date, const std::string& time) {
    std::string nd = nowDate();
    if (date < nd) return true;
    if (date > nd) return false;
    return timeToMins(time) <= timeToMins(nowTime());
}
