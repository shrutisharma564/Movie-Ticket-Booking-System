#ifndef MOVIE_CPP
#define MOVIE_CPP
#include <string>
using namespace std;

// OOP: Encapsulation
class Movie {
private:
    string title;
    string language;
    int duration;

public:
    // Parameters shadow the members on purpose -> "this" is used to
    // disambiguate between the member and the parameter of the same name.
    Movie(string title, string language, int duration) {
        this->title = title;
        this->language = language;
        this->duration = duration;
    }

    string getTitle() const { return this->title; }
    string getLanguage() const { return this->language; }
    int getDuration() const { return this->duration; }
};
#endif
