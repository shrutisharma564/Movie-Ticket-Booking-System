#ifndef UPIPAYMENT_CPP
#define UPIPAYMENT_CPP
#include <iostream>
#include "Payment.cpp"
using namespace std;

// OOP: Inheritance + Runtime Polymorphism
class UpiPayment : public Payment {
private:
    bool simulateSuccess;

public:
    UpiPayment(bool simulateSuccess) {
        this->simulateSuccess = simulateSuccess;
    }

    bool pay(double amount) override {
        cout << "\nProcessing UPI payment of Rs." << amount << "...\n";
        if (this->simulateSuccess) {
            cout << "UPI payment successful.\n";
            return true;
        }
        cout << "UPI payment failed.\n";
        return false;
    }
};
#endif
