#pragma once
#include "Vehicle.h"
using namespace std;

class Bike : public Vehicle {
public:
    Bike();
    Bike(string vid, string oid, string mk,
         string mdl, string plate, int cap);

    string getType()        const override;
    void   displayVehicle() const override;
};
