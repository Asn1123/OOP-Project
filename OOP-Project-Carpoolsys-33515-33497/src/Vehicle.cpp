#include "Vehicle.h"
#include <iostream>

Vehicle::Vehicle() : capacity(0) {}

Vehicle::Vehicle(std::string vid, std::string oid, std::string mk,
                 std::string mdl, std::string plate, int cap)
    : vehicleId(vid), ownerId(oid), make(mk), model(mdl),
      licensePlate(plate), capacity(cap) {}

std::string Vehicle::getVehicleId()    const { return vehicleId; }
std::string Vehicle::getOwnerId()      const { return ownerId; }
std::string Vehicle::getMake()         const { return make; }
std::string Vehicle::getModel()        const { return model; }
std::string Vehicle::getLicensePlate() const { return licensePlate; }
int         Vehicle::getCapacity()     const { return capacity; }

void Vehicle::setMake        (const std::string& m) { make         = m; }
void Vehicle::setModel       (const std::string& m) { model        = m; }
void Vehicle::setLicensePlate(const std::string& p) { licensePlate = p; }
void Vehicle::setCapacity    (int c)                { capacity     = c; }

void Vehicle::displayVehicle() const {
    std::cout << "  ID    : " << vehicleId << "\n"
              << "  Type  : " << getType() << "\n"
              << "  Make  : " << make << " " << model << "\n"
              << "  Plate : " << licensePlate << "\n"
              << "  Seats : " << capacity << "\n";
}

std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    os << "[" << v.getType() << "] " << v.make << " " << v.model
       << " | Plate: " << v.licensePlate
       << " | Seats: " << v.capacity;
    return os;
}
