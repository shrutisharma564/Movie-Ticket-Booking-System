#ifndef CINEMA_CPP
#define CINEMA_CPP
#include <iostream>
#include <vector>
#include <string>
#include "Screen.cpp"
using namespace std;

// OOP: Composition - Cinema owns its Screen objects.
class Cinema {
private:
    string name;
    vector<Screen> screens;

public:
    Cinema(string name) {
        this->name = name;
    }

    void addScreen(int screenNumber, int seatCount) {
        this->screens.emplace_back(screenNumber, seatCount);
    }

    Screen* getScreen(int screenNumber) {
        for (Screen& screen : this->screens) {
            if (screen.getScreenNumber() == screenNumber) return &screen;
        }
        return nullptr;
    }

    void displayScreens() const {
        cout << "Cinema: " << this->name << "\n";
        for (const Screen& screen : this->screens) {
            cout << "Screen " << screen.getScreenNumber() << "\n";
        }
    }
};
#endif
