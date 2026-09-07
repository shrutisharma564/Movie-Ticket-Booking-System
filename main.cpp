#include "BookingService.cpp"
#include "UpiPayment.cpp"
#include "CardPayment.cpp"
#include "CashPayment.cpp"

// Composition root (DIP): this is the ONLY place in the program that knows
// about the concrete payment classes. It builds a Payment* and hands it to
// BookingService through an abstraction (Payment* / function<Payment*(...)>),
// so BookingService itself never depends on UpiPayment/CardPayment/CashPayment.
Payment* createPayment(int paymentChoice, bool simulateSuccess) {
    if (paymentChoice == 1) {
        return new UpiPayment(simulateSuccess);
    } else if (paymentChoice == 2) {
        return new CardPayment(simulateSuccess);
    } else if (paymentChoice == 3) {
        return new CashPayment();
    }
    return nullptr;
}

int main() {
    BookingService bookingService(createPayment);
    bookingService.run();
    return 0;
}
