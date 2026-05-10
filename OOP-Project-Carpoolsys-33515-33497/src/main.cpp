#include "CarpoolSystem.h"
#include <iostream>
#include <limits>

#ifdef USE_GUI
#include "gui.h"
#endif

// ---- console fallback ----

static void consolePrintMenu(bool isDriver) {
    std::cout << "\n============================\n"
              << "  IBA Carpool — "
              << (isDriver ? "Driver" : "Passenger") << " Menu\n"
              << "============================\n";
    if (isDriver) {
        std::cout << "  1. Create Ride\n"
                  << "  2. View Current Ride\n"
                  << "  3. Ride History\n"
                  << "  4. Cancel Active Ride\n"
                  << "  5. Run Apriori\n";
    } else {
        std::cout << "  1. Search Rides\n"
                  << "  2. Book a Ride\n"
                  << "  3. My Notifications\n";
    }
    std::cout << "  0. Logout\n"
              << "============================\n"
              << "  Choice: ";
}

static void runConsole(CarpoolSystem& sys) {
    std::cout << "\n=== IBA Carpool Facilitation System ===\n";

    while (true) {
        // role selection
        std::cout << "\nAre you a (1) Driver or (2) Passenger? (0 to exit): ";
        int role; std::cin >> role; std::cin.ignore();
        if (role == 0) break;
        if (role != 1 && role != 2) { std::cout << "  invalid choice.\n"; continue; }
        bool isDriver = (role == 1);

        // login
        std::cout << "  Student ID: ";
        std::string uid; std::getline(std::cin, uid);
        auto* stu = sys.getStudent(uid);
        if (!stu) { std::cout << "  student not found.\n"; continue; }
        if (isDriver && !stu->getIsDriver()) { std::cout << "  invalid credentials.\n"; continue; }
        if (!isDriver && stu->getIsDriver()) { std::cout << "  invalid credentials.\n"; continue; }

        std::cout << "  Welcome, " << stu->getName() << "!\n";
        sys.checkAutoComplete();

        int choice;
        do {
            consolePrintMenu(isDriver);
            std::cin >> choice; std::cin.ignore();
            std::cout << "\n";

            if (isDriver) {
                switch (choice) {
                case 1: {
                    // create ride
                    std::string activeId = sys.getDriverActiveRide(uid);
                    if (!activeId.empty()) {
                        std::cout << "  you already have an active ride: " << activeId << "\n";
                        break;
                    }
                    std::string vehId = sys.getVehicleForDriver(uid);
                    if (vehId.empty()) { std::cout << "  no vehicle registered.\n"; break; }

                    std::cout << "  Pickup area (0-" << AREA_COUNT-1 << ", 11=IBA City, 12=IBA Main):\n";
                    for (int i = 0; i < AREA_COUNT; ++i)
                        std::cout << "    " << i << ". " << areaToStr(intToArea(i)) << "\n";
                    int pu, dr; std::cin >> pu; std::cin.ignore();
                    std::cout << "  Drop-off area: "; std::cin >> dr; std::cin.ignore();
                    std::cout << "  Ride date offset (0=today, 1=tomorrow, 2=day after): ";
                    int doff; std::cin >> doff; std::cin.ignore();
                    std::string t;
                    std::cout << "  Ride time (HH:MM): "; std::getline(std::cin, t);
                    std::cout << "  Gender pref (0=Boys,1=Girls,2=Hybrid): ";
                    int pref; std::cin >> pref; std::cin.ignore();
                    GenderPref gp = (pref==0) ? GenderPref::BOYS_ONLY :
                                    (pref==1) ? GenderPref::GIRLS_ONLY : GenderPref::HYBRID;
                    std::string rDate = dateOffset(doff < 0 ? 0 : doff > 2 ? 2 : doff);
                    std::string rid = sys.createRide(uid, vehId, rDate, t, intToArea(pu), intToArea(dr), gp);
                    if (rid.empty()) std::cout << "  failed to create ride (check campus, same area, or past time).\n";
                    else             std::cout << "  ride created: " << rid << "\n";
                    break;
                }
                case 2: {
                    // current ride
                    std::string activeId = sys.getDriverActiveRide(uid);
                    if (activeId.empty()) { std::cout << "  no active ride.\n"; break; }
                    auto* r = sys.getRide(activeId);
                    r->displayRide();
                    std::cout << "  passengers:\n";
                    for (auto& pid : r->getPassengers()) {
                        auto* ps = sys.getStudent(pid);
                        if (ps)
                            std::cout << "    " << ps->getName() << "  " << ps->getPhoneNumber() << "\n";
                    }
                    break;
                }
                case 3: {
                    // history
                    auto hist = sys.getDriverHistory(uid);
                    if (hist.empty()) { std::cout << "  no history.\n"; break; }
                    for (auto& id : hist) {
                        auto* r = sys.getRide(id);
                        if (r) r->displayRide();
                        std::cout << "  ---\n";
                    }
                    break;
                }
                case 4: {
                    // cancel
                    std::string activeId = sys.getDriverActiveRide(uid);
                    if (activeId.empty()) { std::cout << "  no active ride.\n"; break; }
                    if (sys.cancelRide(activeId))
                        std::cout << "  ride canceled.\n";
                    else
                        std::cout << "  cannot cancel (ride time already passed).\n";
                    break;
                }
                case 5: {
                    int ms; std::cout << "  min support: "; std::cin >> ms; std::cin.ignore();
                    sys.runApriori(ms);
                    break;
                }
                default: break;
                }
            } else {
                // passenger
                switch (choice) {
                case 1: {
                    // search rides
                    std::cout << "  Pickup area (11=IBA City, 12=IBA Main):\n";
                    for (int i = 0; i < AREA_COUNT; ++i)
                        std::cout << "    " << i << ". " << areaToStr(intToArea(i)) << "\n";
                    int pu, dr; std::cin >> pu; std::cin.ignore();
                    std::cout << "  Drop-off area: "; std::cin >> dr; std::cin.ignore();
                    std::string t;
                    std::cout << "  Desired time (HH:MM): "; std::getline(std::cin, t);
                    auto results = sys.searchRides(intToArea(pu), intToArea(dr), t);
                    if (results.empty()) { std::cout << "  no rides found.\n"; break; }
                    for (auto& rid : results) {
                        auto* r = sys.getRide(rid);
                        if (r) r->displayRide();
                        std::cout << "  ---\n";
                    }
                    break;
                }
                case 2: {
                    // book
                    std::string rid;
                    std::cout << "  Ride ID to book: "; std::getline(std::cin, rid);
                    if (sys.bookRide(rid, uid))
                        std::cout << "  booked successfully.\n";
                    else
                        std::cout << "  booking failed (gender pref, no seats, or not OPEN).\n";
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
    sys.loadCarsFromFile    ("data/cars.csv");
    sys.loadBikesFromFile   ("data/bikes.csv");
    sys.loadRidesFromFile   ("data/rides.csv");

#ifdef USE_GUI
    runGUI(sys);
#else
    runConsole(sys);
#endif

    return 0;
}
