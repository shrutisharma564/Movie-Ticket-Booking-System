#ifndef SCREEN_CPP
#define SCREEN_CPP
#include <iostream>
#include <vector>
#include <string>
#include "Seat.cpp"
using namespace std;

// OOP: Composition - Screen owns its Seat objects.
class Screen {
private:
    int screenNumber;
    vector<Seat> seats;

public:
    Screen(int screenNumber, int seatCount) {
        this->screenNumber = screenNumber;

        // Demo layout: 1-4 Silver, 5-8 Gold, 9-10+ Platinum.
        for (int i = 1; i <= seatCount; ++i) {
            string type = (i <= 4) ? "SILVER" : (i <= 8 ? "GOLD" : "PLATINUM");
            this->seats.emplace_back(i, type);
        }
    }

    int getScreenNumber() const { return this->screenNumber; }

    // Exposed so dependent classes (like Show) never have to guess or
    // hard-code the number of seats a screen actually has.
    int getSeatCount() const { return static_cast<int>(this->seats.size()); }

    Seat* getSeat(int seatNumber) {
        if (seatNumber < 1 || seatNumber > static_cast<int>(this->seats.size())) return nullptr;
        return &this->seats[seatNumber - 1];
    }

    void displaySeats() const {
        cout << "\nScreen " << this->screenNumber << " Seat Layout\n";
        cout << "-----------------------------------\n";
        for (const Seat& seat : this->seats) {
            cout << "Seat " << seat.getSeatNumber()
                 << " [" << seat.getSeatType() << "]\n";
        }
        cout << "-----------------------------------\n";
    }
};
#endif
