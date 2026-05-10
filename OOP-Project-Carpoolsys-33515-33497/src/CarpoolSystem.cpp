#include "CarpoolSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

static const std::vector<Notification> EMPTY_NOTIFS;

CarpoolSystem::~CarpoolSystem() {
    for (auto& [id, ptr] : vehicleMap) delete ptr;
}

void CarpoolSystem::bumpCounter(const std::string& id, int& counter, const std::string& prefix) {
    if (id.rfind(prefix, 0) == 0) {
        int n = std::stoi(id.substr(prefix.size()));
        if (n >= counter) counter = n + 1;
    }
}

void CarpoolSystem::loadStudentsFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) { std::cout << "  [warn] cannot open " << path << "\n"; return; }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string tok;
        while (std::getline(ss, tok, ',')) fields.push_back(tok);
        if (fields.size() < 6) continue;

        std::string id    = fields[0];
        std::string name  = fields[1];
        std::string phone = fields[2];
        std::string gStr  = fields[3];

        // 7-field old format had homeArea at index 4
        std::string dStr = (fields.size() >= 7) ? fields[5] : fields[4];
        std::string rStr = (fields.size() >= 7) ? (fields.size() > 6 ? fields[6] : "0")
                                                 : fields[5];

        Gender g = (gStr == "0") ? Gender::MALE : Gender::FEMALE;
        bool   d = (dStr == "1");
        int    rInt = 0;
        try { rInt = rStr.empty() ? 0 : std::stoi(rStr); } catch (...) {}
        studentMap[id] = Student(id, name, phone, g, d, rInt);
        bumpCounter(id, nextStudentId, "STU-");
    }
}

void CarpoolSystem::loadCarsFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) { std::cout << "  [warn] cannot open " << path << "\n"; return; }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string id, oid, mk, mdl, plate, cap;
        std::getline(ss, id,    ',');
        std::getline(ss, oid,   ',');
        std::getline(ss, mk,    ',');
        std::getline(ss, mdl,   ',');
        std::getline(ss, plate, ',');
        std::getline(ss, cap,   ',');
        int capInt = 1;
        try { capInt = std::stoi(cap); } catch (...) { continue; }
        vehicleMap[id] = new Car(id, oid, mk, mdl, plate, capInt);
        bumpCounter(id, nextVehicleId, "VEH-");
    }
}

void CarpoolSystem::loadBikesFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) { std::cout << "  [warn] cannot open " << path << "\n"; return; }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string id, oid, mk, mdl, plate, cap;
        std::getline(ss, id,    ',');
        std::getline(ss, oid,   ',');
        std::getline(ss, mk,    ',');
        std::getline(ss, mdl,   ',');
        std::getline(ss, plate, ',');
        std::getline(ss, cap,   ',');
        int capInt = 1;
        try { capInt = std::stoi(cap); } catch (...) { continue; }
        vehicleMap[id] = new Bike(id, oid, mk, mdl, plate, capInt);
        bumpCounter(id, nextVehicleId, "VEH-");
    }
}

void CarpoolSystem::loadRidesFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) { std::cout << "  [warn] cannot open " << path << "\n"; return; }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string id, drv, veh, dateOrTime, timeOrPick, pickOrDrop, dropOrPref;
        std::string pref, stat, seats, pax;
        std::getline(ss, id,          ',');
        std::getline(ss, drv,         ',');
        std::getline(ss, veh,         ',');
        std::getline(ss, dateOrTime,  ',');
        std::getline(ss, timeOrPick,  ',');

        // '-' means new format (YYYY-MM-DD date field); otherwise old format (HH:MM time field)
        std::string rideDate, rideTime;
        std::string pick, drop;
        if (dateOrTime.find('-') != std::string::npos) {
            rideDate = dateOrTime;
            rideTime = timeOrPick;
            std::getline(ss, pick,  ',');
            std::getline(ss, drop,  ',');
        } else {
            rideDate = nowDate();
            rideTime = dateOrTime;
            pick = timeOrPick;
            std::getline(ss, drop,  ',');
        }

        std::getline(ss, pref,  ',');
        std::getline(ss, stat,  ',');
        std::getline(ss, seats, ',');
        std::getline(ss, pax,   ',');

        GenderPref gp = (pref == "0") ? GenderPref::BOYS_ONLY :
                        (pref == "1") ? GenderPref::GIRLS_ONLY : GenderPref::HYBRID;
        RideStatus rs = (stat == "0") ? RideStatus::OPEN :
                        (stat == "1") ? RideStatus::FULL :
                        (stat == "2") ? RideStatus::COMPLETED : RideStatus::CANCELED;

        int pickInt = 0, dropInt = 0, seatsInt = 0;
        try { pickInt  = std::stoi(pick);  } catch (...) { continue; }
        try { dropInt  = std::stoi(drop);  } catch (...) { continue; }
        try { seatsInt = std::stoi(seats); } catch (...) { continue; }

        Ride ride(id, drv, veh, rideDate, rideTime,
                  intToArea(pickInt), intToArea(dropInt),
                  gp, 0);
        ride.setSeatsLeft(seatsInt);
        ride.setStatus(rs);

        if (!pax.empty()) {
            std::stringstream pss(pax);
            std::string pid;
            while (std::getline(pss, pid, ';'))
                if (!pid.empty()) ride.rawAddPassenger(pid);
        }

        rideMap[id] = ride;
        bumpCounter(id, nextRideId, "RID-");
    }
}

void CarpoolSystem::saveStudentsToFile(const std::string& path) const {
    std::ofstream f(path);
    for (auto& [id, s] : studentMap) {
        f << id << ","
          << s.getName() << ","
          << s.getPhoneNumber() << ","
          << (s.getGender() == Gender::MALE ? "0" : "1") << ","
          << (s.getIsDriver() ? "1" : "0") << ","
          << s.getRidesCompleted() << "\n";
    }
}

void CarpoolSystem::saveRidesToFile(const std::string& path) const {
    std::ofstream f(path);
    for (auto& [id, r] : rideMap) {
        int gpInt = (r.getGenderPref() == GenderPref::BOYS_ONLY)  ? 0 :
                    (r.getGenderPref() == GenderPref::GIRLS_ONLY) ? 1 : 2;
        int stInt = (r.getStatus() == RideStatus::OPEN)      ? 0 :
                    (r.getStatus() == RideStatus::FULL)      ? 1 :
                    (r.getStatus() == RideStatus::COMPLETED) ? 2 : 3;

        f << id << "," << r.getDriverId() << "," << r.getVehicleId() << ","
          << r.getRideDate() << "," << r.getRideTime() << ","
          << areaToInt(r.getPickupArea()) << ","
          << areaToInt(r.getDropOffArea()) << "," << gpInt << ","
          << stInt << "," << r.getSeatsLeft();

        auto& pax = r.getPassengers();
        if (!pax.empty()) {
            f << ",";
            for (int i = 0; i < (int)pax.size(); ++i) {
                if (i) f << ";";
                f << pax[i];
            }
        }
        f << "\n";
    }
}

void CarpoolSystem::saveCarsToFile(const std::string& path) const {
    std::ofstream f(path);
    for (auto& [id, v] : vehicleMap) {
        if (v->getType() != "Car") continue;
        f << id << "," << v->getOwnerId() << "," << v->getMake() << ","
          << v->getModel() << "," << v->getLicensePlate() << "," << v->getCapacity() << "\n";
    }
}

void CarpoolSystem::saveBikesToFile(const std::string& path) const {
    std::ofstream f(path);
    for (auto& [id, v] : vehicleMap) {
        if (v->getType() != "Bike") continue;
        f << id << "," << v->getOwnerId() << "," << v->getMake() << ","
          << v->getModel() << "," << v->getLicensePlate() << "," << v->getCapacity() << "\n";
    }
}

std::string CarpoolSystem::createStudent(const std::string& name, const std::string& phone,
                                          Gender g, bool isDriver) {
    std::string id = "STU-" + std::to_string(nextStudentId++);
    studentMap[id] = Student(id, name, phone, g, isDriver, 0);
    saveStudentsToFile("data/students.csv");
    return id;
}

bool CarpoolSystem::updateStudent(const std::string& id, const std::string& name,
                                   const std::string& phone, Gender g) {
    auto it = studentMap.find(id);
    if (it == studentMap.end()) return false;
    it->second.setName(name);
    it->second.setPhone(phone);
    it->second.setGender(g);
    saveStudentsToFile("data/students.csv");
    return true;
}

bool CarpoolSystem::updateVehicle(const std::string& ownerId, const std::string& mk,
                                   const std::string& mdl, const std::string& plate, int cap) {
    for (auto& [vid, veh] : vehicleMap) {
        if (veh->getOwnerId() == ownerId) {
            veh->setMake(mk);
            veh->setModel(mdl);
            veh->setLicensePlate(plate);
            if (veh->getType() == "Car") veh->setCapacity(cap);
            saveCarsToFile("data/cars.csv");
            saveBikesToFile("data/bikes.csv");
            return true;
        }
    }
    return false;
}

bool CarpoolSystem::deleteStudent(const std::string& id) {
    auto sit = studentMap.find(id);
    if (sit == studentMap.end()) return false;

    bool isDrv = sit->second.getIsDriver();

    if (isDrv) {
        // notify passengers, remove all rides this driver owns
        std::vector<std::string> toRemove;
        for (auto& [rid, ride] : rideMap) {
            if (ride.getDriverId() != id) continue;
            if (ride.getStatus() == RideStatus::OPEN || ride.getStatus() == RideStatus::FULL) {
                for (auto& pid : ride.getPassengers())
                    notifMap[pid].emplace_back(pid,
                        "CANCELED: ride " + rid + " canceled — driver deleted their account.");
            }
            toRemove.push_back(rid);
        }
        for (auto& rid : toRemove) rideMap.erase(rid);
    } else {
        for (auto& [rid, ride] : rideMap)
            ride.removePassenger(id);
    }

    std::vector<std::string> vehToRemove;
    for (auto& [vid, veh] : vehicleMap)
        if (veh->getOwnerId() == id) vehToRemove.push_back(vid);
    for (auto& vid : vehToRemove) { delete vehicleMap[vid]; vehicleMap.erase(vid); }

    studentMap.erase(id);
    notifMap.erase(id);

    saveStudentsToFile("data/students.csv");
    saveCarsToFile("data/cars.csv");
    saveBikesToFile("data/bikes.csv");
    saveRidesToFile("data/rides.csv");
    return true;
}

std::string CarpoolSystem::addCar(const std::string& ownerId, const std::string& make,
                                   const std::string& model, const std::string& plate, int capacity) {
    std::string id = "VEH-" + std::to_string(nextVehicleId++);
    vehicleMap[id] = new Car(id, ownerId, make, model, plate, capacity);
    saveCarsToFile("data/cars.csv");
    return id;
}

std::string CarpoolSystem::addBike(const std::string& ownerId, const std::string& make,
                                    const std::string& model, const std::string& plate, int capacity) {
    std::string id = "VEH-" + std::to_string(nextVehicleId++);
    vehicleMap[id] = new Bike(id, ownerId, make, model, plate, capacity);
    saveBikesToFile("data/bikes.csv");
    return id;
}

std::string CarpoolSystem::createRide(const std::string& driverId, const std::string& vehicleId,
                                       const std::string& rideDate, const std::string& rideTime,
                                       Area pickup, Area dropoff, GenderPref pref) {
    if (studentMap.find(driverId)  == studentMap.end()) return "";
    if (vehicleMap.find(vehicleId) == vehicleMap.end()) return "";
    if (!getDriverActiveRide(driverId).empty())          return "";
    if (pickup == dropoff)                               return "";
    if (!isIBACampus(pickup) && !isIBACampus(dropoff))  return "";
    if (isDateTimePast(rideDate, rideTime))              return "";

    int seats = vehicleMap.at(vehicleId)->getCapacity();
    std::string rid = "RID-" + std::to_string(nextRideId++);
    rideMap[rid] = Ride(rid, driverId, vehicleId, rideDate, rideTime, pickup, dropoff, pref, seats);
    saveRidesToFile("data/rides.csv");
    return rid;
}

bool CarpoolSystem::bookRide(const std::string& rideId, const std::string& studentId) {
    auto rit = rideMap.find(rideId);
    auto sit = studentMap.find(studentId);
    if (rit == rideMap.end() || sit == studentMap.end()) return false;
    if (rit->second.getStatus() != RideStatus::OPEN) return false;

    GenderPref pref = rit->second.getGenderPref();
    Gender     gen  = sit->second.getGender();
    if (pref == GenderPref::BOYS_ONLY  && gen == Gender::FEMALE) return false;
    if (pref == GenderPref::GIRLS_ONLY && gen == Gender::MALE)   return false;

    if (rit->second.getDriverId() == studentId) return false;

    if (!rit->second.addPassenger(studentId)) return false;

    notifMap[studentId].emplace_back(studentId,
        "Ride booked: " + rideId + "  " +
        areaToStr(rit->second.getPickupArea()) + " -> " +
        areaToStr(rit->second.getDropOffArea()) +
        "  on " + rit->second.getRideDate() +
        "  at " + rit->second.getRideTime());

    saveRidesToFile("data/rides.csv");
    return true;
}

bool CarpoolSystem::cancelRide(const std::string& rideId) {
    auto it = rideMap.find(rideId);
    if (it == rideMap.end()) return false;

    auto& ride = it->second;
    if (ride.getStatus() != RideStatus::OPEN && ride.getStatus() != RideStatus::FULL)
        return false;

    if (isDateTimePast(ride.getRideDate(), ride.getRideTime())) return false;

    ride.markCanceled();

    for (auto& pid : ride.getPassengers()) {
        notifMap[pid].emplace_back(pid,
            "CANCELED: ride " + rideId + " (" +
            areaToStr(ride.getPickupArea()) + " -> " +
            areaToStr(ride.getDropOffArea()) + " on " +
            ride.getRideDate() + " at " +
            ride.getRideTime() + ") was canceled by driver.");
    }

    saveRidesToFile("data/rides.csv");
    return true;
}

void CarpoolSystem::checkAutoComplete() {
    bool changed = false;

    for (auto& [id, ride] : rideMap) {
        if (ride.getStatus() != RideStatus::OPEN && ride.getStatus() != RideStatus::FULL)
            continue;
        if (!isDateTimePast(ride.getRideDate(), ride.getRideTime()))
            continue;

        ride.markCompleted();
        changed = true;

        for (auto& pid : ride.getAllParticipants()) {
            auto sit = studentMap.find(pid);
            if (sit != studentMap.end())
                sit->second.incrementRidesCompleted();
        }

        for (auto& pid : ride.getPassengers()) {
            notifMap[pid].emplace_back(pid,
                "Completed: ride " + id + " (" +
                areaToStr(ride.getPickupArea()) + " -> " +
                areaToStr(ride.getDropOffArea()) + ") is done.");
        }
    }

    if (changed) {
        saveRidesToFile("data/rides.csv");
        saveStudentsToFile("data/students.csv");
    }
}

std::string CarpoolSystem::getDriverActiveRide(const std::string& driverId) const {
    for (auto& [id, r] : rideMap) {
        if (r.getDriverId() == driverId &&
            (r.getStatus() == RideStatus::OPEN || r.getStatus() == RideStatus::FULL))
            return id;
    }
    return "";
}

std::string CarpoolSystem::getVehicleForDriver(const std::string& driverId) const {
    for (auto& [id, v] : vehicleMap)
        if (v->getOwnerId() == driverId) return id;
    return "";
}

std::vector<std::string> CarpoolSystem::searchRides(Area pickup, Area dropoff,
                                                     const std::string& requestedTime) const {
    std::vector<std::string> results;
    int reqMins = timeToMins(requestedTime);
    for (auto& [id, r] : rideMap) {
        if (r.getStatus() != RideStatus::OPEN && r.getStatus() != RideStatus::FULL) continue;
        if (r.getPickupArea() != pickup || r.getDropOffArea() != dropoff) continue;
        if (std::abs(timeToMins(r.getRideTime()) - reqMins) <= 30)
            results.push_back(id);
    }
    // closest to requested time; ties broken by Ride::operator<
    std::sort(results.begin(), results.end(), [&](const std::string& a, const std::string& b) {
        const Ride& ra = rideMap.at(a);
        const Ride& rb = rideMap.at(b);
        int da = std::abs(timeToMins(ra.getRideTime()) - reqMins);
        int db = std::abs(timeToMins(rb.getRideTime()) - reqMins);
        if (da != db) return da < db;
        return ra < rb;
    });
    return results;
}

std::vector<std::string> CarpoolSystem::getDriverHistory(const std::string& driverId) const {
    std::vector<std::string> hist;
    for (auto& [id, r] : rideMap) {
        if (r.getDriverId() == driverId &&
            (r.getStatus() == RideStatus::COMPLETED || r.getStatus() == RideStatus::CANCELED))
            hist.push_back(id);
    }
    return hist;
}

const Student* CarpoolSystem::getStudent(const std::string& id) const {
    auto it = studentMap.find(id);
    return (it != studentMap.end()) ? &it->second : nullptr;
}

Ride* CarpoolSystem::getRide(const std::string& id) {
    auto it = rideMap.find(id);
    return (it != rideMap.end()) ? &it->second : nullptr;
}

const Ride* CarpoolSystem::getRide(const std::string& id) const {
    auto it = rideMap.find(id);
    return (it != rideMap.end()) ? &it->second : nullptr;
}

const Vehicle* CarpoolSystem::getVehicle(const std::string& id) const {
    auto it = vehicleMap.find(id);
    return (it != vehicleMap.end()) ? it->second : nullptr;
}

const std::vector<Notification>& CarpoolSystem::getNotifications(const std::string& sid) const {
    auto it = notifMap.find(sid);
    return (it != notifMap.end()) ? it->second : EMPTY_NOTIFS;
}

const std::map<std::string, Ride>&    CarpoolSystem::getRideMap()    const { return rideMap; }
const std::map<std::string, Student>& CarpoolSystem::getStudentMap() const { return studentMap; }

void CarpoolSystem::showAvailableRides() const {
    std::cout << "\n-- available rides --\n";
    bool any = false;
    for (auto& [id, r] : rideMap) {
        if (r.getStatus() == RideStatus::OPEN || r.getStatus() == RideStatus::FULL) {
            r.displayRide(); std::cout << "  ---\n"; any = true;
        }
    }
    if (!any) std::cout << "  none.\n";
}

void CarpoolSystem::showStudents() const {
    std::cout << "\n-- registered students --\n";
    for (auto& [id, s] : studentMap) { s.displayProfile(); std::cout << "  ---\n"; }
}

void CarpoolSystem::showNotifications(const std::string& sid) const {
    auto it = studentMap.find(sid);
    if (it == studentMap.end()) { std::cout << "  student not found.\n"; return; }
    std::cout << "\n-- notifications for " << it->second.getName() << " --\n";
    auto& notifs = getNotifications(sid);
    if (notifs.empty()) { std::cout << "  none.\n"; return; }
    for (auto& n : notifs) n.display();
}

std::map<std::string, int> CarpoolSystem::countSingletons() {
    std::map<std::string, int> freq;
    for (auto& [id, r] : rideMap) {
        if (r.getStatus() != RideStatus::COMPLETED) continue;
        for (auto& pid : r.getAllParticipants()) freq[pid]++;
    }
    return freq;
}

std::string CarpoolSystem::idToName(const std::string& id) {
    auto it = studentMap.find(id);
    return (it != studentMap.end()) ? it->second.getName() : id;
}

// backtracking: builds all k-combinations from candidates, counts each across transactions
void CarpoolSystem::generateItemsets(
        const std::vector<std::vector<std::string>>& transactions,
        std::vector<std::string>& current,
        int startIdx,
        const std::vector<std::string>& candidates,
        int k,
        std::map<std::vector<std::string>, int>& freqMap) {

    if ((int)current.size() == k) {
        int count = 0;
        for (auto& trans : transactions) {
            bool allPresent = true;
            for (auto& item : current) {
                if (std::find(trans.begin(), trans.end(), item) == trans.end()) {
                    allPresent = false;
                    break;
                }
            }
            if (allPresent) count++;
        }
        if (count > 0) freqMap[current] = count;
        return;
    }

    for (int i = startIdx; i < (int)candidates.size(); ++i) {
        current.push_back(candidates[i]);
        generateItemsets(transactions, current, i + 1, candidates, k, freqMap);
        current.pop_back();
    }
}

void CarpoolSystem::runApriori(int minSupport) {
    std::cout << "\n-- apriori frequent traveller analysis --\n"
              << "  min support: " << minSupport << "\n\n";

    std::vector<std::vector<std::string>> transactions;
    for (auto& [id, r] : rideMap) {
        if (r.getStatus() != RideStatus::COMPLETED) continue;
        auto parts = r.getAllParticipants();
        std::sort(parts.begin(), parts.end());
        transactions.push_back(parts);
    }
    int total = (int)transactions.size();
    if (total == 0) { std::cout << "  no completed rides yet.\n"; return; }

    std::cout << "  step 1: frequent individual students\n";
    auto singles = countSingletons();
    std::vector<std::string> freqIds;
    for (auto& [id, cnt] : singles) {
        if (cnt >= minSupport) {
            freqIds.push_back(id);
            std::cout << "    " << idToName(id)
                      << "  count=" << cnt
                      << "  support=" << (int)(100.0 * cnt / total) << "%\n";
        }
    }
    std::sort(freqIds.begin(), freqIds.end());

    std::cout << "\n  step 2: frequent pairs\n";
    std::map<std::vector<std::string>, int> pairMap;
    std::vector<std::string> current;
    generateItemsets(transactions, current, 0, freqIds, 2, pairMap);
    bool foundPair = false;
    for (auto& [itemset, cnt] : pairMap) {
        if (cnt >= minSupport) {
            foundPair = true;
            int supp = (int)(100.0 * cnt / total);
            std::cout << "    { " << idToName(itemset[0]) << " & " << idToName(itemset[1]) << " }"
                      << "  count=" << cnt << "  support=" << supp << "%\n";
            if (supp >= 50)
                std::cout << "    -> suggest placing them in the same car\n";
        }
    }
    if (!foundPair) std::cout << "    none above threshold.\n";

    std::cout << "\n  step 3: frequent triples\n";
    std::map<std::vector<std::string>, int> tripleMap;
    generateItemsets(transactions, current, 0, freqIds, 3, tripleMap);
    bool foundTriple = false;
    for (auto& [itemset, cnt] : tripleMap) {
        if (cnt >= minSupport) {
            foundTriple = true;
            std::cout << "    { " << idToName(itemset[0])
                      << " & " << idToName(itemset[1])
                      << " & " << idToName(itemset[2]) << " }"
                      << "  count=" << cnt
                      << "  support=" << (int)(100.0 * cnt / total) << "%\n";
        }
    }
    if (!foundTriple) std::cout << "    none above threshold.\n";
}
