#ifndef TICKETSYSTEM_SRC_SCHEDULE_H
#define TICKETSYSTEM_SRC_SCHEDULE_H

#include <iostream>
#include <string>

namespace sjtu {
    class Date { // 6-30 7-31 8-31
    public:
        short month_;
        short day_;

        Date() {}

        Date(const std::string &s) {
            month_ = (short) ((s[0] - '0') * 10 + s[1] - '0');
            day_ = (short) ((s[3] - '0') * 10 + s[4] - '0');
        }

        Date &operator++() {
            ++day_;
            if (day_ < 31) return *this;
            else if (day_ == 31) {
                if (month_ == 6) {
                    ++month_;
                    day_ = 1;
                }
                return *this;
            } else {
                ++month_;
                day_ = 1;
                return *this;
            }

        }

        friend bool operator==(const Date &a, const Date &b) {
            return a.month_ == b.month_ && a.day_ == b.day_;
        }

        friend bool operator<(const Date &a, const Date &b) {
            if (a.month_ != b.month_) return a.month_ < b.month_;
            else return a.day_ < b.day_;
        }

        friend std::ostream &operator<<(std::ostream &os, Date &d) {
            os << d.month_ / 10 << d.month_ % 10 << '-' << d.day_ / 10 << d.day_ % 10;
            return os;
        }
//    Date &operator+=(const short day){
//        day_+=day;
//        if(day_<31) return *this;
//        else if(day_==31){
//            if(month_==6){
//                ++month_;
//                day_-=30;
//            }
//            return *this;
//        }else{
//            while(day_>30){
//                ++month_;
//                day_-=30;
//            }
//            return *this;
//        }
//    }

    };

    class Time {
    public:
        short hour_;
        short minute_;

        Time() {}

        Time(const std::string &s) {
            hour_ = (short) ((s[0] - '0') * 10 + s[1] - '0');
            minute_ = (short) ((s[3] - '0') * 10 + s[4] - '0');
        }

        friend std::ostream &operator<<(std::ostream &os, Time &t) {
            os << t.hour_ / 10 << t.hour_ % 10 << ':' << t.minute_ / 10 << t.minute_ % 10;
            return os;
        }

        void update(Date &d, Time &t) {
            while (t.hour_ >= 24) {
                ++d;
                t.hour_ -= 24;
            }
        }

    };
}

#endif //TICKETSYSTEM_SRC_SCHEDULE_H
