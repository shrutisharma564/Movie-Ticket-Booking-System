#ifndef SHOWSEAT_CPP
#define SHOWSEAT_CPP
#include <iostream>
#include <string>
#include "Seat.cpp"
using namespace std;

// OOP: Encapsulation - status is private and changed only through methods.
class ShowSeat {
private:
    Seat* seat;
    string status;

public:
    ShowSeat(Seat* seat) {
        this->seat = seat;
        this->status = "AVAILABLE";
    }

    bool isAvailable() const { return this->status == "AVAILABLE"; }

    bool bookSeat() {
        if (!this->isAvailable()) return false;
        this->status = "BOOKED";
        return true;
    }

    void cancelSeat() { this->status = "AVAILABLE"; }

    Seat* getSeat() const { return this->seat; }
    string getStatus() const { return this->status; }
};
#endif
