#include "Notification.h"
#include <iostream>
using namespace std;

Notification::Notification(string id, string msg)
    : toId(id), message(msg) {}

string Notification::getTo()      const { return toId; }
string Notification::getMessage() const { return message; }

void Notification::display() const {
    cout << "  [!] " << message << "\n";
}
