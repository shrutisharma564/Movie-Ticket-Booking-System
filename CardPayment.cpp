#ifndef CARDPAYMENT_CPP
#define CARDPAYMENT_CPP
#include <iostream>
#include "Payment.cpp"
using namespace std;

// OOP: Inheritance + Runtime Polymorphism
class CardPayment : public Payment {
private:
    bool simulateSuccess;

public:
    CardPayment(bool simulateSuccess) {
        this->simulateSuccess = simulateSuccess;
    }

    bool pay(double amount) override {
        cout << "\nProcessing Card payment of Rs." << amount << "...\n";
        if (this->simulateSuccess) {
            cout << "Card payment successful.\n";
            return true;
        }
        cout << "Card payment failed.\n";
        return false;
    }
};
#endif
