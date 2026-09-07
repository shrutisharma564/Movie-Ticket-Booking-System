#ifndef CUSTOMER_CPP
#define CUSTOMER_CPP
#include <string>
using namespace std;

// OOP: Encapsulation
class Customer {
private:
    string name;
    string phone;

public:
    Customer(string name, string phone) {
        this->name = name;
        this->phone = phone;
    }

    string getName() const { return this->name; }
    string getPhone() const { return this->phone; }
};
#endif
