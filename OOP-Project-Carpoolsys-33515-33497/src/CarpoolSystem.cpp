#include "CarpoolSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
using namespace std;

static const vector<Notification> NO_NOTIFS;

CarpoolSystem::~CarpoolSystem() {
    for (auto& [id, ptr] : vehicles) delete ptr;
}

void CarpoolSystem::bumpId(const string& id, int& counter, const string& prefix) {
    if (id.rfind(prefix, 0) == 0) {
        int n = stoi(id.substr(prefix.size()));
        if (n >= counter) counter = n + 1;
    }
}

void CarpoolSystem::loadStudentsFromFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) { cout << "  [warn] cannot open " << path << "\n"; return; }
    string line;
    while (getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        vector<string> fields;
        stringstream ss(line);
        string tok;
        while (getline(ss, tok, ',')) fields.push_back(tok);
        if (fields.size() < 6) continue;

        string id = fields[0];
        string name = fields[1];
        string phone = fields[2];
        string gStr = fields[3];

        // 7-field old format had homeArea at index 4
        string dStr = (fields.size() >= 7) ? fields[5] : fields[4];
        string rStr = (fields.size() >= 7) ? (fields.size() > 6 ? fields[6] : "0")
                                           : fields[5];

        Gender g = (gStr == "0") ? Gender::MALE : Gender::FEMALE;
        bool drv = (dStr == "1");
        int done = 0;
        try { done = rStr.empty() ? 0 : stoi(rStr); } catch (...) {}
        students[id] = Student(id, name, phone, g, drv, done);
        bumpId(id, nextStu, "STU-");
    }
}

void CarpoolSystem::loadCarsFromFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) { cout << "  [warn] cannot open " << path << "\n"; return; }
    string line;
    while (getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        stringstream ss(line);
        string id, oid, mk, mdl, plate, cap;
        getline(ss, id, ',');
        getline(ss, oid, ',');
        getline(ss, mk, ',');
        getline(ss, mdl, ',');
        getline(ss, plate, ',');
        getline(ss, cap, ',');
        int capInt = 1;
        try { capInt = stoi(cap); } catch (...) { continue; }
        vehicles[id] = new Car(id, oid, mk, mdl, plate, capInt);
        bumpId(id, nextVeh, "VEH-");
    }
}

void CarpoolSystem::loadBikesFromFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) { cout << "  [warn] cannot open " << path << "\n"; return; }
    string line;
    while (getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        stringstream ss(line);
        string id, oid, mk, mdl, plate, cap;
        getline(ss, id, ',');
        getline(ss, oid, ',');
        getline(ss, mk, ',');
        getline(ss, mdl, ',');
        getline(ss, plate, ',');
        getline(ss, cap, ',');
        int capInt = 1;
        try { capInt = stoi(cap); } catch (...) { continue; }
        vehicles[id] = new Bike(id, oid, mk, mdl, plate, capInt);
        bumpId(id, nextVeh, "VEH-");
    }
}

void CarpoolSystem::loadRidesFromFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) { cout << "  [warn] cannot open " << path << "\n"; return; }
    string line;
    while (getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        stringstream ss(line);
        string id, drv, veh, dateOrTime, timeOrPick;
        string gpStr, stStr, seatsStr, paxStr;
        getline(ss, id, ',');
        getline(ss, drv, ',');
        getline(ss, veh, ',');
        getline(ss, dateOrTime, ',');
        getline(ss, timeOrPick, ',');

        // '-' in field 4 means new format (has YYYY-MM-DD date)
        string rideDate, rideTime;
        string pick, drop;
        if (dateOrTime.find('-') != string::npos) {
            rideDate = dateOrTime;
            rideTime = timeOrPick;
            getline(ss, pick, ',');
            getline(ss, drop, ',');
        } else {
            rideDate = nowDate();
            rideTime = dateOrTime;
            pick = timeOrPick;
            getline(ss, drop, ',');
        }

        getline(ss, gpStr, ',');
        getline(ss, stStr, ',');
        getline(ss, seatsStr, ',');
        getline(ss, paxStr, ',');

        GenderPref gp = (gpStr == "0") ? GenderPref::BOYS_ONLY :
                        (gpStr == "1") ? GenderPref::GIRLS_ONLY : GenderPref::HYBRID;
        RideStatus rs = (stStr == "0") ? RideStatus::OPEN :
                        (stStr == "1") ? RideStatus::FULL :
                        (stStr == "2") ? RideStatus::COMPLETED : RideStatus::CANCELED;

        int pickInt = 0, dropInt = 0, seatsInt = 0;
        try { pickInt  = stoi(pick);     } catch (...) { continue; }
        try { dropInt  = stoi(drop);     } catch (...) { continue; }
        try { seatsInt = stoi(seatsStr); } catch (...) { continue; }

        Ride ride(id, drv, veh, rideDate, rideTime,
                  intToArea(pickInt), intToArea(dropInt), gp, 0);
        ride.setSeats(seatsInt);
        ride.setStatus(rs);

        if (!paxStr.empty()) {
            stringstream pss(paxStr);
            string pid;
            while (getline(pss, pid, ';'))
                if (!pid.empty()) ride.rawAddPassenger(pid);
        }

        rides[id] = ride;
        bumpId(id, nextRid, "RID-");
    }
}

void CarpoolSystem::saveStudentsToFile(const string& path) const {
    ofstream f(path);
    for (auto& [id, s] : students) {
        f << id << ","
          << s.getName() << ","
          << s.getPhoneNumber() << ","
          << (s.getGender() == Gender::MALE ? "0" : "1") << ","
          << (s.getIsDriver() ? "1" : "0") << ","
          << s.getRidesDone() << "\n";
    }
}

void CarpoolSystem::saveRidesToFile(const string& path) const {
    ofstream f(path);
    for (auto& [id, r] : rides) {
        int gpInt = (r.getPref() == GenderPref::BOYS_ONLY) ? 0 :
                    (r.getPref() == GenderPref::GIRLS_ONLY) ? 1 : 2;
        int stInt = (r.getStatus() == RideStatus::OPEN)      ? 0 :
                    (r.getStatus() == RideStatus::FULL)      ? 1 :
                    (r.getStatus() == RideStatus::COMPLETED) ? 2 : 3;

        f << id << "," << r.getDriverId() << "," << r.getVehicleId() << ","
          << r.getRideDate() << "," << r.getRideTime() << ","
          << areaToInt(r.getPickup()) << ","
          << areaToInt(r.getDropoff()) << "," << gpInt << ","
          << stInt << "," << r.getSeats();

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

void CarpoolSystem::saveCarsToFile(const string& path) const {
    ofstream f(path);
    for (auto& [id, v] : vehicles) {
        if (v->getType() != "Car") continue;
        f << id << "," << v->getOwnerId() << "," << v->getMake() << ","
          << v->getModel() << "," << v->getLicensePlate() << "," << v->getCapacity() << "\n";
    }
}

void CarpoolSystem::saveBikesToFile(const string& path) const {
    ofstream f(path);
    for (auto& [id, v] : vehicles) {
        if (v->getType() != "Bike") continue;
        f << id << "," << v->getOwnerId() << "," << v->getMake() << ","
          << v->getModel() << "," << v->getLicensePlate() << "," << v->getCapacity() << "\n";
    }
}

string CarpoolSystem::createStudent(const string& name, const string& phone,
                                     Gender g, bool isDriver) {
    string id = "STU-" + to_string(nextStu++);
    students[id] = Student(id, name, phone, g, isDriver, 0);
    saveStudentsToFile("data/students.csv");
    return id;
}

bool CarpoolSystem::updateStudent(const string& id, const string& name,
                                   const string& phone, Gender g) {
    auto it = students.find(id);
    if (it == students.end()) return false;
    it->second.setName(name);
    it->second.setPhone(phone);
    it->second.setGender(g);
    saveStudentsToFile("data/students.csv");
    return true;
}

bool CarpoolSystem::updateVehicle(const string& ownerId, const string& mk,
                                   const string& mdl, const string& plate, int cap) {
    for (auto& [vid, veh] : vehicles) {
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

bool CarpoolSystem::deleteStudent(const string& id) {
    auto sit = students.find(id);
    if (sit == students.end()) return false;

    bool isDrv = sit->second.getIsDriver();

    if (isDrv) {
        // notify passengers then wipe their rides
        vector<string> toRemove;
        for (auto& [rid, ride] : rides) {
            if (ride.getDriverId() != id) continue;
            if (ride.getStatus() == RideStatus::OPEN || ride.getStatus() == RideStatus::FULL) {
                for (auto& pid : ride.getPassengers())
                    notifs[pid].emplace_back(pid,
                        "CANCELED: ride " + rid + " canceled — driver deleted their account.");
            }
            toRemove.push_back(rid);
        }
        for (auto& rid : toRemove) rides.erase(rid);
    } else {
        for (auto& [rid, ride] : rides)
            ride.removePassenger(id);
    }

    vector<string> vehToRemove;
    for (auto& [vid, veh] : vehicles)
        if (veh->getOwnerId() == id) vehToRemove.push_back(vid);
    for (auto& vid : vehToRemove) { delete vehicles[vid]; vehicles.erase(vid); }

    students.erase(id);
    notifs.erase(id);

    saveStudentsToFile("data/students.csv");
    saveCarsToFile("data/cars.csv");
    saveBikesToFile("data/bikes.csv");
    saveRidesToFile("data/rides.csv");
    return true;
}

string CarpoolSystem::addCar(const string& ownerId, const string& make,
                              const string& model, const string& plate, int cap) {
    string id = "VEH-" + to_string(nextVeh++);
    vehicles[id] = new Car(id, ownerId, make, model, plate, cap);
    saveCarsToFile("data/cars.csv");
    return id;
}

string CarpoolSystem::addBike(const string& ownerId, const string& make,
                               const string& model, const string& plate, int cap) {
    string id = "VEH-" + to_string(nextVeh++);
    vehicles[id] = new Bike(id, ownerId, make, model, plate, cap);
    saveBikesToFile("data/bikes.csv");
    return id;
}

string CarpoolSystem::createRide(const string& driverId, const string& vehicleId,
                                  const string& rideDate, const string& rideTime,
                                  Area pickup, Area dropoff, GenderPref pref) {
    if (students.find(driverId)  == students.end()) return "";
    if (vehicles.find(vehicleId) == vehicles.end()) return "";
    if (!activeRide(driverId).empty())               return "";
    if (pickup == dropoff)                           return "";
    if (!isCampus(pickup) && !isCampus(dropoff))     return "";
    if (isPast(rideDate, rideTime))                  return "";

    int cap = vehicles.at(vehicleId)->getCapacity();
    string rid = "RID-" + to_string(nextRid++);
    rides[rid] = Ride(rid, driverId, vehicleId, rideDate, rideTime, pickup, dropoff, pref, cap);
    saveRidesToFile("data/rides.csv");
    return rid;
}

bool CarpoolSystem::bookRide(const string& rideId, const string& stuId) {
    auto rit = rides.find(rideId);
    auto sit = students.find(stuId);
    if (rit == rides.end() || sit == students.end()) return false;
    if (rit->second.getStatus() != RideStatus::OPEN) return false;

    GenderPref rPref = rit->second.getPref();
    Gender     gen   = sit->second.getGender();
    if (rPref == GenderPref::BOYS_ONLY  && gen == Gender::FEMALE) return false;
    if (rPref == GenderPref::GIRLS_ONLY && gen == Gender::MALE)   return false;

    if (rit->second.getDriverId() == stuId) return false;

    if (!rit->second.addPassenger(stuId)) return false;

    notifs[stuId].emplace_back(stuId,
        "Ride booked: " + rideId + "  " +
        areaToStr(rit->second.getPickup()) + " -> " +
        areaToStr(rit->second.getDropoff()) +
        "  on " + rit->second.getRideDate() +
        "  at " + rit->second.getRideTime());

    saveRidesToFile("data/rides.csv");
    return true;
}

bool CarpoolSystem::cancelRide(const string& rideId) {
    auto it = rides.find(rideId);
    if (it == rides.end()) return false;

    auto& ride = it->second;
    if (ride.getStatus() != RideStatus::OPEN && ride.getStatus() != RideStatus::FULL)
        return false;

    if (isPast(ride.getRideDate(), ride.getRideTime())) return false;

    ride.markCanceled();

    for (auto& pid : ride.getPassengers()) {
        notifs[pid].emplace_back(pid,
            "CANCELED: ride " + rideId + " (" +
            areaToStr(ride.getPickup()) + " -> " +
            areaToStr(ride.getDropoff()) + " on " +
            ride.getRideDate() + " at " +
            ride.getRideTime() + ") was canceled by driver.");
    }

    saveRidesToFile("data/rides.csv");
    return true;
}

void CarpoolSystem::checkAutoComplete() {
    bool changed = false;

    for (auto& [id, ride] : rides) {
        if (ride.getStatus() != RideStatus::OPEN && ride.getStatus() != RideStatus::FULL)
            continue;
        if (!isPast(ride.getRideDate(), ride.getRideTime()))
            continue;

        ride.markCompleted();
        changed = true;

        for (auto& pid : ride.everyone()) {
            auto sit = students.find(pid);
            if (sit != students.end()) sit->second.addRide();
        }

        for (auto& pid : ride.getPassengers()) {
            notifs[pid].emplace_back(pid,
                "Completed: ride " + id + " (" +
                areaToStr(ride.getPickup()) + " -> " +
                areaToStr(ride.getDropoff()) + ") is done.");
        }
    }

    if (changed) {
        saveRidesToFile("data/rides.csv");
        saveStudentsToFile("data/students.csv");
    }
}

string CarpoolSystem::activeRide(const string& driverId) const {
    for (auto& [id, r] : rides) {
        if (r.getDriverId() == driverId &&
            (r.getStatus() == RideStatus::OPEN || r.getStatus() == RideStatus::FULL))
            return id;
    }
    return "";
}

string CarpoolSystem::vehicleOf(const string& driverId) const {
    for (auto& [id, v] : vehicles)
        if (v->getOwnerId() == driverId) return id;
    return "";
}

vector<string> CarpoolSystem::searchRides(Area pickup, Area dropoff,
                                           const string& wantTime) const {
    vector<string> results;
    int want = toMins(wantTime);
    for (auto& [id, r] : rides) {
        if (r.getStatus() != RideStatus::OPEN && r.getStatus() != RideStatus::FULL) continue;
        if (r.getPickup() != pickup || r.getDropoff() != dropoff) continue;
        if (abs(toMins(r.getRideTime()) - want) <= 30)
            results.push_back(id);
    }
    // sort by proximity to wantTime; ties go chronological
    sort(results.begin(), results.end(), [&](const string& a, const string& b) {
        const Ride& ra = rides.at(a);
        const Ride& rb = rides.at(b);
        int da = abs(toMins(ra.getRideTime()) - want);
        int db = abs(toMins(rb.getRideTime()) - want);
        if (da != db) return da < db;
        return ra < rb;
    });
    return results;
}

vector<string> CarpoolSystem::driverHistory(const string& driverId) const {
    vector<string> hist;
    for (auto& [id, r] : rides) {
        if (r.getDriverId() == driverId &&
            (r.getStatus() == RideStatus::COMPLETED || r.getStatus() == RideStatus::CANCELED))
            hist.push_back(id);
    }
    return hist;
}

const Student* CarpoolSystem::getStudent(const string& id) const {
    auto it = students.find(id);
    return (it != students.end()) ? &it->second : nullptr;
}

Ride* CarpoolSystem::getRide(const string& id) {
    auto it = rides.find(id);
    return (it != rides.end()) ? &it->second : nullptr;
}

const Ride* CarpoolSystem::getRide(const string& id) const {
    auto it = rides.find(id);
    return (it != rides.end()) ? &it->second : nullptr;
}

const Vehicle* CarpoolSystem::getVehicle(const string& id) const {
    auto it = vehicles.find(id);
    return (it != vehicles.end()) ? it->second : nullptr;
}

const vector<Notification>& CarpoolSystem::getNotifications(const string& stuId) const {
    auto it = notifs.find(stuId);
    return (it != notifs.end()) ? it->second : NO_NOTIFS;
}

const map<string, Ride>&    CarpoolSystem::getRides()    const { return rides; }
const map<string, Student>& CarpoolSystem::getStudents() const { return students; }

void CarpoolSystem::showAvailableRides() const {
    cout << "\n-- available rides --\n";
    bool any = false;
    for (auto& [id, r] : rides) {
        if (r.getStatus() == RideStatus::OPEN || r.getStatus() == RideStatus::FULL) {
            r.displayRide(); cout << "  ---\n"; any = true;
        }
    }
    if (!any) cout << "  none.\n";
}

void CarpoolSystem::showStudents() const {
    cout << "\n-- registered students --\n";
    for (auto& [id, s] : students) { s.displayProfile(); cout << "  ---\n"; }
}

void CarpoolSystem::showNotifications(const string& stuId) const {
    auto it = students.find(stuId);
    if (it == students.end()) { cout << "  student not found.\n"; return; }
    cout << "\n-- notifications for " << it->second.getName() << " --\n";
    auto& ns = getNotifications(stuId);
    if (ns.empty()) { cout << "  none.\n"; return; }
    for (auto& n : ns) n.display();
}

map<string, int> CarpoolSystem::countSingles() {
    map<string, int> freq;
    for (auto& [id, r] : rides) {
        if (r.getStatus() != RideStatus::COMPLETED) continue;
        for (auto& pid : r.everyone()) freq[pid]++;
    }
    return freq;
}

string CarpoolSystem::idToName(const string& id) {
    auto it = students.find(id);
    return (it != students.end()) ? it->second.getName() : id;
}

// count how often each k-combo appears across transactions
void CarpoolSystem::genItemsets(
        const vector<vector<string>>& txns,
        vector<string>& cur,
        int start,
        const vector<string>& cands,
        int k,
        map<vector<string>, int>& freq) {

    if ((int)cur.size() == k) {
        int count = 0;
        for (auto& tx : txns) {
            bool ok = true;
            for (auto& item : cur) {
                if (find(tx.begin(), tx.end(), item) == tx.end()) { ok = false; break; }
            }
            if (ok) count++;
        }
        if (count > 0) freq[cur] = count;
        return;
    }

    for (int i = start; i < (int)cands.size(); ++i) {
        cur.push_back(cands[i]);
        genItemsets(txns, cur, i + 1, cands, k, freq);
        cur.pop_back();
    }
}

void CarpoolSystem::runApriori(int minSupport) {
    cout << "\n-- apriori frequent traveller analysis --\n"
         << "  min support: " << minSupport << "\n\n";

    vector<vector<string>> txns;
    for (auto& [id, r] : rides) {
        if (r.getStatus() != RideStatus::COMPLETED) continue;
        auto parts = r.everyone();
        sort(parts.begin(), parts.end());
        txns.push_back(parts);
    }
    int total = (int)txns.size();
    if (total == 0) { cout << "  no completed rides yet.\n"; return; }

    cout << "  step 1: frequent individual students\n";
    auto singles = countSingles();
    vector<string> freqIds;
    for (auto& [id, cnt] : singles) {
        if (cnt >= minSupport) {
            freqIds.push_back(id);
            cout << "    " << idToName(id)
                 << "  count=" << cnt
                 << "  support=" << (int)(100.0 * cnt / total) << "%\n";
        }
    }
    sort(freqIds.begin(), freqIds.end());

    cout << "\n  step 2: frequent pairs\n";
    map<vector<string>, int> pairs;
    vector<string> cur;
    genItemsets(txns, cur, 0, freqIds, 2, pairs);
    bool foundPair = false;
    for (auto& [set, cnt] : pairs) {
        if (cnt >= minSupport) {
            foundPair = true;
            int supp = (int)(100.0 * cnt / total);
            cout << "    { " << idToName(set[0]) << " & " << idToName(set[1]) << " }"
                 << "  count=" << cnt << "  support=" << supp << "%\n";
            if (supp >= 50)
                cout << "    -> suggest placing them in the same car\n";
        }
    }
    if (!foundPair) cout << "    none above threshold.\n";

    cout << "\n  step 3: frequent triples\n";
    map<vector<string>, int> triples;
    genItemsets(txns, cur, 0, freqIds, 3, triples);
    bool foundTriple = false;
    for (auto& [set, cnt] : triples) {
        if (cnt >= minSupport) {
            foundTriple = true;
            cout << "    { " << idToName(set[0])
                 << " & " << idToName(set[1])
                 << " & " << idToName(set[2]) << " }"
                 << "  count=" << cnt
                 << "  support=" << (int)(100.0 * cnt / total) << "%\n";
        }
    }
    if (!foundTriple) cout << "    none above threshold.\n";
}
