#ifndef PAYMENT_CPP
#define PAYMENT_CPP

// OOP: Abstraction
// This is the ONLY payment type BookingService is allowed to know about.
// Concrete payment types (UpiPayment/CardPayment/CashPayment) are created
// outside BookingService (see main.cpp) and handed in as a Payment*.
class Payment {
public:
    // Runtime polymorphism: the actual pay() that executes is decided at
    // runtime, based on the real (derived) object the Payment* points to.
    virtual bool pay(double amount) = 0;
    virtual ~Payment() = default;
};

#endif
