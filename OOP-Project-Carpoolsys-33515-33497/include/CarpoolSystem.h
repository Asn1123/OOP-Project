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

class CarpoolSystem {
private:
    std::map<std::string, Student>                   studentMap;
    std::map<std::string, Vehicle*>                  vehicleMap;
    std::map<std::string, Ride>                      rideMap;
    std::map<std::string, std::vector<Notification>> notifMap;

    int nextStudentId = 1;
    int nextVehicleId = 1;
    int nextRideId    = 1;

    void bumpCounter(const std::string& id, int& counter, const std::string& prefix);

    // apriori helpers
    std::map<std::string, int> countSingletons();
    std::string idToName(const std::string& id);

    // Recursive k-itemset generator (backtracking over sorted candidate set)
    void generateItemsets(const std::vector<std::vector<std::string>>& transactions,
                          std::vector<std::string>& current,
                          int startIdx,
                          const std::vector<std::string>& candidates,
                          int k,
                          std::map<std::vector<std::string>, int>& freqMap);

public:
    ~CarpoolSystem();

    // --- file I/O ---
    void loadStudentsFromFile  (const std::string& path);
    void loadCarsFromFile      (const std::string& path);
    void loadBikesFromFile     (const std::string& path);
    void loadRidesFromFile     (const std::string& path);
    void saveStudentsToFile    (const std::string& path) const;
    void saveRidesToFile       (const std::string& path) const;
    void saveCarsToFile        (const std::string& path) const;
    void saveBikesToFile       (const std::string& path) const;

    // --- account creation ---
    std::string createStudent(const std::string& name, const std::string& phone,
                              Gender g, bool isDriver);
    std::string addCar (const std::string& ownerId, const std::string& make,
                        const std::string& model, const std::string& plate, int capacity);
    std::string addBike(const std::string& ownerId, const std::string& make,
                        const std::string& model, const std::string& plate, int capacity);

    // --- account management ---
    bool updateStudent(const std::string& id, const std::string& name,
                       const std::string& phone, Gender g);
    bool updateVehicle(const std::string& ownerId, const std::string& mk,
                       const std::string& mdl, const std::string& plate, int cap);
    bool deleteStudent(const std::string& id);

    // --- ride lifecycle ---
    // returns new rideId or "" on failure
    std::string createRide(const std::string& driverId, const std::string& vehicleId,
                           const std::string& rideDate, const std::string& rideTime,
                           Area pickup, Area dropoff, GenderPref pref);
    bool bookRide  (const std::string& rideId, const std::string& studentId);
    bool cancelRide(const std::string& rideId);

    void checkAutoComplete();

    // --- queries ---
    std::string              getDriverActiveRide (const std::string& driverId) const;
    std::string              getVehicleForDriver (const std::string& driverId) const;
    std::vector<std::string> searchRides         (Area pickup, Area dropoff,
                                                  const std::string& requestedTime) const;
    std::vector<std::string> getDriverHistory    (const std::string& driverId) const;

    // --- GUI accessors ---
    const Student*  getStudent(const std::string& id) const;
    Ride*           getRide   (const std::string& id);
    const Ride*     getRide   (const std::string& id) const;
    const Vehicle*  getVehicle(const std::string& id) const;

    const std::vector<Notification>& getNotifications(const std::string& studentId) const;
    const std::map<std::string, Ride>&    getRideMap()    const;
    const std::map<std::string, Student>& getStudentMap() const;

    // --- console fallback ---
    void showAvailableRides()                    const;
    void showStudents()                          const;
    void showNotifications(const std::string& studentId) const;
    void runApriori(int minSupport = 2);
};
