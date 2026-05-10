#pragma once
#include <string>

class Notification {
private:
    std::string recipientId;
    std::string message;

public:
    Notification(std::string rid, std::string msg);

    std::string getRecipientId() const;
    std::string getMessage()     const;
    void        display()        const;
};
