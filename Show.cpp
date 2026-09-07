#ifndef SHOW_CPP
#define SHOW_CPP
#include <iostream>
#include <vector>
#include <string>
#include "Movie.cpp"
#include "Screen.cpp"
#include "ShowSeat.cpp"
using namespace std;

// OOP: Aggregation - Show refers to an existing Movie and Screen.
// OOP: Composition - Show owns its ShowSeat objects.
class Show {
private:
    Movie* movie;
    Screen* screen;
    string startTime;
    vector<ShowSeat> showSeats;

public:
    Show(Movie* movie, Screen* screen, string startTime) {
        this->movie = movie;
        this->screen = screen;
        this->startTime = startTime;

        // Built from the screen's actual seat count (not a hard-coded number)
        // so an invalid/undersized screen can never produce a null seat later.
        int seatCount = (this->screen != nullptr) ? this->screen->getSeatCount() : 0;
        for (int i = 1; i <= seatCount; ++i) {
            this->showSeats.emplace_back(this->screen->getSeat(i));
        }
    }

    Movie* getMovie() const { return this->movie; }
    Screen* getScreen() const { return this->screen; }
    string getStartTime() const { return this->startTime; }

    ShowSeat* getShowSeat(int seatNumber) {
        if (seatNumber < 1 || seatNumber > static_cast<int>(this->showSeats.size())) return nullptr;
        return &this->showSeats[seatNumber - 1];
    }

    void displaySeatLayout() const {
        cout << "\nSeat Layout for " << this->movie->getTitle()
             << " | Screen " << this->screen->getScreenNumber()
             << " | " << this->startTime << "\n";
        cout << "-----------------------------------\n";

        for (const ShowSeat& ss : this->showSeats) {
            Seat* seat = ss.getSeat();
            if (seat == nullptr) continue;
            cout << "Seat " << seat->getSeatNumber()
                 << " [" << seat->getSeatType() << "] - "
                 << ss.getStatus() << "\n";
        }
        cout << "-----------------------------------\n";
    }
};
#endif
