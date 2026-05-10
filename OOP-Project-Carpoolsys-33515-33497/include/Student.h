#pragma once
#include <string>
#include <ostream>
#include "enums.h"

class Student {
private:
    std::string studentId;
    std::string name;
    std::string phoneNumber;
    Gender      gender;
    bool        isDriver;
    int         ridesCompleted;

public:
    Student();
    Student(std::string id, std::string n, std::string phone,
            Gender g, bool driver, int rides = 0);
    Student(const Student& other);

    std::string getId()             const;
    std::string getName()           const;
    std::string getPhoneNumber()    const;
    Gender      getGender()         const;
    bool        getIsDriver()       const;
    int         getRidesCompleted() const;

    void setName(const std::string& n);
    void setPhone(const std::string& p);
    void setGender(Gender g);
    void incrementRidesCompleted();
    void displayProfile() const;

    bool operator==(const Student& other) const;
    bool operator!=(const Student& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Student& s);
};
