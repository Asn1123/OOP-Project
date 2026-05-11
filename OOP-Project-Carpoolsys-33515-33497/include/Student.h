#pragma once
#include <string>
#include <ostream>
#include "enums.h"
using namespace std;

class Student {
private:
    string studentId;
    string name;
    string phoneNumber;
    Gender gender;
    bool isDriver;
    int ridesDone;

public:
    Student();
    Student(string id, string n, string phone,
            Gender g, bool driver, int rides = 0);
    Student(const Student& other);

    string getId() const;
    string getName() const;
    string getPhoneNumber() const;
    Gender getGender() const;
    bool getIsDriver() const;
    int getRidesDone() const;

    void setName(const string& n);
    void setPhone(const string& p);
    void setGender(Gender g);
    void addRide();
    void displayProfile() const;

    bool operator==(const Student& other) const;
    bool operator!=(const Student& other) const;

    friend ostream& operator<<(ostream& os, const Student& s);
};
