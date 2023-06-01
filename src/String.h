#ifndef TICKETSYSTEM_SRC_STRING_H
#define TICKETSYSTEM_SRC_STRING_H
#include <iostream>
namespace sjtu{
    template<int size>
    struct String {
        char string[size];

        String() {
            memset(string, 0, sizeof(string));
        }

        String(const char *s) {
            strcpy(string, s);
        }

        String(const std::string &s) {
            strcpy(string, s.c_str());
        }

        String &operator=(const String &rhs) {
            if (this == &rhs) return *this;
            strcpy(string, rhs.string);
            return *this;
        }

        bool operator<(const String &rhs) const {
            return strcmp(string, rhs.string) < 0;
        }

        bool operator==(const String &rhs) const {
            return strcmp(string, rhs.string) == 0;
        }

        bool operator!=(const String &rhs) const {
            return !(*this == rhs);
        }

        friend std::istream &operator>>(std::istream &is, String &s) {
            is >> s.string;
            return is;
        }

        friend std::ostream &operator<<(std::ostream &os, String s) {
            os << s.string;
            return os;
        }
    };
}
#endif //TICKETSYSTEM_SRC_STRING_H
