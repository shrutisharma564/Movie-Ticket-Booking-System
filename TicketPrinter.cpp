#ifndef TICKETPRINTER_CPP
#define TICKETPRINTER_CPP
#include <iostream>
#include <iomanip>
#include <string>
#include "Booking.cpp"
using namespace std;

// Single Responsibility: ticket formatting/printing only.
class TicketPrinter {
public:
    // Compile-time polymorphism: function overloading.
    // Overload 1: plain ticket, no extra note.
    void printTicket(const Booking& booking) const {
        this->printTicket(booking, "");
    }

    // Overload 2: same name, extra "note" parameter -> resolved at compile time.
    void printTicket(const Booking& booking, const string& note) const {
        Show* show = booking.getShow();

        cout << "\n========================================\n";
        cout << "              MOVIE TICKET\n";
        cout << "========================================\n";
        cout << "Booking ID : B" << booking.getBookingId() << "\n";
        cout << "Movie      : " << show->getMovie()->getTitle() << "\n";
        cout << "Screen     : " << show->getScreen()->getScreenNumber() << "\n";
        cout << "Time       : " << show->getStartTime() << "\n";
        cout << "Seats      : ";

        const auto& seats = booking.getSeats();
        for (size_t i = 0; i < seats.size(); ++i) {
            if (seats[i] != nullptr && seats[i]->getSeat() != nullptr)
                cout << seats[i]->getSeat()->getSeatNumber();
            if (i + 1 < seats.size()) cout << ", ";
        }

        cout << "\nAmount     : Rs." << fixed << setprecision(2)
             << booking.getTotalAmount() << "\n";
        cout << "Status     : " << booking.getStatus() << "\n";

        if (!note.empty()) {
            cout << "Note       : " << note << "\n";
        }

        cout << "========================================\n";
    }
};
#endif
