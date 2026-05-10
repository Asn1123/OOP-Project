#include "Notification.h"
#include <iostream>

Notification::Notification(std::string rid, std::string msg)
    : recipientId(rid), message(msg) {}

std::string Notification::getRecipientId() const { return recipientId; }
std::string Notification::getMessage()     const { return message; }

void Notification::display() const {
    std::cout << "  [!] " << message << "\n";
}
