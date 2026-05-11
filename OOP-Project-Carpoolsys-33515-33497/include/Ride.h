#pragma once
#include <string>
#include <vector>
#include <ostream>
#include "enums.h"
using namespace std;

class Ride {
private:
    string rideId;
    string driverId;
    string vehicleId;
    string rideDate;  // "YYYY-MM-DD"
    string rideTime;  // "HH:MM"
    Area pickup;
    Area dropoff;
    GenderPref pref;
    RideStatus status;
    int seats;
    vector<string> passengers;

public:
    Ride();
    Ride(string rid, string drv, string veh,
         string date, string time, Area pu, Area drop,
         GenderPref gp, int seats);

    string getRideId() const;
    string getDriverId() const;
    string getVehicleId() const;
    string getRideDate() const;
    string getRideTime() const;
    Area getPickup() const;
    Area getDropoff() const;
    GenderPref getPref() const;
    RideStatus getStatus() const;
    int getSeats() const;
    const vector<string>& getPassengers() const;

    bool addPassenger(const string& stuId);
    bool removePassenger(const string& stuId);
    void markCompleted();
    void markCanceled();

    // raw load only — no side effects on seats/status
    void setStatus(RideStatus s);
    void setSeats(int s);
    void setRideDate(const string& d);
    void rawAddPassenger(const string& stuId);

    vector<string> everyone() const; // driver + all passengers
    void displayRide() const;

    bool operator<(const Ride& other) const;
    bool operator==(const Ride& other) const;

    friend ostream& operator<<(ostream& os, const Ride& r);
};
