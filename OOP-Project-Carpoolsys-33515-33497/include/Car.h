#pragma once
#include "Vehicle.h"
using namespace std;

class Car : public Vehicle {
public:
    Car();
    Car(string vid, string oid, string mk,
        string mdl, string plate, int cap);

    string getType()        const override;
    void   displayVehicle() const override;
};
