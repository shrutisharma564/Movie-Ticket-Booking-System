#ifndef SEAT_CPP
#define SEAT_CPP
#include <string>
using namespace std;

// OOP: Encapsulation
class Seat {
private:
    int seatNumber;
    string seatType;

public:
    Seat(int seatNumber, string seatType) {
        this->seatNumber = seatNumber;
        this->seatType = seatType;
    }

    int getSeatNumber() const { return this->seatNumber; }
    string getSeatType() const { return this->seatType; }

    double getPrice() const {
        if (this->seatType == "SILVER") return 150.0;
        if (this->seatType == "GOLD") return 250.0;
        if (this->seatType == "PLATINUM") return 400.0;
        return 0.0;
    }
};
#endif
