#ifndef PRICECALCULATOR_CPP
#define PRICECALCULATOR_CPP
#include <vector>
#include "ShowSeat.cpp"
using namespace std;

// Single Responsibility: pricing only.
class PriceCalculator {
public:
    // Compile-time polymorphism: function overloading.
    // Same name "calculateTotal", different parameter lists -> the compiler
    // decides which one to call based on the arguments used at the call site.

    // Overload 1: plain total, no discount.
    double calculateTotal(const vector<ShowSeat*>& seats) const {
        double total = 0.0;
        for (ShowSeat* showSeat : seats) {
            if (showSeat != nullptr && showSeat->getSeat() != nullptr)
                total += showSeat->getSeat()->getPrice();
        }
        return total;
    }

    // Overload 2: total after applying a discount percentage.
    double calculateTotal(const vector<ShowSeat*>& seats, double discountPercent) const {
        double total = this->calculateTotal(seats);
        double discount = total * (discountPercent / 100.0);
        return total - discount;
    }
};
#endif
