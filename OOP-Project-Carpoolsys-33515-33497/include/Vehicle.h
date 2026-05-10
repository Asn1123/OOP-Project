#pragma once
#include <string>
#include <ostream>

class Vehicle {
protected:
    std::string vehicleId;
    std::string ownerId;
    std::string make;
    std::string model;
    std::string licensePlate;
    int         capacity;

public:
    Vehicle();
    Vehicle(std::string vid, std::string oid, std::string mk,
            std::string mdl, std::string plate, int cap);
    virtual ~Vehicle() {}

    std::string getVehicleId()    const;
    std::string getOwnerId()      const;
    std::string getMake()         const;
    std::string getModel()        const;
    std::string getLicensePlate() const;
    int         getCapacity()     const;

    void setMake        (const std::string& m);
    void setModel       (const std::string& m);
    void setLicensePlate(const std::string& p);
    void setCapacity    (int c);

    virtual std::string getType()        const = 0;
    virtual void        displayVehicle() const;

    // Prints type, make, model, plate, seats via virtual dispatch
    friend std::ostream& operator<<(std::ostream& os, const Vehicle& v);
};
