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
#include "utility.h"
#include "String.h"


class Train {
public:
    int tag_ = -1;
    char type_;
    char trainID_[21];
    char stations_[100][31];
    bool release_ = false;
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
private:
    struct StationIndex {
        sjtu::String<31> station_name_;
        int order_;

        StationIndex() {}

        StationIndex(const char(&station)[31], int o) {
            station_name_ = station;
            order_ = o;
        }

        bool operator<(const StationIndex &rhs) const {
            return station_name_ < rhs.station_name_;
        }

        bool operator>(const StationIndex &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const StationIndex &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const StationIndex &rhs) const {
            return !(*this < rhs);
        }

        bool operator==(const StationIndex &rhs) const {
            return station_name_ == rhs.station_name_;
        }

        bool operator!=(const StationIndex &rhs) const {
            return !(*this == rhs);
        }
    };

    struct SeatIndex {

        sjtu::Date date_;
        sjtu::String<21> trainID_;

        SeatIndex() {}

        SeatIndex(const sjtu::Date d, const char(&trainID)[21]) {
            date_ = d;
            trainID_ = trainID;
        }

        bool operator<(const SeatIndex &rhs) const {
            if (trainID_ != rhs.trainID_) return trainID_ < rhs.trainID_;
            else return date_ < rhs.date_;
        }

        bool operator>(const SeatIndex &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const SeatIndex &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const SeatIndex &rhs) const {
            return !(*this < rhs);
        }

        bool operator==(const SeatIndex &rhs) const {
            return trainID_ == rhs.trainID_ && date_ == rhs.date_;
        }

        bool operator!=(const SeatIndex &rhs) const {
            return !(*this == rhs);
        }

    };

    struct Seat {
        int seat_[99];

        Seat() {
            memset(seat_, 0, sizeof(Seat));
        }
    };

public:
    BPT<sjtu::String<21>, int> train_map_;
    BPT<SeatIndex, int> seat_map_;
    MultiBPT<StationIndex, int> station_map_;
    std::fstream train_inf_;
    std::string filename1_ = "train_inf";
    std::fstream seat_inf_;
    std::string filename2_ = "seat_inf";
    int amount_train_ = 0;
    int amount_seat_ = 0;
    sjtu::vector<int> storage_of_train_tag_; // store unused tag;
    sjtu::vector<int> storage_of_seat_tag_; // store unused tag;

    TrainSystem() : train_map_("trainID_tree", "trainID_record", "train_tag") {
        train_inf_.open(filename1_);
        if (!train_inf_.good()) { // 是否成功打开
            train_inf_.open(filename1_, std::fstream::out); // 新建
            train_inf_.close();
            train_inf_.open(filename1_); // 变为可读可写
        } else {
            train_inf_.seekg(0);
            train_inf_.read(reinterpret_cast <char *> (&amount_train_), sizeof(amount_train_));
        }
        seat_inf_.open(filename2_);
        if (!seat_inf_.good()) { // 是否成功打开
            seat_inf_.open(filename2_, std::fstream::out); // 新建
            seat_inf_.close();
            seat_inf_.open(filename2_); // 变为可读可写
        } else {
            seat_inf_.seekg(0);
            train_inf_.read(reinterpret_cast <char *> (&amount_seat_), sizeof(amount_seat_));

        }
    }

    ~TrainSystem() {
        train_inf_.seekp(0);
        train_inf_.write(reinterpret_cast <char *> (&amount_train_), sizeof(amount_train_));
        train_inf_.close();
        seat_inf_.seekp(0);
        seat_inf_.write(reinterpret_cast <char *> (&amount_seat_), sizeof(amount_seat_));
        seat_inf_.close();
    }

    void readTrainFile(Train &read, const int &location) {
        train_inf_.seekg(sizeof(Train) * location + sizeof(amount_train_));
        train_inf_.read(reinterpret_cast<char *>(&read), sizeof(Train));
    }

    void writeTrainFile(Train &write, const int &location) {
        train_inf_.seekp(sizeof(Train) * location + sizeof(amount_train_));
        train_inf_.write(reinterpret_cast<char *>(&write), sizeof(Train));
    }

    void readSeatFile(Seat &read, const int &location) {
        train_inf_.seekg(sizeof(Seat) * location + sizeof(amount_seat_));
        train_inf_.read(reinterpret_cast<char *>(&read), sizeof(Seat));
    }

    void writeSeatFile(Seat &write, const int &location) {
        train_inf_.seekp(sizeof(Seat) * location + sizeof(amount_seat_));
        train_inf_.write(reinterpret_cast<char *>(&write), sizeof(Seat));
    }

    int AssignTrainTag() {
        if (storage_of_train_tag_.empty()) {
            return amount_train_++;
        } else {
            int tag = storage_of_train_tag_.back();
            storage_of_train_tag_.pop_back();
            return tag;
        }
    }

    void RestoreTrainTag(int tag) {
        storage_of_train_tag_.push_back(tag);
    }

    int AssignSeatTag() {
        if (storage_of_seat_tag_.empty()) {
            return amount_seat_++;
        } else {
            int tag = storage_of_seat_tag_.back();
            storage_of_seat_tag_.pop_back();
            return tag;
        }
    }

    void RestoreSeatTag(int tag) {
        storage_of_seat_tag_.push_back(tag);
    }



    void AddTrain(Train &new_inf) {
        int tag = AssignTrainTag();
        train_map_.insert(new_inf.trainID_, tag);
        writeTrainFile(new_inf, tag);
        std::cout << "0\n";
    }

    int DeleteTrain(const char(&trainID)[21]) {
        int tag = train_map_.find(trainID);
        if (tag == -1) return -1;
        Train tmp;
        readTrainFile(tmp, tag);
        if (tmp.release_) return -1;
        train_map_.erase(trainID, tag);
        RestoreTrainTag(tag);
    }

    int ReleaseTrain(const char(&trainID)[21]) {
        int tag = train_map_.find(trainID);
        if (tag == -1) return -1;
        Train tmp;
        readTrainFile(tmp, tag);
        if (tmp.release_) return -1;
        tmp.release_ = true;
        writeTrainFile(tmp, tag);

        for (int i = 0; i < tmp.station_num_; ++i) {
            StationIndex station_index(tmp.stations_[i], i);
            station_map_.insert(station_index, tag);
        }

        int tag_s = AssignSeatTag();
        SeatIndex seat_index(tmp.start_date_, trainID);
        Seat seat;
        for (int i = 0; i < tmp.station_num_ - 1; ++i) seat.seat_[i] = tmp.seat_num_;
        for (; seat_index.date_ <= tmp.end_date_; ++seat_index.date_) {
            seat_map_.insert(seat_index, tag_s);
        }

        return 0;
    }

    void QueryTrain(const char(&trainID)[21], sjtu::Date date) { //
        int tag = train_map_.find(trainID);
        if (tag == -1) {
            std::cout << "-1\n";
            return;
        }
        Train tmp;
        readTrainFile(tmp, tag);
        if (date < tmp.start_date_ || tmp.end_date_ < date) {
            std::cout << "-1\n";
            return;
        }
        if (tmp.release_) {
            SeatIndex seat_index(date,trainID);
            int tag_s=seat_map_.find(seat_index);
            Seat seat;
            readSeatFile(seat,tag_s);
            std::cout << tmp.trainID_ << ' ' << tmp.type_ << '\n';
            std::cout << tmp.stations_[0] << " xx-xx xx:xx -> " << date << ' ' <<
                      tmp.leave_time_[0] << " 0 " << seat.seat_[0] << '\n';
            sjtu::Date d;
            sjtu::Time t;
            for (int i = 1; i < tmp.station_num_ - 1; ++i) {
                d = date;
                t = tmp.arrive_time_[i - 1];
                t.update(d, t);
                std::cout << tmp.stations_[i] << ' ' << d << ' ' << t << " -> ";
                d = date;
                t = tmp.leave_time_[i];
                t.update(d, t);
                std::cout << d << ' ' << t << ' ' << tmp.prices_[i] << ' ' << seat.seat_[i] << '\n';
            }
            d = date;
            t = tmp.arrive_time_[tmp.station_num_ - 2];
            t.update(d, t);
            std::cout << tmp.stations_[tmp.station_num_ - 1] << ' ' << d << ' ' << t <<
                      " -> xx-xx xx:xx " << tmp.prices_[tmp.station_num_ - 1] << " x\n";
        } else {
            std::cout << tmp.trainID_ << ' ' << tmp.type_ << '\n';
            std::cout << tmp.stations_[0] << " xx-xx xx:xx -> " << date << ' ' <<
                      tmp.leave_time_[0] << " 0 " << tmp.seat_num_ << '\n';
            sjtu::Date d;
            sjtu::Time t;
            for (int i = 1; i < tmp.station_num_ - 1; ++i) {
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
private:
    struct QueryData{
        char trainID_[21];
        char start[31];
        char target[31];
        int seat_num_;
        int prices_;
        sjtu::Date leave_date_;
        sjtu::Date arrive_date_;
        sjtu::Time leave_time_;
        sjtu::Time arrive_time_;
    };
public:
    void QueryTicket(const char(&start)[31], const char (&target)[31], sjtu::Date date, bool p) {
        sjtu::vector<int> satisfied_train;
        sjtu::vector<int> start_o;
        sjtu::vector<int> target_o;
        FindTicket(start,target,satisfied_train,start_o,target_o);
        int size=satisfied_train.size();
        QueryData query_data;
        for(int i=0;i<size;++i){
            Train train;
            readTrainFile(train,satisfied_train[i]);
//            if(start_o[i]==train.station_num_) continue;
            query_data.leave_time_=train.leave_time_[start_o[i]];
        }
    }

    void QueryTransfer(){

    }

    void BuyTicket(){

    };

    void QueryOrder(){

    }

    void RefundTicket(){

    }

    void clean(){

    }

private:
    void FindTicket(const char(&start)[31], const char (&target)[31], sjtu::vector<int> &satisfied_train,sjtu::vector<int> &s_o,sjtu::vector<int> &t_o) {
        sjtu::vector<int> start_tag,tar_tag;
        sjtu::vector<StationIndex> start_inf,tar_inf;
        StationIndex s(start, 0);
        station_map_.find(s, start_tag,start_inf);
        if(start_tag.empty()) return;
        StationIndex t(target, 0);
        station_map_.find(t, tar_tag,tar_inf);
        if(tar_tag.empty()) return;
        int start_size=start_tag.size(),tar_size=tar_tag.size();
        int i=0,j=0;
        while(i<start_size && j<tar_size){
            while(i!=start_size && start_tag[i]<tar_tag[i]) ++i;
            if(i==start_size) return;
            if(start_tag[i]!=tar_tag[i]){
                if(start_inf[i].order_<tar_inf[i].order_){
                    satisfied_train.push_back(start_tag[i]);
                    s_o.push_back(start_inf[i].order_);
                    t_o.push_back(tar_inf[i].order_);
                }
                ++i;
            }
            ++j;
            while(j!=tar_size &&tar_tag[i] < start_tag[i]) ++j;
            if(j==tar_size) return;
            if(start_tag[i]!=tar_tag[i]){
                if(start_inf[i].order_<tar_inf[i].order_){
                    satisfied_train.push_back(start_tag[i]);
                    s_o.push_back(start_inf[i].order_);
                    t_o.push_back(tar_inf[i].order_);
                }
                ++j;
            }
            ++i;
        }

    }
};

#endif //TICKETSYSTEM_SRC_TRAINSYSTEM_H
