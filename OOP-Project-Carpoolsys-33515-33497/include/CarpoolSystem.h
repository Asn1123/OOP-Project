#pragma once
#include <map>
#include <string>
#include <vector>
#include "Student.h"
#include "Vehicle.h"
#include "Car.h"
#include "Bike.h"
#include "Ride.h"
#include "Notification.h"
using namespace std;

class CarpoolSystem {
private:
    map<string, Student> students;
    map<string, Vehicle*> vehicles;
    map<string, Ride> rides;
    map<string, vector<Notification>> notifs;

    int nextStu = 1;
    int nextVeh = 1;
    int nextRid = 1;

    void bumpId(const string& id, int& counter, const string& prefix);

    // apriori helpers
    map<string, int> countSingles();
    string idToName(const string& id);

    // backtracking k-itemset counter
    void genItemsets(const vector<vector<string>>& txns,
                     vector<string>& cur,
                     int start,
                     const vector<string>& cands,
                     int k,
                     map<vector<string>, int>& freq);

public:
    ~CarpoolSystem();

    // --- file I/O ---
    void loadStudentsFromFile(const string& path);
    void loadCarsFromFile(const string& path);
    void loadBikesFromFile(const string& path);
    void loadRidesFromFile(const string& path);
    void saveStudentsToFile(const string& path) const;
    void saveRidesToFile(const string& path) const;
    void saveCarsToFile(const string& path) const;
    void saveBikesToFile(const string& path) const;

    // --- account creation ---
    string createStudent(const string& name, const string& phone,
                         Gender g, bool isDriver);
    string addCar(const string& ownerId, const string& make,
                  const string& model, const string& plate, int cap);
    string addBike(const string& ownerId, const string& make,
                   const string& model, const string& plate, int cap);

    // --- account management ---
    bool updateStudent(const string& id, const string& name,
                       const string& phone, Gender g);
    bool updateVehicle(const string& ownerId, const string& mk,
                       const string& mdl, const string& plate, int cap);
    bool deleteStudent(const string& id);

    // --- ride lifecycle ---
    // returns new rideId or "" on failure
    string createRide(const string& driverId, const string& vehicleId,
                      const string& rideDate, const string& rideTime,
                      Area pickup, Area dropoff, GenderPref pref);
    bool bookRide(const string& rideId, const string& stuId);
    bool cancelRide(const string& rideId);

    void checkAutoComplete();

    // --- queries ---
    string activeRide(const string& driverId) const;
    string vehicleOf(const string& driverId) const;
    vector<string> searchRides(Area pickup, Area dropoff, const string& wantTime) const;
    vector<string> driverHistory(const string& driverId) const;

    // --- accessors ---
    const Student* getStudent(const string& id) const;
    Ride* getRide(const string& id);
    const Ride* getRide(const string& id) const;
    const Vehicle* getVehicle(const string& id) const;

    const vector<Notification>& getNotifications(const string& stuId) const;
    const map<string, Ride>& getRides() const;
    const map<string, Student>& getStudents() const;

    // --- console fallback ---
    void showAvailableRides() const;
    void showStudents() const;
    void showNotifications(const string& stuId) const;
    void runApriori(int minSupport = 2);
};
