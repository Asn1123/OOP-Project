#include "Car.h"
#include <iostream>
using namespace std;

Car::Car() {}

Car::Car(string vid, string oid, string mk,
         string mdl, string plate, int cap)
    : Vehicle(vid, oid, mk, mdl, plate, cap) {}

string Car::getType() const { return "Car"; }

void Car::displayVehicle() const {
    Vehicle::displayVehicle();
}
