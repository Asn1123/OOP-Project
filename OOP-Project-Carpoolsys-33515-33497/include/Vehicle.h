#pragma once
#include <string>
#include <ostream>
using namespace std;

class Vehicle {
protected:
    string vehicleId;
    string ownerId;
    string make;
    string model;
    string licensePlate;
    int capacity;

public:
    Vehicle();
    Vehicle(string vid, string oid, string mk,
            string mdl, string plate, int cap);
    virtual ~Vehicle() {}

    string getVehicleId() const;
    string getOwnerId() const;
    string getMake() const;
    string getModel() const;
    string getLicensePlate() const;
    int getCapacity() const;

    void setMake(const string& m);
    void setModel(const string& m);
    void setLicensePlate(const string& p);
    void setCapacity(int c);

    virtual string getType() const = 0;
    virtual void displayVehicle() const;

    friend ostream& operator<<(ostream& os, const Vehicle& v);
};
