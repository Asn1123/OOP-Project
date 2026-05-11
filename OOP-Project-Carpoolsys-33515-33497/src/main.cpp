#include "CarpoolSystem.h"
#include <iostream>
#include <limits>
using namespace std;

#ifdef USE_GUI
#include "gui.h"
#endif

// console mode

static void printMenu(bool isDriver) {
    cout << "\n============================\n"
         << "  IBA Carpool — "
         << (isDriver ? "Driver" : "Passenger") << " Menu\n"
         << "============================\n";
    if (isDriver) {
        cout << "  1. Create Ride\n"
             << "  2. View Current Ride\n"
             << "  3. Ride History\n"
             << "  4. Cancel Active Ride\n"
             << "  5. Run Apriori\n";
    } else {
        cout << "  1. Search Rides\n"
             << "  2. Book a Ride\n"
             << "  3. My Notifications\n";
    }
    cout << "  0. Logout\n"
         << "============================\n"
         << "  Choice: ";
}

static void runConsole(CarpoolSystem& sys) {
    cout << "\n=== IBA Carpool Facilitation System ===\n";

    while (true) {
        cout << "\nAre you a (1) Driver or (2) Passenger? (0 to exit): ";
        int role; cin >> role; cin.ignore();
        if (role == 0) break;
        if (role != 1 && role != 2) { cout << "  invalid choice.\n"; continue; }
        bool isDriver = (role == 1);

        cout << "  Student ID: ";
        string uid; getline(cin, uid);
        auto* stu = sys.getStudent(uid);
        if (!stu) { cout << "  student not found.\n"; continue; }
        if (isDriver && !stu->getIsDriver()) { cout << "  invalid credentials.\n"; continue; }
        if (!isDriver && stu->getIsDriver()) { cout << "  invalid credentials.\n"; continue; }

        cout << "  Welcome, " << stu->getName() << "!\n";
        sys.checkAutoComplete();

        int choice;
        do {
            printMenu(isDriver);
            cin >> choice; cin.ignore();
            cout << "\n";

            if (isDriver) {
                switch (choice) {
                case 1: {
                    string aid = sys.activeRide(uid);
                    if (!aid.empty()) {
                        cout << "  you already have an active ride: " << aid << "\n";
                        break;
                    }
                    string vehId = sys.vehicleOf(uid);
                    if (vehId.empty()) { cout << "  no vehicle registered.\n"; break; }

                    cout << "  Pickup area (0-" << AREA_COUNT-1 << ", 10=IBA City, 11=IBA Main):\n";
                    for (int i = 0; i < AREA_COUNT; ++i)
                        cout << "    " << i << ". " << areaToStr(intToArea(i)) << "\n";
                    int pu, dr; cin >> pu; cin.ignore();
                    cout << "  Drop-off area: "; cin >> dr; cin.ignore();
                    cout << "  Date offset (0=today, 1=tomorrow, 2=day after): ";
                    int doff; cin >> doff; cin.ignore();
                    string t;
                    cout << "  Time (HH:MM): "; getline(cin, t);
                    cout << "  Gender pref (0=Boys,1=Girls,2=Hybrid): ";
                    int pref; cin >> pref; cin.ignore();
                    GenderPref gp = (pref==0) ? GenderPref::BOYS_ONLY :
                                    (pref==1) ? GenderPref::GIRLS_ONLY : GenderPref::HYBRID;
                    string rDate = dayOffset(doff < 0 ? 0 : doff > 2 ? 2 : doff);
                    string rid = sys.createRide(uid, vehId, rDate, t, intToArea(pu), intToArea(dr), gp);
                    if (rid.empty()) cout << "  failed to create ride (check campus, same area, or past time).\n";
                    else             cout << "  ride created: " << rid << "\n";
                    break;
                }
                case 2: {
                    string aid = sys.activeRide(uid);
                    if (aid.empty()) { cout << "  no active ride.\n"; break; }
                    auto* r = sys.getRide(aid);
                    r->displayRide();
                    cout << "  passengers:\n";
                    for (auto& pid : r->getPassengers()) {
                        auto* ps = sys.getStudent(pid);
                        if (ps) cout << "    " << ps->getName() << "  " << ps->getPhoneNumber() << "\n";
                    }
                    break;
                }
                case 3: {
                    auto hist = sys.driverHistory(uid);
                    if (hist.empty()) { cout << "  no history.\n"; break; }
                    for (auto& id : hist) {
                        auto* r = sys.getRide(id);
                        if (r) r->displayRide();
                        cout << "  ---\n";
                    }
                    break;
                }
                case 4: {
                    string aid = sys.activeRide(uid);
                    if (aid.empty()) { cout << "  no active ride.\n"; break; }
                    if (sys.cancelRide(aid)) cout << "  ride canceled.\n";
                    else                     cout << "  cannot cancel (ride time already passed).\n";
                    break;
                }
                case 5: {
                    int ms; cout << "  min support: "; cin >> ms; cin.ignore();
                    sys.runApriori(ms);
                    break;
                }
                default: break;
                }
            } else {
                switch (choice) {
                case 1: {
                    cout << "  Pickup area (10=IBA City, 11=IBA Main):\n";
                    for (int i = 0; i < AREA_COUNT; ++i)
                        cout << "    " << i << ". " << areaToStr(intToArea(i)) << "\n";
                    int pu, dr; cin >> pu; cin.ignore();
                    cout << "  Drop-off area: "; cin >> dr; cin.ignore();
                    string t;
                    cout << "  Desired time (HH:MM): "; getline(cin, t);
                    auto results = sys.searchRides(intToArea(pu), intToArea(dr), t);
                    if (results.empty()) { cout << "  no rides found.\n"; break; }
                    for (auto& rid : results) {
                        auto* r = sys.getRide(rid);
                        if (r) r->displayRide();
                        cout << "  ---\n";
                    }
                    break;
                }
                case 2: {
                    string rid;
                    cout << "  Ride ID to book: "; getline(cin, rid);
                    if (sys.bookRide(rid, uid)) cout << "  booked successfully.\n";
                    else                        cout << "  booking failed (gender pref, no seats, or not OPEN).\n";
                    break;
                }
                case 3:
                    sys.showNotifications(uid);
                    break;
                default: break;
                }
            }

        } while (choice != 0);
    }
}

int main() {
    CarpoolSystem sys;
    sys.loadStudentsFromFile("data/students.csv");
    sys.loadCarsFromFile("data/cars.csv");
    sys.loadBikesFromFile("data/bikes.csv");
    sys.loadRidesFromFile("data/rides.csv");

#ifdef USE_GUI
    runGUI(sys);
#else
    runConsole(sys);
#endif

    return 0;
}
