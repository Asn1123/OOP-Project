#include "Bike.h"
#include <iostream>

Bike::Bike() {}

Bike::Bike(std::string vid, std::string oid, std::string mk,
           std::string mdl, std::string plate, int cap)
    : Vehicle(vid, oid, mk, mdl, plate, cap) {}

std::string Bike::getType() const { return "Bike"; }

void Bike::displayVehicle() const {
    Vehicle::displayVehicle();
}
