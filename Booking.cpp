#ifndef BOOKING_CPP
#define BOOKING_CPP
#include <vector>
#include <string>
#include "Show.cpp"
using namespace std;

// OOP: Encapsulation + Static Member
class Booking {
private:
    static int nextBookingId;
    int bookingId;
    Show* show;
    vector<ShowSeat*> seats;
    double totalAmount;
    string status;

public:
    Booking(Show* show, const vector<ShowSeat*>& seats, double amount) {
        this->bookingId = nextBookingId++;
        this->show = show;
        this->seats = seats;
        this->totalAmount = amount;
        this->status = "PENDING";
    }

    int getBookingId() const { return this->bookingId; }
    Show* getShow() const { return this->show; }
    const vector<ShowSeat*>& getSeats() const { return this->seats; }
    double getTotalAmount() const { return this->totalAmount; }
    string getStatus() const { return this->status; }

    void confirmBooking() { this->status = "CONFIRMED"; }

    void cancelBooking() {
        for (ShowSeat* seat : this->seats) {
            if (seat != nullptr) seat->cancelSeat();
        }
        this->status = "CANCELLED";
    }
};

// Static member shared by all Booking objects.
int Booking::nextBookingId = 1001;

#endif
