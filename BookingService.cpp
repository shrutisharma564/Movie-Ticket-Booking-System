#ifndef BOOKINGSERVICE_CPP
#define BOOKINGSERVICE_CPP
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include "Cinema.cpp"
#include "Movie.cpp"
#include "Show.cpp"
#include "Customer.cpp"
#include "Booking.cpp"
#include "PriceCalculator.cpp"
#include "Payment.cpp"
#include "TicketPrinter.cpp"
using namespace std;

// Association: Customer interacts with BookingService.
// Single Responsibility: orchestrates the booking flow.
//
// DIP (Dependency Inversion Principle):
// BookingService NEVER creates UpiPayment/CardPayment/CashPayment itself.
// It only depends on the Payment abstraction. The decision of WHICH concrete
// Payment to build is injected from outside (the composition root in
// main.cpp) as a factory function: function<Payment*(int,bool)>.
// This file therefore never includes UpiPayment.cpp/CardPayment.cpp/CashPayment.cpp.
class BookingService {
private:
    Cinema cinema;
    vector<Movie> movies;
    vector<Show> shows;
    Customer customer;
    PriceCalculator priceCalculator;
    TicketPrinter ticketPrinter;
    function<Payment*(int, bool)> paymentFactory;
    bool inputStreamEnded = false;

    // Association: BookingService keeps every Booking it has ever created so
    // that a cancellation can be looked up by Booking ID instead of by
    // re-choosing the movie/show/seat (which does not identify a booking).
    vector<Booking> bookings;

public:
    BookingService(function<Payment*(int, bool)> paymentFactory)
        : cinema("PVR Cinema"), customer("Guest Customer", "9999999999") {

        this->paymentFactory = paymentFactory;

        // 3 screens, 10 seats each (4 SILVER + 4 GOLD + 2 PLATINUM per screen
        // - see Screen.cpp for the exact split).
        this->cinema.addScreen(1, 10);
        this->cinema.addScreen(2, 10);
        this->cinema.addScreen(3, 10);

        this->movies.emplace_back("Interstellar", "English", 169);
        this->movies.emplace_back("Inception", "English", 148);
        this->movies.emplace_back("3 Idiots", "Hindi", 170);
        this->movies.emplace_back("Jawan", "Hindi", 169);
        this->movies.emplace_back("Kalki 2898 AD", "Hindi", 181);

        // Two shows per movie, spread across the 3 screens.
        this->shows.emplace_back(&this->movies[0], this->cinema.getScreen(1), "10:00 AM"); // Interstellar
        this->shows.emplace_back(&this->movies[0], this->cinema.getScreen(2), "07:00 PM"); // Interstellar

        this->shows.emplace_back(&this->movies[1], this->cinema.getScreen(2), "11:00 AM"); // Inception
        this->shows.emplace_back(&this->movies[1], this->cinema.getScreen(3), "04:00 PM"); // Inception

        this->shows.emplace_back(&this->movies[2], this->cinema.getScreen(1), "02:00 PM"); // 3 Idiots
        this->shows.emplace_back(&this->movies[2], this->cinema.getScreen(3), "08:00 PM"); // 3 Idiots

        this->shows.emplace_back(&this->movies[3], this->cinema.getScreen(1), "06:30 PM"); // Jawan
        this->shows.emplace_back(&this->movies[3], this->cinema.getScreen(2), "03:00 PM"); // Jawan

        this->shows.emplace_back(&this->movies[4], this->cinema.getScreen(3), "12:30 PM"); // Kalki 2898 AD
        this->shows.emplace_back(&this->movies[4], this->cinema.getScreen(1), "10:00 PM"); // Kalki 2898 AD
    }

    void displayMovies() const {
        cout << "\n===== MOVIES CURRENTLY PLAYING =====\n";
        for (size_t i = 0; i < this->movies.size(); ++i) {
            cout << i + 1 << ". " << this->movies[i].getTitle()
                 << " (" << this->movies[i].getLanguage()
                 << ", " << this->movies[i].getDuration() << " min)\n";
        }
    }

    void displayShows(int movieChoice) const {
        if (movieChoice < 1 || movieChoice > static_cast<int>(this->movies.size())) {
            cout << "Invalid movie choice.\n";
            return;
        }

        cout << "\n===== SHOWS =====\n";
        int number = 1;
        for (const Show& show : this->shows) {
            if (show.getMovie() == &this->movies[movieChoice - 1]) {
                cout << number++ << ". Screen "
                     << show.getScreen()->getScreenNumber()
                     << " | " << show.getStartTime() << "\n";
            }
        }
    }

    Show* chooseShow(int movieChoice) {
        vector<Show*> matchingShows;

        if (movieChoice < 1 || movieChoice > static_cast<int>(this->movies.size()))
            return nullptr;

        for (Show& show : this->shows) {
            if (show.getMovie() == &this->movies[movieChoice - 1])
                matchingShows.push_back(&show);
        }

        if (matchingShows.empty()) return nullptr;

        int choice;
        cout << "Choose show: ";
        cin >> choice;

        if (cin.fail()) {
            this->handleBadInput("Invalid show choice.");
            return nullptr;
        }

        if (choice < 1 || choice > static_cast<int>(matchingShows.size())) {
            cout << "Invalid show choice.\n";
            return nullptr;
        }

        return matchingShows[choice - 1];
    }

    void displaySeats(Show* show) const {
        if (show != nullptr) show->displaySeatLayout();
    }

    // Robustness helper: called right after a failed cin >> read.
    // If the input stream has actually run out (EOF - e.g. piped input
    // ended), we must stop reading instead of looping forever. Otherwise
    // we just clear the failbit, discard the bad line and let the caller
    // ask again. Returns true when the caller should stop the program.
    bool handleBadInput(const string& message) {
        if (cin.eof()) {
            cout << "\nNo more input available. Exiting.\n";
            this->inputStreamEnded = true;
            return true;
        }
        cin.clear();
        cin.ignore(10000, '\n');
        cout << message << "\n";
        return false;
    }

    // Books one OR MORE seats in a single booking (assignment requirement:
    // "one or more seats"). Validation is all-or-nothing: every requested
    // seat number must exist, be unique within the request, and be
    // AVAILABLE, or NO seat is touched and NO Booking is created.
    bool bookSeatsDemo(Show* show, const vector<int>& seatNumbers, int paymentChoice, bool paymentSuccess) {
        if (show == nullptr) {
            cout << "Invalid show.\n";
            return false;
        }

        if (seatNumbers.empty()) {
            cout << "No seats selected.\n";
            return false;
        }

        vector<ShowSeat*> selectedSeats;
        for (size_t i = 0; i < seatNumbers.size(); ++i) {
            int seatNumber = seatNumbers[i];

            for (size_t j = i + 1; j < seatNumbers.size(); ++j) {
                if (seatNumbers[j] == seatNumber) {
                    cout << "Duplicate seat number " << seatNumber
                         << " in selection. Booking rejected.\n";
                    return false;
                }
            }

            ShowSeat* seat = show->getShowSeat(seatNumber);
            if (seat == nullptr) {
                cout << "Invalid seat number: " << seatNumber << ". No changes made.\n";
                return false;
            }
            if (!seat->isAvailable()) {
                cout << "Seat " << seatNumber << " is already BOOKED. Booking rejected.\n";
                return false;
            }
            selectedSeats.push_back(seat);
        }

        // Compile-time polymorphism: calls the overloaded calculateTotal()
// function with the selected seats. Pricing follows the assignment:
// SILVER = Rs.150, GOLD = Rs.250, PLATINUM = Rs.400.
double total = this->priceCalculator.calculateTotal(selectedSeats);

        // DIP in action: BookingService asks the injected factory for a
        // Payment* - it has no idea which concrete class comes back.
        Payment* payment = this->paymentFactory(paymentChoice, paymentSuccess);

        if (payment == nullptr) {
            cout << "Invalid payment choice. No changes made.\n";
            return false;
        }

        // Runtime polymorphism: the concrete pay() invoked depends on the
        // actual object the Payment* points to, decided at runtime.
        bool paid = payment->pay(total);

        if (!paid) {
            cout << "Payment failed. Booking NOT confirmed; seats remain AVAILABLE.\n";
            delete payment;
            return false;
        }

        // Seats are marked booked only after successful payment.
        for (ShowSeat* seat : selectedSeats) seat->bookSeat();

        // «create» Booking - one Booking holds ALL the seats from this request.
        Booking booking(show, selectedSeats, total);
        booking.confirmBooking();
        this->bookings.push_back(booking);

        // Compile-time polymorphism: overloaded printTicket() - cash
        // payments get an extra note via the two-argument overload.
        if (paymentChoice == 3) {
            this->ticketPrinter.printTicket(booking, "Cash paid at counter. Please keep the receipt.");
        } else {
            this->ticketPrinter.printTicket(booking);
        }

        delete payment;
        return true;
    }

    // Cancellation is now driven by Booking ID (not by re-picking a seat),
    // matching the assignment's intended flow: find the Booking, cancel it,
    // and let Booking::cancelBooking() release every seat that belongs to it.
    void cancelBookingById(int bookingId) {
        for (Booking& booking : this->bookings) {
            if (booking.getBookingId() == bookingId) {
                if (booking.getStatus() == "CANCELLED") {
                    cout << "Booking B" << bookingId << " is already CANCELLED.\n";
                    return;
                }

                booking.cancelBooking();
                cout << "Booking B" << bookingId << " cancelled. "
                     << booking.getSeats().size()
                     << " seat(s) released and marked AVAILABLE.\n";
                return;
            }
        }
        cout << "Booking ID B" << bookingId << " not found.\n";
    }

    void run() {
        cout << "\n========================================\n";
        cout << "       MOVIE TICKET BOOKING SYSTEM\n";
        cout << "========================================\n";

        while (true) {
            cout << "\n1. List Movies\n";
            cout << "2. Book Ticket\n";
            cout << "3. Cancel Booking\n";
            cout << "4. Exit\n";
            cout << "Enter choice: ";

            int menuChoice;
            cin >> menuChoice;

            if (cin.fail()) {
                if (this->handleBadInput("Invalid input. Please enter a number.")) break;
                continue;
            }

            if (menuChoice == 1) {
                this->displayMovies();
            } else if (menuChoice == 2) {
                this->displayMovies();

                int movieChoice;
                cout << "Choose movie: ";
                cin >> movieChoice;

                if (cin.fail()) {
                    if (this->handleBadInput("Invalid movie choice.")) break;
                    continue;
                }
                if (movieChoice < 1 || movieChoice > static_cast<int>(this->movies.size())) {
                    cout << "Invalid movie choice.\n";
                    continue;
                }

                this->displayShows(movieChoice);
                Show* selectedShow = this->chooseShow(movieChoice);
                if (this->inputStreamEnded) break;
                if (selectedShow == nullptr) continue;

                this->displaySeats(selectedShow);

                // Assignment requirement: "one or more seats" per booking.
                int seatCount;
                cout << "How many seats? ";
                cin >> seatCount;

                if (cin.fail()) {
                    if (this->handleBadInput("Invalid number of seats.")) break;
                    continue;
                }
                if (seatCount < 1) {
                    cout << "You must book at least 1 seat.\n";
                    continue;
                }

                vector<int> seatNumbers;
                cout << "Enter " << seatCount << " seat number(s) (space-separated): ";
                bool badSeatInput = false;
                for (int i = 0; i < seatCount; ++i) {
                    int seatNumber;
                    cin >> seatNumber;
                    if (cin.fail()) {
                        badSeatInput = true;
                        break;
                    }
                    seatNumbers.push_back(seatNumber);
                }

                if (badSeatInput) {
                    if (this->handleBadInput("Invalid seat number.")) break;
                    continue;
                }

                cout << "\nPayment Method:\n";
                cout << "1. UPI\n2. Card\n3. Cash\n";
                cout << "Choose payment: ";

                int paymentChoice;
                cin >> paymentChoice;

                if (cin.fail()) {
                    if (this->handleBadInput("Invalid payment choice.")) break;
                    continue;
                }

                bool paymentSuccess = true;
                if (paymentChoice == 1 || paymentChoice == 2) {
                    char successChoice;
                    cout << "Simulate successful payment? (Y/N): ";
                    cin >> successChoice;
                    paymentSuccess = (successChoice == 'Y' || successChoice == 'y');
                }

                this->bookSeatsDemo(selectedShow, seatNumbers, paymentChoice, paymentSuccess);
            } else if (menuChoice == 3) {
                // Cancellation is by Booking ID, e.g. the "B1001" printed on
                // the ticket - this correctly cancels ALL seats belonging to
                // that booking, not just one seat.
                int bookingId;
                cout << "Enter Booking ID (e.g. 1001): ";
                cin >> bookingId;

                if (cin.fail()) {
                    if (this->handleBadInput("Invalid Booking ID.")) break;
                    continue;
                }

                this->cancelBookingById(bookingId);
            } else if (menuChoice == 4) {
                cout << "Thank you for using the system.\n";
                break;
            } else {
                cout << "Invalid menu choice. Please try again.\n";
            }
        }
    }
};
#endif
