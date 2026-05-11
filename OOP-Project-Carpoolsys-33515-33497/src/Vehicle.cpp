#include "Vehicle.h"
#include <iostream>
using namespace std;

Vehicle::Vehicle() : capacity(0) {}

Vehicle::Vehicle(string vid, string oid, string mk,
                 string mdl, string plate, int cap)
    : vehicleId(vid), ownerId(oid), make(mk), model(mdl),
      licensePlate(plate), capacity(cap) {}

string Vehicle::getVehicleId() const { return vehicleId; }
string Vehicle::getOwnerId() const { return ownerId; }
string Vehicle::getMake() const { return make; }
string Vehicle::getModel() const { return model; }
string Vehicle::getLicensePlate() const { return licensePlate; }
int Vehicle::getCapacity() const { return capacity; }

void Vehicle::setMake(const string& m) { make = m; }
void Vehicle::setModel(const string& m) { model = m; }
void Vehicle::setLicensePlate(const string& p) { licensePlate = p; }
void Vehicle::setCapacity(int c) { capacity = c; }

void Vehicle::displayVehicle() const {
    cout << "  ID    : " << vehicleId << "\n"
         << "  Type  : " << getType() << "\n"
         << "  Make  : " << make << " " << model << "\n"
         << "  Plate : " << licensePlate << "\n"
         << "  Seats : " << capacity << "\n";
}

ostream& operator<<(ostream& os, const Vehicle& v) {
    os << "[" << v.getType() << "] " << v.make << " " << v.model
       << " | Plate: " << v.licensePlate
       << " | Seats: " << v.capacity;
    return os;
}
