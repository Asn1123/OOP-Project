#include "Ride.h"
#include <iostream>

Ride::Ride()
    : pickupArea(Area::IBA_CITY_CAMPUS), dropOffArea(Area::IBA_CITY_CAMPUS),
      genderPref(GenderPref::HYBRID), status(RideStatus::OPEN), seatsLeft(0) {}

Ride::Ride(std::string rid, std::string drv, std::string veh,
           std::string date, std::string time, Area pickup, Area dropoff,
           GenderPref pref, int seats)
    : rideId(rid), driverId(drv), vehicleId(veh), rideDate(date), rideTime(time),
      pickupArea(pickup), dropOffArea(dropoff), genderPref(pref),
      status(RideStatus::OPEN), seatsLeft(seats) {}

std::string Ride::getRideId()      const { return rideId; }
std::string Ride::getDriverId()    const { return driverId; }
std::string Ride::getVehicleId()   const { return vehicleId; }
std::string Ride::getRideDate()    const { return rideDate; }
std::string Ride::getRideTime()    const { return rideTime; }
Area        Ride::getPickupArea()  const { return pickupArea; }
Area        Ride::getDropOffArea() const { return dropOffArea; }
GenderPref  Ride::getGenderPref()  const { return genderPref; }
RideStatus  Ride::getStatus()      const { return status; }
int         Ride::getSeatsLeft()   const { return seatsLeft; }

const std::vector<std::string>& Ride::getPassengers() const { return passengerIds; }

bool Ride::addPassenger(const std::string& stuId) {
    if (seatsLeft <= 0 || status != RideStatus::OPEN) return false;
    // prevent duplicate booking
    for (auto& pid : passengerIds)
        if (pid == stuId) return false;
    passengerIds.push_back(stuId);
    if (--seatsLeft == 0) status = RideStatus::FULL;
    return true;
}

void Ride::markCompleted() { status = RideStatus::COMPLETED; }
void Ride::markCanceled()  { status = RideStatus::CANCELED; }

bool Ride::removePassenger(const std::string& stuId) {
    for (auto it = passengerIds.begin(); it != passengerIds.end(); ++it) {
        if (*it == stuId) {
            passengerIds.erase(it);
            seatsLeft++;
            if (status == RideStatus::FULL) status = RideStatus::OPEN;
            return true;
        }
    }
    return false;
}

void Ride::setStatus(RideStatus s)               { status = s; }
void Ride::setSeatsLeft(int s)                   { seatsLeft = s; }
void Ride::setRideDate(const std::string& d)     { rideDate = d; }
void Ride::rawAddPassenger(const std::string& stuId) { passengerIds.push_back(stuId); }

std::vector<std::string> Ride::getAllParticipants() const {
    std::vector<std::string> all = passengerIds;
    all.push_back(driverId);
    return all;
}

void Ride::displayRide() const {
    std::cout << "  Ride    : " << rideId << "\n"
              << "  Date    : " << rideDate << "\n"
              << "  Time    : " << rideTime << "\n"
              << "  Pickup  : " << areaToStr(pickupArea) << "\n"
              << "  Drop-off: " << areaToStr(dropOffArea) << "\n"
              << "  Pref    : " << genderPrefToStr(genderPref) << "\n"
              << "  Status  : " << rideStatusToStr(status) << "\n"
              << "  Seats   : " << seatsLeft << "\n";
}

bool Ride::operator<(const Ride& other) const {
    if (rideDate != other.rideDate) return rideDate < other.rideDate;
    return rideTime < other.rideTime;
}

bool Ride::operator==(const Ride& other) const { return rideId == other.rideId; }

std::ostream& operator<<(std::ostream& os, const Ride& r) {
    os << "[" << r.rideId << "] "
       << areaToStr(r.pickupArea) << " -> " << areaToStr(r.dropOffArea)
       << " | " << r.rideDate << " " << r.rideTime
       << " | " << rideStatusToStr(r.status)
       << " | Seats left: " << r.seatsLeft;
    return os;
}
