#pragma once
#include "Vehicle.h"

class Car : public Vehicle {
public:
    Car();
    Car(std::string vid, std::string oid, std::string mk,
        std::string mdl, std::string plate, int cap);

    std::string getType()        const override;
    void        displayVehicle() const override;
};
