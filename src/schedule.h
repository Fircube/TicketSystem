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

        Date &operator+=(const short day) {
            day_ += day;
            while (day_ > 30) {
                if(month_==6){
                    month_=7;
                    day_-=30;
                }else if(day_>31){
                    month_=8;
                    day_-=31;
                }
            }
            return *this;
        }

        Date &operator-=(const short day) {
            day_ -= day;
            while (day_ <= 0) {
                if(month_==7){
                    month_=6;
                    day_+=30;
                }else{
                    month_=7;
                    day_+=31;
                }
            }
            return *this;
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

        friend bool operator>(const Date &a, const Date &b) {
            if (a.month_ != b.month_) return a.month_ > b.month_;
            else return a.day_ > b.day_;
        }

        friend bool operator<=(const Date &a, const Date &b) {
            return !(a > b);
        }

        friend bool operator>=(const Date &a, const Date &b) {
            return !(a < b);
        }

        friend std::ostream &operator<<(std::ostream &os, Date &d) {
            os << d.month_ / 10 << d.month_ % 10 << '-' << d.day_ / 10 << d.day_ % 10;
            return os;
        }

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

        Time operator+(const int &t) {
            Time time(*this);
            time.minute_ += t % 60;
            time.hour_ += t / 60 + time.minute_ / 60;
            time.minute_ %= 60;
            return time;
        }

        int operator-(const Time &t) const {
            int ans=minute_-t.minute_+60*(hour_-t.hour_);
            return ans;
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
