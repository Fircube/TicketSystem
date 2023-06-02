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
#include "map.h"


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

        StationIndex() = default;

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
public:
    struct SeatIndex {

        sjtu::Date date_;
        sjtu::String<21> trainID_;

        SeatIndex() = default;

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
        int tag_;
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
//    sjtu::vector<int> storage_of_seat_tag_; // store unused tag;

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

//private:
    void readTrainFile(Train &read, const int &location) {
        train_inf_.seekg(sizeof(Train) * location + sizeof(amount_train_));
        train_inf_.read(reinterpret_cast<char *>(&read), sizeof(Train));
    }

    void writeTrainFile(Train &write, const int &location) {
        train_inf_.seekp(sizeof(Train) * location + sizeof(amount_train_));
        train_inf_.write(reinterpret_cast<char *>(&write), sizeof(Train));
    }

    void readSeatFile(Seat &read, const int &location) {
        seat_inf_.seekg(sizeof(Seat) * location + sizeof(amount_seat_));
        seat_inf_.read(reinterpret_cast<char *>(&read), sizeof(Seat));
    }

    void writeSeatFile(Seat &write, const int &location) {
        seat_inf_.seekp(sizeof(Seat) * location + sizeof(amount_seat_));
        seat_inf_.write(reinterpret_cast<char *>(&write), sizeof(Seat));
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
//        if (storage_of_seat_tag_.empty()) {
        return amount_seat_++;
//        } else {
//            int tag = storage_of_seat_tag_.back();
//            storage_of_seat_tag_.pop_back();
//            return tag;
//        }
    }

//    void RestoreSeatTag(int tag) {
//        storage_of_seat_tag_.push_back(tag);
//    }


public:
    void AddTrain(Train &new_inf) {
        int tag = AssignTrainTag();
        new_inf.tag_ = tag;
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
        return 0;
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

        SeatIndex seat_index(tmp.start_date_, trainID);
        Seat seat;
        for (int i = 0; i < tmp.station_num_ - 1; ++i) seat.seat_[i] = tmp.seat_num_;
        for (; seat_index.date_ <= tmp.end_date_; ++seat_index.date_) {
            int tag_s = AssignSeatTag();
            seat.tag_=tag_s;
            seat_map_.insert(seat_index, tag_s);
            writeSeatFile(seat,tag_s);
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
            SeatIndex seat_index(date, trainID);
            int tag_s = seat_map_.find(seat_index);
            Seat seat;
            readSeatFile(seat, tag_s);
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
    struct QueryData {
        char trainID_[21];
//        char start_[31];
//        char target_[31];
        int seat_num_;
        int prices_;
        int time_;
        sjtu::Date leave_date_;
        sjtu::Date arrive_date_;
        sjtu::Time leave_time_;
        sjtu::Time arrive_time_;

        QueryData() {
            memset(trainID_, 0, sizeof(trainID_));
//            memset(start_, 0, sizeof(start_));
//            memset(target_, 0, sizeof(target_));
        }
    };

public:
    void QueryTicket(const char(&start)[31], const char (&target)[31], sjtu::Date date, bool p) {
        sjtu::vector<int> satisfied_train;
        sjtu::vector<int> start_o;
        sjtu::vector<int> target_o;
        FindTicket(start, target, satisfied_train, start_o, target_o);
        int size = satisfied_train.size();
        sjtu::vector<QueryData> record;
        QueryData query_data;
        for (int i = 0; i < size; ++i) {
            Train train;
            readTrainFile(train, satisfied_train[i]);
//            if(start_o[i]==train.station_num_) continue;
            sjtu::Time leave_time = train.leave_time_[start_o[i]];
            int day = leave_time.hour_ / 24;
            sjtu::Date departure_date = date;
            departure_date -= day;
            if (departure_date < train.start_date_ || train.end_date_ < departure_date) continue;

            strcpy(query_data.trainID_, train.trainID_);
            query_data.seat_num_ = train.seat_num_;
            query_data.prices_ = train.prices_[target_o[i]] - train.prices_[start_o[i]];
            query_data.leave_date_ = date;
            query_data.leave_time_ = leave_time;
            query_data.arrive_date_ = departure_date;
            query_data.arrive_time_ = train.arrive_time_[start_o[i] - 1];
            query_data.arrive_time_.update(query_data.arrive_date_, query_data.arrive_time_);
            query_data.time_ = query_data.arrive_time_ - query_data.leave_time_;
            SeatIndex index;
            index.date_ = departure_date;
            index.trainID_ = train.trainID_;
            int tag = seat_map_.find(index);
            Seat seat;
            readSeatFile(seat, tag);
            for (int j = start_o[i]; j < target_o[i]; j++) {
                query_data.seat_num_ = std::min(query_data.seat_num_, seat.seat_[j]);
            }
            record.push_back(query_data);
        }

        size = record.size();
        int *sort_tag = new int[size];
        for (int i = 0; i < size; i++) sort_tag[i] = i;

        mergesort(0, size - 1, sort_tag, record, p);

        std::cout << size << '\n';
        for (int i = 0; i < size; i++) {
            int j = sort_tag[i];
            std::cout << record[j].trainID_ << ' ' << start << ' ' << record[j].leave_date_ << ' '
                      << record[j].leave_time_ << " -> " << target << ' ' << record[j].arrive_date_ << ' '
                      << record[j].arrive_time_ << ' ' << record[j].prices_ << ' ' << record[j].seat_num_ << '\n';
        }
        delete[] sort_tag;
    }

    void QueryTransfer(const char(&start)[31], const char (&target)[31], sjtu::Date date, bool p) {
        std::cout << "0\n";
//        sjtu::vector<int> satisfied_train;
//        sjtu::vector<int> start_o;
//        sjtu::vector<int> target_o;
////        FindTransfer(start, target, satisfied_train, start_o, target_o,date);
//        if (start_o.empty() || target_o.empty()) {
//            std::cout << "0\n";
//            return;
//        }
//
//
//
//        QueryData s,t;
//
//        std::cout << s.trainID_ << ' ' << start << ' ' << s.leave_date_ << ' '
//                  << s.leave_time_ << " -> " << target << ' ' << s.arrive_date_ << ' '
//                  << s.arrive_time_ << ' ' << s.prices_ << ' ' << s.seat_num_ << '\n';
//
//        std::cout << t.trainID_ << ' ' << start << ' ' << t.leave_date_ << ' '
//                  << t.leave_time_ << " -> " << target << ' ' << t.arrive_date_ << ' '
//                  << t.arrive_time_ << ' ' << t.prices_ << ' ' << t.seat_num_ << '\n';
    }



    void clean() {

    }

private:
    void FindTicket(const char(&start)[31], const char (&target)[31], sjtu::vector<int> &satisfied_train,
                    sjtu::vector<int> &s_o, sjtu::vector<int> &t_o) {
        sjtu::vector<int> start_tag, tar_tag;
        sjtu::vector<StationIndex> start_inf, tar_inf;
        StationIndex s(start, 0);
        station_map_.find_o(s, start_tag, start_inf);
        if (start_tag.empty()) return;
        StationIndex t(target, 0);
        station_map_.find_o(t, tar_tag, tar_inf);
        if (tar_tag.empty()) return;
        int start_size = start_tag.size(), tar_size = tar_tag.size();
        int i = 0, j = 0;
        while (i < start_size && j < tar_size) {
            while (i != start_size && start_tag[i] < tar_tag[i]) ++i;
            if (i == start_size) return;
            if (start_tag[i] == tar_tag[i]) {
                if (start_inf[i].order_ < tar_inf[i].order_) {
                    satisfied_train.push_back(start_tag[i]);
                    s_o.push_back(start_inf[i].order_);
                    t_o.push_back(tar_inf[i].order_);
                }
                ++i;
            }
            ++j;
            while (j != tar_size && tar_tag[i] < start_tag[i]) ++j;
            if (j == tar_size) return;
            if (start_tag[i] == tar_tag[i]) {
                if (start_inf[i].order_ < tar_inf[i].order_) {
                    satisfied_train.push_back(start_tag[i]);
                    s_o.push_back(start_inf[i].order_);
                    t_o.push_back(tar_inf[i].order_);
                }
                ++j;
            }
            ++i;
        }
    }

    void FindTransfer(const char(&start)[31], const char (&target)[31], sjtu::Date &date, bool p) {
        sjtu::vector<int> start_tag, tar_tag;
        sjtu::vector<StationIndex> start_inf, tar_inf;
        StationIndex s(start, 0);
        station_map_.find_o(s, start_tag, start_inf);
        if (start_tag.empty()) return;
        StationIndex t(target, 0);
        station_map_.find_o(t, tar_tag, tar_inf);
        if (tar_tag.empty()) return;
        int start_size = start_tag.size(), tar_size = tar_tag.size();

        sjtu::map<sjtu::String<31>, sjtu::vector<sjtu::pair<int, int>>> from_a;

        for (int i = 0; i < start_size; ++i) {
            Train train;
            readTrainFile(train, start_tag[i]);
            sjtu::Time leave_time = train.leave_time_[start_inf[i].order_];
            int day = leave_time.hour_ / 24;
            sjtu::Date departure_date = date;
            departure_date -= day;
            if (departure_date < train.start_date_ || train.end_date_ < departure_date) continue;


        }

        for (int i = 0; i < tar_size; ++i) {
            Train train;
            readTrainFile(train, tar_tag[i]);
            sjtu::Time leave_time = train.leave_time_[tar_inf[i].order_];
            int day = leave_time.hour_ / 24;
            sjtu::Date departure_date = date;
            departure_date -= day;
            if (departure_date < train.start_date_ || train.end_date_ < departure_date) continue;


        }
    }

    void merge(int l, int mid, int r, int *tag, sjtu::vector<QueryData> &re, bool p) {
        int *tmp = new int[r - l + 10];
        int ll = l, rr = mid + 1;
        int order = 0;
        if (!p) {
            while (ll <= mid && rr <= r) {
                if (re[tag[ll]].time_ < re[tag[rr]].time_) {
                    tmp[order] = tag[ll];
                    ++ll;
                    ++order;
                } else {
                    tmp[order] = tag[rr];
                    ++rr;
                    ++order;
                }
            }
        } else {
            if (re[tag[ll]].prices_ < re[tag[rr]].prices_) {
                tmp[order] = tag[ll];
                ++ll;
                ++order;
            } else {
                tmp[order] = tag[rr];
                ++rr;
                ++order;
            }
        }
        while (ll <= mid) {
            tmp[order] = tag[ll];
            ++ll;
            ++order;
        }
        while (rr <= r) {
            tmp[order] = tag[rr];
            ++rr;
            ++order;
        }
        for (int i = 0; i < order; ++i) {
            tag[l + i] = tmp[i];
        }
        delete[]tmp;
    }

    void mergesort(int l, int r, int *tag, sjtu::vector<QueryData> &re, bool p) {
        if (l == r) return;
        int mid = (l + r) >> 1;
        mergesort(l, mid, tag, re, p);
        mergesort(mid + 1, r, tag, re, p);
        merge(l, mid, r, tag, re, p);
    }
};

#endif //TICKETSYSTEM_SRC_TRAINSYSTEM_H
