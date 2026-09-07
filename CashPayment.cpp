#ifndef CASHPAYMENT_CPP
#define CASHPAYMENT_CPP
#include <iostream>
#include "Payment.cpp"
using namespace std;

// OOP: Inheritance + Runtime Polymorphism
class CashPayment : public Payment {
public:
    bool pay(double amount) override {
        cout << "\nCash payment of Rs." << amount << " received.\n";
        return true;
    }
};
#endif
