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
        int tag_ = -1;
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

    TrainSystem() : train_map_("trainID_tree", "trainID_record", "train_tag"),
                    seat_map_("seat_tree", "seat_record", "seat_tag"),
                    station_map_("station_tree", "station_record", "station_tag") {
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
            seat_inf_.read(reinterpret_cast <char *> (&amount_seat_), sizeof(amount_seat_));

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
            seat.tag_ = tag_s;
            seat_map_.insert(seat_index, tag_s);
            writeSeatFile(seat, tag_s);
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
        int seat_num_;
        int prices_;
        int time_;
        sjtu::Date leave_date_;
        sjtu::Date arrive_date_;
        sjtu::Time leave_time_;
        sjtu::Time arrive_time_;

        QueryData() {
            memset(trainID_, 0, sizeof(trainID_));
        }
    };

    struct TransferData {
        char trainID1_[21];
        char trainID2_[21];
        int tag1_;
        int tag2_;
        int time_ = 0x7fffffff;
        int price_ = 0x7fffffff;
        int start1_;
        int target1_;
        int start2_;
        int target2_;
        sjtu::Date transfer_date_;
        sjtu::Time transfer_time_;
        sjtu::Date departure_date2_;

        TransferData() {
            memset(trainID1_, 0, sizeof(trainID1_));
            memset(trainID2_, 0, sizeof(trainID2_));
        }

        TransferData &operator=(const TransferData &rhs) {
            if (this == &rhs) return *this;
            strcpy(trainID1_, rhs.trainID1_);
            strcpy(trainID2_, rhs.trainID2_);
            tag1_ = rhs.tag1_;
            tag2_ = rhs.tag2_;
            time_ = rhs.time_;
            price_ = rhs.price_;
            start1_ = rhs.start1_;
            target1_ = rhs.target1_;
            start2_ = rhs.start2_;
            target2_ = rhs.target2_;
            transfer_date_ = rhs.transfer_date_;
            transfer_time_ = rhs.transfer_time_;
            departure_date2_ = rhs.departure_date2_;
            return *this;
        }

        bool time_first(const TransferData &rhs) {
            if (time_ != rhs.time_) return time_ < rhs.time_;
            if (price_ != rhs.price_) return price_ < rhs.price_;
            auto cmp = strcmp(trainID1_, rhs.trainID1_);
            if (cmp != 0) return cmp < 0;
            return strcmp(trainID2_, rhs.trainID2_) < 0;
        }

        bool price_first(const TransferData &rhs) {
            if (price_ != rhs.price_) return price_ < rhs.price_;
            if (time_ != rhs.time_) return time_ < rhs.time_;
            auto cmp = strcmp(trainID1_, rhs.trainID1_);
            if (cmp != 0) return cmp < 0;
            return strcmp(trainID2_, rhs.trainID2_) < 0;
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
            if (start_o[i] == train.station_num_ - 1) continue;
            sjtu::Time leave_time = train.leave_time_[start_o[i]];
            int day = leave_time.hour_ / 24;
            sjtu::Date departure_date = date;
            departure_date -= day;
            if (departure_date < train.start_date_ || train.end_date_ < departure_date) continue;

            strcpy(query_data.trainID_, train.trainID_);
            query_data.seat_num_ = train.seat_num_;
            query_data.prices_ = train.prices_[target_o[i]] - train.prices_[start_o[i]];
            query_data.time_ = train.arrive_time_[target_o[i] - 1] - leave_time;
            query_data.leave_date_ = date;
            leave_time.hour_ %= 24;
            query_data.leave_time_ = leave_time;
            query_data.arrive_date_ = departure_date;
            query_data.arrive_time_ = train.arrive_time_[target_o[i] - 1];
            query_data.arrive_time_.update(query_data.arrive_date_, query_data.arrive_time_);
            SeatIndex index;
            index.date_ = departure_date;
            index.trainID_ = train.trainID_;
            int tag = seat_map_.find(index);
            Seat seat;
            readSeatFile(seat, tag);
            for (int j = start_o[i]; j < target_o[i]; j++)
                query_data.seat_num_ = std::min(query_data.seat_num_, seat.seat_[j]);

            record.push_back(query_data);
        }

        size = record.size();
        if (!size) {
            std::cout << "0\n";
            return;
        }
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
        TransferData transfer;
        if (!FindTransfer(start, target, date, p, transfer)) {
            std::cout << "0\n";
            return;
        }

        Train train1, train2;
        readTrainFile(train1, transfer.tag1_);
        readTrainFile(train2, transfer.tag2_);

        QueryData s, t;
        s.seat_num_ = train1.seat_num_;
        s.prices_ = train1.prices_[transfer.target1_] - train1.prices_[transfer.start1_];
        s.leave_date_ = date;
        s.leave_time_ = train1.leave_time_[transfer.start1_];
        s.leave_time_.hour_ %= 24;
        s.arrive_date_ = transfer.transfer_date_;
        s.arrive_time_ = transfer.transfer_time_;
        SeatIndex index1;
        sjtu::Date departure_date = date;
        departure_date -= train1.leave_time_[transfer.start1_].hour_ / 24;
        index1.date_ = departure_date;
        index1.trainID_ = transfer.trainID1_;
        int tag = seat_map_.find(index1);
        Seat seat;
        readSeatFile(seat, tag);
        for (int j = transfer.start1_; j < transfer.target1_; j++)
            s.seat_num_ = std::min(s.seat_num_, seat.seat_[j]);

        t.seat_num_ = train2.seat_num_;
        t.prices_ = train2.prices_[transfer.target2_] - train2.prices_[transfer.start2_];
        t.leave_date_ = transfer.departure_date2_;
        t.leave_time_ = train2.leave_time_[transfer.start2_];
        t.leave_time_.update(t.leave_date_, t.leave_time_);
        t.arrive_date_ = transfer.departure_date2_;
        t.arrive_time_ = train2.arrive_time_[transfer.target2_ - 1];
        t.arrive_time_.update(t.arrive_date_, t.arrive_time_);
        SeatIndex index2;
        index2.date_ = transfer.departure_date2_;
        index2.trainID_ = transfer.trainID2_;
        tag = seat_map_.find(index2);
        readSeatFile(seat, tag);
        for (int j = transfer.start2_; j < transfer.target2_; j++)
            t.seat_num_ = std::min(t.seat_num_, seat.seat_[j]);
        std::cout << transfer.trainID1_ << ' ' << start << ' ' << s.leave_date_ << ' '
                  << s.leave_time_ << " -> " << train1.stations_[transfer.target1_] << ' ' << s.arrive_date_ << ' '
                  << s.arrive_time_ << ' ' << s.prices_ << ' ' << s.seat_num_ << '\n';

        std::cout << transfer.trainID2_ << ' ' << train1.stations_[transfer.target1_] << ' ' << t.leave_date_ << ' '
                  << t.leave_time_ << " -> " << target << ' ' << t.arrive_date_ << ' '
                  << t.arrive_time_ << ' ' << t.prices_ << ' ' << t.seat_num_ << '\n';

    }


    void clean() {
        train_map_.clear();
        seat_map_.clear();
        station_map_.clear();
        amount_train_ = 0;
        amount_seat_ = 0;
        storage_of_train_tag_.clear();
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
            while (i != start_size && start_tag[i] < tar_tag[j]) ++i;
            if (i == start_size) return;
            if (start_tag[i] == tar_tag[j]) {
                if (start_inf[i].order_ < tar_inf[j].order_) {
                    satisfied_train.push_back(start_tag[i]);
                    s_o.push_back(start_inf[i].order_);
                    t_o.push_back(tar_inf[j].order_);
                }
                ++i;
            }
            ++j;
            if (i == start_size) return;
            while (j != tar_size && tar_tag[j] < start_tag[i]) ++j;
            if (j == tar_size) return;
            if (start_tag[i] == tar_tag[j]) {
                if (start_inf[i].order_ < tar_inf[j].order_) {
                    satisfied_train.push_back(start_tag[i]);
                    s_o.push_back(start_inf[i].order_);
                    t_o.push_back(tar_inf[j].order_);
                }
                ++j;
            }
            ++i;
            if (j == tar_size) return;
        }
    }

    bool
    FindTransfer(const char(&start)[31], const char (&target)[31], sjtu::Date &date, bool p, TransferData &transfer) {
        sjtu::vector<int> start_tag, tar_tag;
        sjtu::vector<StationIndex> start_inf, tar_inf;

        StationIndex s(start, 0);
        station_map_.find_o(s, start_tag, start_inf);
        if (start_tag.empty()) return false;
        StationIndex t(target, 0);
        station_map_.find_o(t, tar_tag, tar_inf);
        if (tar_tag.empty()) return false;

        int start_size = start_tag.size(), tar_size = tar_tag.size();

        sjtu::map<sjtu::String<31>, sjtu::vector<TransferData>> destination;
        bool flag = false;
        for (int i = 0; i < start_size; ++i) {
            int k = start_inf[i].order_;

            Train train;
            readTrainFile(train, start_tag[i]);
            sjtu::Time leave_time1 = train.leave_time_[k];
            int day = leave_time1.hour_ / 24;
            sjtu::Date departure_date1 = date;
            departure_date1 -= day;
            if (departure_date1 < train.start_date_ || train.end_date_ < departure_date1) continue;

            TransferData tmp;
            strcpy(tmp.trainID1_, train.trainID_);
            tmp.tag1_ = start_tag[i];
            tmp.time_ = (train.leave_time_[k].hour_ % 24) * 60 + train.leave_time_[k].minute_;
            for (int j = k + 1; j < train.station_num_; ++j) {
                if (strcmp(train.stations_[j], target) == 0) continue;

                flag = true;
                auto find = destination.find(train.stations_[j]);
                if (find == destination.end()) {
                    sjtu::vector<TransferData> v;
                    tmp.transfer_time_ = train.arrive_time_[j - 1];
                    tmp.transfer_date_ = departure_date1;
                    tmp.transfer_time_.update(tmp.transfer_date_, tmp.transfer_time_);
                    tmp.price_ = train.prices_[j] - train.prices_[k];
                    tmp.start1_ = k;
                    tmp.target1_ = j;
                    v.push_back(tmp);
                    destination[train.stations_[j]] = v;
                } else {
                    tmp.transfer_time_ = train.arrive_time_[j - 1];
                    tmp.transfer_date_ = departure_date1;
                    tmp.transfer_time_.update(tmp.transfer_date_, tmp.transfer_time_);
                    tmp.price_ = train.prices_[j] - train.prices_[k];
                    tmp.start1_ = k;
                    tmp.target1_ = j;
                    destination[train.stations_[j]].push_back(tmp);
                }
            }
        }
        if (!flag) return false;
        else flag = false;
        for (int i = 0; i < tar_size; ++i) {
            int h = tar_inf[i].order_;

            Train train;
            readTrainFile(train, tar_tag[i]);
            sjtu::Time arrive_time2 = train.arrive_time_[h - 1];
            int day = arrive_time2.hour_ / 24;
            sjtu::Date departure_date2_k = date;
            departure_date2_k -= day;
            if (train.end_date_ < departure_date2_k) continue;

            departure_date2_k = train.end_date_;
            sjtu::Date arrive_date2_k = train.end_date_;
            arrive_time2.update(arrive_date2_k, arrive_time2);
            for (int j = h - 1; j >= 0; --j) {
                if (strcmp(train.stations_[j], start) == 0) continue;
                auto find = destination.find(train.stations_[j]);
                if (find == destination.end()) {
                    continue;
                } else {
                    sjtu::vector<TransferData> &v = destination[train.stations_[j]];
                    int size = v.size();

                    for (int k = 0; k < size; ++k) {
                        TransferData tmp = v[k];
                        if (train.tag_ == tmp.tag1_) continue;
                        sjtu::Date departure_date2=departure_date2_k;
                        sjtu::Date arrive_date2=arrive_date2_k;
                        sjtu::Time leave_time2 = train.leave_time_[j];
                        sjtu::Date leave_date2 = train.end_date_;
                        leave_time2.update(leave_date2, leave_time2);
                        if (leave_date2 < tmp.transfer_date_ ||
                            leave_date2 == tmp.transfer_date_ && leave_time2 < tmp.transfer_time_)
                            continue;

                        flag = true;
                        int distance;
                        if (leave_time2 < tmp.transfer_time_) {
                            if (tmp.transfer_date_ + 1 < leave_date2) {
                                distance = leave_date2 - tmp.transfer_date_ - 1;
                                int d = train.end_date_ - train.start_date_;
                                distance = distance < d ? distance : d;
                                leave_date2 -= distance;
                                departure_date2 -= distance;
                                arrive_date2 -= distance;
                            }
                        } else {
                            if (tmp.transfer_date_ < leave_date2) {
                                distance = leave_date2 - tmp.transfer_date_;
                                int d = train.end_date_ - train.start_date_;
                                distance = distance < d ? distance : d;
                                leave_date2 -= distance;
                                departure_date2 -= distance;
                                arrive_date2 -= distance;
                            }
                        }

                        strcpy(tmp.trainID2_, train.trainID_);
                        tmp.time_ = (arrive_time2.hour_ % 24) * 60 + arrive_time2.minute_ - tmp.time_;
                        tmp.time_ += 60 * 24 * (arrive_date2 - date);
                        tmp.price_ += train.prices_[h] - train.prices_[j];
                        if (!p) {
                            if (tmp.time_first(transfer)) {
                                tmp.tag2_ = tar_tag[i];
                                tmp.start2_ = j;
                                tmp.target2_ = h;
                                tmp.departure_date2_ = departure_date2;
                                transfer = tmp;
                            }
                        } else {
                            if (tmp.price_first(transfer)) {
                                tmp.tag2_ = tar_tag[i];
                                tmp.start2_ = j;
                                tmp.target2_ = h;
                                tmp.departure_date2_ = departure_date2;
                                transfer = tmp;
                            }
                        }
                    }
                }
            }
        }
        return flag;
    }

    void merge(int l, int mid, int r, int *tag, sjtu::vector<QueryData> &re, bool p) {
        int *tmp = new int[r - l + 10];
        int ll = l, rr = mid + 1;
        int order = 0;
        if (!p) {
            while (ll <= mid && rr <= r) {
                if (re[tag[ll]].time_ != re[tag[rr]].time_) {
                    if (re[tag[ll]].time_ < re[tag[rr]].time_) {
                        tmp[order] = tag[ll];
                        ++ll;
                        ++order;
                    } else {
                        tmp[order] = tag[rr];
                        ++rr;
                        ++order;
                    }
                } else {
                    if (strcmp(re[tag[ll]].trainID_, re[tag[rr]].trainID_) < 0) {
                        tmp[order] = tag[ll];
                        ++ll;
                        ++order;
                    } else {
                        tmp[order] = tag[rr];
                        ++rr;
                        ++order;
                    }
                }
            }
        } else {
            while (ll <= mid && rr <= r) {
                if (re[tag[ll]].prices_ != re[tag[rr]].prices_) {
                    if (re[tag[ll]].prices_ < re[tag[rr]].prices_) {
                        tmp[order] = tag[ll];
                        ++ll;
                        ++order;
                    } else {
                        tmp[order] = tag[rr];
                        ++rr;
                        ++order;
                    }
                } else {
                    if (strcmp(re[tag[ll]].trainID_, re[tag[rr]].trainID_) < 0) {
                        tmp[order] = tag[ll];
                        ++ll;
                        ++order;
                    } else {
                        tmp[order] = tag[rr];
                        ++rr;
                        ++order;
                    }
                }
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
