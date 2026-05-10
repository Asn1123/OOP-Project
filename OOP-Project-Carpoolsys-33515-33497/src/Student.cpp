#include "Student.h"
#include <iostream>

Student::Student()
    : gender(Gender::MALE), isDriver(false), ridesCompleted(0) {}

Student::Student(std::string id, std::string n, std::string phone,
                 Gender g, bool driver, int rides)
    : studentId(id), name(n), phoneNumber(phone),
      gender(g), isDriver(driver), ridesCompleted(rides) {}

Student::Student(const Student& other)
    : studentId(other.studentId), name(other.name), phoneNumber(other.phoneNumber),
      gender(other.gender), isDriver(other.isDriver), ridesCompleted(other.ridesCompleted) {}

std::string Student::getId()             const { return studentId; }
std::string Student::getName()           const { return name; }
std::string Student::getPhoneNumber()    const { return phoneNumber; }
Gender      Student::getGender()         const { return gender; }
bool        Student::getIsDriver()       const { return isDriver; }
int         Student::getRidesCompleted() const { return ridesCompleted; }

void Student::setName(const std::string& n)  { name = n; }
void Student::setPhone(const std::string& p) { phoneNumber = p; }
void Student::setGender(Gender g)            { gender = g; }

void Student::incrementRidesCompleted() { ridesCompleted++; }

void Student::displayProfile() const {
    std::cout << "  ID     : " << studentId << "\n"
              << "  Name   : " << name << "\n"
              << "  Phone  : " << phoneNumber << "\n"
              << "  Role   : " << (isDriver ? "Driver" : "Passenger") << "\n"
              << "  Rides  : " << ridesCompleted << "\n";
}

bool Student::operator==(const Student& other) const { return studentId == other.studentId; }
bool Student::operator!=(const Student& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Student& s) {
    os << "[" << s.studentId << "] " << s.name
       << " | " << (s.isDriver ? "Driver" : "Passenger")
       << " | Rides completed: " << s.ridesCompleted;
    return os;
}
