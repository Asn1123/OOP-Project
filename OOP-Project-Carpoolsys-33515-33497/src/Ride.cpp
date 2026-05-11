#include "Ride.h"
#include <iostream>
using namespace std;

Ride::Ride()
    : pickup(Area::IBA_CITY_CAMPUS), dropoff(Area::IBA_CITY_CAMPUS),
      pref(GenderPref::HYBRID), status(RideStatus::OPEN), seats(0) {}

Ride::Ride(string rid, string drv, string veh,
           string date, string time, Area pu, Area drop,
           GenderPref gp, int cap)
    : rideId(rid), driverId(drv), vehicleId(veh), rideDate(date), rideTime(time),
      pickup(pu), dropoff(drop), pref(gp),
      status(RideStatus::OPEN), seats(cap) {}

string Ride::getRideId() const { return rideId; }
string Ride::getDriverId() const { return driverId; }
string Ride::getVehicleId() const { return vehicleId; }
string Ride::getRideDate() const { return rideDate; }
string Ride::getRideTime() const { return rideTime; }
Area Ride::getPickup() const { return pickup; }
Area Ride::getDropoff() const { return dropoff; }
GenderPref Ride::getPref() const { return pref; }
RideStatus Ride::getStatus() const { return status; }
int Ride::getSeats() const { return seats; }

const vector<string>& Ride::getPassengers() const { return passengers; }

bool Ride::addPassenger(const string& stuId) {
    if (seats <= 0 || status != RideStatus::OPEN) return false;
    for (auto& p : passengers) if (p == stuId) return false; // no duplicate
    passengers.push_back(stuId);
    if (--seats == 0) status = RideStatus::FULL;
    return true;
}

void Ride::markCompleted() { status = RideStatus::COMPLETED; }
void Ride::markCanceled() { status = RideStatus::CANCELED; }

bool Ride::removePassenger(const string& stuId) {
    for (auto it = passengers.begin(); it != passengers.end(); ++it) {
        if (*it == stuId) {
            passengers.erase(it);
            seats++;
            if (status == RideStatus::FULL) status = RideStatus::OPEN;
            return true;
        }
    }
    return false;
}

void Ride::setStatus(RideStatus s) { status = s; }
void Ride::setSeats(int s) { seats = s; }
void Ride::setRideDate(const string& d) { rideDate = d; }
void Ride::rawAddPassenger(const string& stuId) { passengers.push_back(stuId); }

vector<string> Ride::everyone() const {
    vector<string> all = passengers;
    all.push_back(driverId);
    return all;
}

void Ride::displayRide() const {
    cout << "  Ride    : " << rideId << "\n"
         << "  Date    : " << rideDate << "\n"
         << "  Time    : " << rideTime << "\n"
         << "  Pickup  : " << areaToStr(pickup) << "\n"
         << "  Drop-off: " << areaToStr(dropoff) << "\n"
         << "  Pref    : " << prefStr(pref) << "\n"
         << "  Status  : " << statusStr(status) << "\n"
         << "  Seats   : " << seats << "\n";
}

bool Ride::operator<(const Ride& other) const {
    if (rideDate != other.rideDate) return rideDate < other.rideDate;
    return rideTime < other.rideTime;
}

bool Ride::operator==(const Ride& other) const { return rideId == other.rideId; }

ostream& operator<<(ostream& os, const Ride& r) {
    os << "[" << r.rideId << "] "
       << areaToStr(r.pickup) << " -> " << areaToStr(r.dropoff)
       << " | " << r.rideDate << " " << r.rideTime
       << " | " << statusStr(r.status)
       << " | Seats left: " << r.seats;
    return os;
}
