#ifndef TICKETSYSTEM_SRC_TRAINSYSTEM_H
#define TICKETSYSTEM_SRC_TRAINSYSTEM_H

#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
#include "bpt.h"
#include "multi_object_bpt.h"
#include "schedule.h"


class Train {
public:
    int tag_ = -1;
    char type;
    char trainID_[21];
    char stations_[100][31];
    bool release = false;
    int station_num_;
    int seat_num_;
    int prices_[100];
    sjtu::Date start_date_;
    sjtu::Date end_date_;
    sjtu::Time arrive_time_[99];
    sjtu::Time leave_time_[99];

    Train() {
        memset(trainID_, 0, sizeof(trainID_));
        memset(stations_, 0, sizeof(stations_));
        memset(prices_, 0, sizeof(prices_));
    }
};

class TrainSystem {
public:
    BPT<char[21]> train_map_;
//    MultiBPT<
    std::fstream train_inf_;
    std::string filename_ = "train_inf";
    int amount = 0;
    sjtu::vector<int> storage_of_tag_; // store unused tag;


    TrainSystem() : train_map_("trainID_tree", "trainID_record", "train_tag") {
        train_inf_.open(filename_);
        if (!train_inf_.good()) { // 是否成功打开
            train_inf_.open(filename_, std::fstream::out); // 新建
            train_inf_.close();
            train_inf_.open(filename_); // 变为可读可写
        } else {
            train_inf_.seekg(0);
            train_inf_.read(reinterpret_cast <char *> (&amount), sizeof(amount));
        }
    }

    ~TrainSystem() {
        train_inf_.seekp(0);
        train_inf_.write(reinterpret_cast <char *> (&amount), sizeof(amount));
        train_inf_.close();
    }

    void readFile(Train &read, const int &location) {
        train_inf_.seekg(sizeof(Train) * location + sizeof(amount));
        train_inf_.read(reinterpret_cast<char *>(&read), sizeof(Train));
    }

    void writeFile(Train &write, const int &location) {
        train_inf_.seekp(sizeof(Train) * location + sizeof(amount));
        train_inf_.write(reinterpret_cast<char *>(&write), sizeof(Train));
    }

    int AssignTag() {
        if (storage_of_tag_.empty()) {
            return amount++;
        } else {
            int tag = storage_of_tag_.back();
            storage_of_tag_.pop_back();
            return tag;
        }
    }

    void RestoreTag(int tag) {
        storage_of_tag_.push_back(tag);
    }

    void AddTrain(Train &new_inf) {
        int tag = AssignTag();
        train_map_.insert(new_inf.trainID_, tag);
        writeFile(new_inf, tag);
        std::cout << "0\n";
    }

    int DeleteTrain(const char(&trainID)[21]) {
        int tag = train_map_.find(trainID);
        if (tag == -1) return -1;
        Train tmp;
        readFile(tmp, tag);
        if (tmp.release) return -1;
        train_map_.erase(trainID, tag);
        RestoreTag(tag);
    }

    int ReleaseTrain(const char(&trainID)[21]) {
        int tag = train_map_.find(trainID);
        if (tag == -1) return -1;
        Train tmp;
        readFile(tmp, tag);
        if (tmp.release) return -1;
        tmp.release = true;
        writeFile(tmp, tag);
        return 0;
    }

    void QueryTrain(const char(&trainID)[21], sjtu::Date date) {
        int tag = train_map_.find(trainID);
        if (tag == -1) {
            std::cout << "-1\n";
            return;
        }
        Train tmp;
        readFile(tmp, tag);
        if (date < tmp.start_date_ || tmp.end_date_ < date) {
            std::cout << "-1\n";
            return;
        }
        if (tmp.release) {
            std::cout << tmp.trainID_ << ' ' << tmp.type << '\n';
            std::cout << tmp.stations_[0] << " xx-xx xx:xx -> " << date << ' ' <<
                      tmp.leave_time_[0] << " 0 " << seat->s[0] << '\n';
            sjtu::Date d;
            sjtu::Time t;
            for (int i = 1; i < tmp.station_num_ - 1; i++) {
                d = date;
                t = tmp.arrive_time_[i - 1];
                t.update(d, t);
                std::cout << tmp.stations_[i] << ' ' << d << ' ' << t << " -> ";
                d = date;
                t = tmp.leave_time_[i];
                t.update(d, t);
                std::cout << d << ' ' << t << ' ' << tmp.prices_[i] << ' ' << seat->s[i] << '\n';
            }
            d = date;
            t = tmp.arrive_time_[tmp.station_num_ - 2];
            t.update(d, t);
            std::cout << tmp.stations_[tmp.station_num_ - 1] << ' ' << d << ' ' << t <<
                      " -> xx-xx xx:xx " << tmp.prices_[tmp.station_num_ - 1] << " x\n";
        } else {
            std::cout << tmp.trainID_ << ' ' << tmp.type << '\n';
            std::cout << tmp.stations_[0] << " xx-xx xx:xx -> " << date << ' ' <<
                      tmp.leave_time_[0] << " 0 " << tmp.seat_num_ << '\n';
            sjtu::Date d;
            sjtu::Time t;
            for (int i = 1; i < tmp.station_num_ - 1; i++) {
                d = date;
                t = tmp.arrive_time_[i - 1];
                t.update(d, t);
                std::cout << tmp.stations_[i] << ' ' << d << ' ' << t << " -> ";
                d = date;
                t = tmp.leave_time_[i];
                t.update(d, t);
                std::cout << d << ' ' << t << ' ' << tmp.prices_[i] << ' ' << tmp.seat_num_ << '\n';
            }
            d = date;
            t = tmp.arrive_time_[tmp.station_num_ - 2];
            t.update(d, t);
            std::cout << tmp.stations_[tmp.station_num_ - 1] << ' ' << d << ' ' << t <<
                      " -> xx-xx xx:xx " << tmp.prices_[tmp.station_num_ - 1] << " x\n";
        }
    }

    void QueryTicket(const char(&start)[31],const char (&target)[31],sjtu::Date date,bool p){

    }
};

#endif //TICKETSYSTEM_SRC_TRAINSYSTEM_H
