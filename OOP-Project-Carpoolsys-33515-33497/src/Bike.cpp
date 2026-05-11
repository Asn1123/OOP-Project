#include "Bike.h"
#include <iostream>
using namespace std;

Bike::Bike() {}

Bike::Bike(string vid, string oid, string mk,
           string mdl, string plate, int cap)
    : Vehicle(vid, oid, mk, mdl, plate, cap) {}

string Bike::getType() const { return "Bike"; }

void Bike::displayVehicle() const {
    Vehicle::displayVehicle();
}
