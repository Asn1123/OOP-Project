#include "Car.h"
#include <iostream>

Car::Car() {}

Car::Car(std::string vid, std::string oid, std::string mk,
         std::string mdl, std::string plate, int cap)
    : Vehicle(vid, oid, mk, mdl, plate, cap) {}

std::string Car::getType() const { return "Car"; }

void Car::displayVehicle() const {
    Vehicle::displayVehicle();
}
