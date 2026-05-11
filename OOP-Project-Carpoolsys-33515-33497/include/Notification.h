#pragma once
#include <string>
using namespace std;

class Notification {
private:
    string toId;
    string message;

public:
    Notification(string id, string msg);

    string getTo()      const;
    string getMessage() const;
    void   display()    const;
};
