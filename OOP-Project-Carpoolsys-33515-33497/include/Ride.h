#pragma once
#include <string>
#include <vector>
#include <ostream>
#include "enums.h"

class Ride {
private:
    std::string  rideId;
    std::string  driverId;
    std::string  vehicleId;
    std::string  rideDate;      // "YYYY-MM-DD"
    std::string  rideTime;      // "HH:MM"
    Area         pickupArea;
    Area         dropOffArea;
    GenderPref   genderPref;
    RideStatus   status;
    int          seatsLeft;
    std::vector<std::string> passengerIds;

public:
    Ride();
    Ride(std::string rid, std::string drv, std::string veh,
         std::string date, std::string time, Area pickup, Area dropoff,
         GenderPref pref, int seats);

    std::string getRideId()      const;
    std::string getDriverId()    const;
    std::string getVehicleId()   const;
    std::string getRideDate()    const;
    std::string getRideTime()    const;
    Area        getPickupArea()  const;
    Area        getDropOffArea() const;
    GenderPref  getGenderPref()  const;
    RideStatus  getStatus()      const;
    int         getSeatsLeft()   const;
    const std::vector<std::string>& getPassengers() const;

    bool addPassenger   (const std::string& stuId);
    bool removePassenger(const std::string& stuId);
    void markCompleted();
    void markCanceled();

    // file-load only; no side effects on seatsLeft/status
    void setStatus(RideStatus s);
    void setSeatsLeft(int s);
    void setRideDate(const std::string& d);
    void rawAddPassenger(const std::string& stuId);

    std::vector<std::string> getAllParticipants() const;
    void displayRide() const;

    // Chronological ordering: earlier date+time is "less than"
    bool operator<(const Ride& other) const;
    bool operator==(const Ride& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Ride& r);
};
