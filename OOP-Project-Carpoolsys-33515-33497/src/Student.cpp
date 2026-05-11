#include "Student.h"
#include <iostream>
using namespace std;

Student::Student()
    : gender(Gender::MALE), isDriver(false), ridesDone(0) {}

Student::Student(string id, string n, string phone,
                 Gender g, bool driver, int rides)
    : studentId(id), name(n), phoneNumber(phone),
      gender(g), isDriver(driver), ridesDone(rides) {}

Student::Student(const Student& other)
    : studentId(other.studentId), name(other.name), phoneNumber(other.phoneNumber),
      gender(other.gender), isDriver(other.isDriver), ridesDone(other.ridesDone) {}

string Student::getId() const { return studentId; }
string Student::getName() const { return name; }
string Student::getPhoneNumber() const { return phoneNumber; }
Gender Student::getGender() const { return gender; }
bool Student::getIsDriver() const { return isDriver; }
int Student::getRidesDone() const { return ridesDone; }

void Student::setName(const string& n) { name = n; }
void Student::setPhone(const string& p) { phoneNumber = p; }
void Student::setGender(Gender g) { gender = g; }

void Student::addRide() { ridesDone++; }

void Student::displayProfile() const {
    cout << "  ID    : " << studentId << "\n"
         << "  Name  : " << name << "\n"
         << "  Phone : " << phoneNumber << "\n"
         << "  Role  : " << (isDriver ? "Driver" : "Passenger") << "\n"
         << "  Rides : " << ridesDone << "\n";
}

bool Student::operator==(const Student& other) const { return studentId == other.studentId; }
bool Student::operator!=(const Student& other) const { return !(*this == other); }

ostream& operator<<(ostream& os, const Student& s) {
    os << "[" << s.studentId << "] " << s.name
       << " | " << (s.isDriver ? "Driver" : "Passenger")
       << " | Rides: " << s.ridesDone;
    return os;
}
