#ifndef TICKETSYSTEM_SRC_ORDER_SYSTEM_H
#define TICKETSYSTEM_SRC_ORDER_SYSTEM_H

#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
#include "user_system.h"
#include "train_system.h"
#include "schedule.h"
#include "String.h"
#include "multi_object_bpt.h"

struct Order {
public:
    int tag_ = -1;
    char trainID_[21];
    char from_[31];
    char to_[31];
    int from_o_;
    int to_o_;
    int status_; // 0-success, 1-pending, 3-refunded
    int price_;
    int num_;
//    sjtu::Date leave_date_;
//    sjtu::Date arrive_date_;
    sjtu::Time leave_time_;
    sjtu::Time arrive_time_;
    sjtu::Date departure_date_;

    Order() {
        memset(trainID_, 0, sizeof(trainID_));
        memset(from_, 0, sizeof(from_));
        memset(to_, 0, sizeof(to_));
    }
};

struct OrderSystem {
public:
    int amount_ = 0;
    MultiBPT<sjtu::String<21>, int> order_map_;
    MultiBPT<TrainSystem::SeatIndex, int> order_queue_; // seatIndex 同样也是date加trainID的配置，不用新设结构体了
    std::fstream order_inf_;
    std::string filename_ = "order_inf";

    OrderSystem() : order_map_("order_tree", "order_record", "order_tag"),
                    order_queue_("queue_tree", "queue_record", "queue_tag") {
        order_inf_.open(filename_);
        if (!order_inf_.good()) { // 是否成功打开
            order_inf_.open(filename_, std::fstream::out); // 新建
            order_inf_.close();
            order_inf_.open(filename_); // 变为可读可写
        } else {
            order_inf_.seekg(0);
            order_inf_.read(reinterpret_cast <char *> (&amount_), sizeof(amount_));
        }

    }

    ~OrderSystem() {
        order_inf_.seekp(0);
        order_inf_.write(reinterpret_cast <char *> (&amount_), sizeof(amount_));
        order_inf_.close();
    }

    void readFile(Order &read, const int &location) {
        order_inf_.seekg(sizeof(Order) * location + sizeof(amount_));
        order_inf_.read(reinterpret_cast<char *>(&read), sizeof(Order));
    }

    void writeFile(Order &write, const int &location) {
        order_inf_.seekp(sizeof(Order) * location + sizeof(amount_));
        order_inf_.write(reinterpret_cast<char *>(&write), sizeof(Order));
    }

    void BuyTicket(const char(&username)[21], const char(&trainID)[21], const char(&from)[31], const char (&to)[31],
                   const sjtu::Date date, int num, bool if_queue, UserSystem &userSystem, TrainSystem &trainSystem) {
        auto user_find = userSystem.user_login_.find(username);
        if (user_find == userSystem.user_login_.end() || user_find->second == -1) { // 当前用户未登录
            std::cout << "-1\n";
            return;
        }

        int train_tag = trainSystem.train_map_.find(trainID);
        Train train;
        trainSystem.readTrainFile(train, train_tag);
        if (!train.release_) {
            std::cout << "-1\n";
            return;
        }

        int from_o = -1, to_o = -1;
        for (int i = 0; i < train.station_num_; i++) {
            if (!strcmp(from, train.stations_[i])) {
                from_o = i;
            } else if (!strcmp(to, train.stations_[i])) {
                to_o = i;
                break;
            }
        }
        if (from_o == -1 || to_o == -1) {
            std::cout << "-1\n";
            return;
        }


        int day = train.leave_time_[from_o].hour_ / 24;
        sjtu::Date departure_date = date;
        departure_date -= day;
        if (departure_date < train.start_date_ || train.end_date_ < departure_date) {
            std::cout << "-1\n";
            return;
        }
        TrainSystem::SeatIndex seat_index(departure_date, trainID);
        int seat_tag = trainSystem.seat_map_.find(seat_index);
        TrainSystem::Seat seat;
        trainSystem.readSeatFile(seat, seat_tag);

        int empty_seat = 0x7fffffff;
        for (int i = from_o; i < to_o; i++) empty_seat = std::min(empty_seat, seat.seat_[i]);
        if (empty_seat < num && !if_queue || num > train.seat_num_) {
            std::cout << "-1\n";
            return;
        }
        Order order;
        order.tag_ = amount_++;
        strcpy(order.trainID_, trainID);
        strcpy(order.from_, from);
        strcpy(order.to_, to);
        order.price_ = train.prices_[to_o] - train.prices_[from_o];
        order.num_ = num;
        order.from_o_ = from_o;
        order.to_o_ = to_o;
        order.leave_time_ = train.leave_time_[from_o];
        order.arrive_time_ = train.arrive_time_[to_o - 1];
        order.departure_date_ = departure_date;

        if (empty_seat >= num) {
            order.status_ = 0;
            order_map_.insert(username, order.tag_);
            writeFile(order, order.tag_);
            for (int i = from_o; i < to_o; i++) seat.seat_[i] -= num;
            trainSystem.writeSeatFile(seat, seat.tag_);
            std::cout << (long long) num * order.price_ << '\n';
        } else {
            order.status_ = 1;
            order_map_.insert(username, order.tag_);
            writeFile(order, order.tag_);
            order_queue_.insert(seat_index, order.tag_);
            std::cout << "queue\n";
        }
    };

    void QueryOrder(const char(&username)[21], UserSystem &userSystem, TrainSystem &trainSystem) {
        auto user_find = userSystem.user_login_.find(username);
        if (user_find == userSystem.user_login_.end() || user_find->second == -1) { // 当前用户未登录
            std::cout << "-1\n";
            return;
        }

        sjtu::vector<int> record;
        order_map_.find(username, record);
        int size = record.size();
        std::cout << size << '\n';
        for (int i = size - 1; i >= 0; --i) {
            Order order;
            readFile(order, record[i]);
            if (!order.status_)
                std::cout << "[success] ";
            else if (order.status_ == 1)
                std::cout << "[pending] ";
            else
                std::cout << "[refunded] ";
            std::cout << order.trainID_ << ' ' << order.from_ << ' ';
            sjtu::Date date = order.departure_date_;
            sjtu::Time time = order.leave_time_;
            time.update(date, time);
            std::cout << date << ' ' << time << " -> " << order.to_ << ' ';
            date = order.departure_date_;
            time = order.arrive_time_;
            time.update(date, time);
            std::cout << date << ' ' << time << ' ' << order.price_ << ' ' << order.num_ << '\n';
        }
    }

    void RefundTicket(const char(&username)[21], int num, UserSystem &userSystem, TrainSystem &trainSystem) {
        auto user_find = userSystem.user_login_.find(username);
        if (user_find == userSystem.user_login_.end() || user_find->second == -1) { // 当前用户未登录
            std::cout << "-1\n";
            return;
        }

        sjtu::vector<int> record;
        order_map_.find(username, record);
        if (num > record.size()) {
            std::cout << "-1\n";
            return;
        }
        int tag = record[record.size() - num];
        Order order;
        readFile(order, tag);
        if (order.status_ == 2) {
            std::cout << "-1\n";
            return;
        }

        TrainSystem::SeatIndex seat_index;
        seat_index.date_ = order.departure_date_;
        seat_index.trainID_ = order.trainID_;
        if (order.status_ == 1) {
            order.status_ = 2;
            writeFile(order, order.tag_);
            order_queue_.erase(seat_index, order.tag_);
            std::cout << "0\n";
            return;
        }
        order.status_ = 2;
        writeFile(order, order.tag_);
        int seat_tag = trainSystem.seat_map_.find(seat_index);
        TrainSystem::Seat seat;
        trainSystem.readSeatFile(seat, seat_tag);
        for (int i = order.from_o_; i < order.to_o_; ++i) seat.seat_[i] += order.num_;
        sjtu::vector<int> re;
        order_queue_.find(seat_index, re);
        int size = re.size();
        for (int i = 0; i < size; ++i) {
            Order o;
            readFile(o, re[i]);
            int empty_seat = 0x7fffffff;
            for (int j = o.from_o_; j < o.to_o_; j++) empty_seat = std::min(empty_seat, seat.seat_[j]);
            if (empty_seat < o.num_) continue;
            o.status_ = 0;
            for (int j = o.from_o_; j < o.to_o_; j++) seat.seat_[j] -= o.num_;
            order_queue_.erase(seat_index, o.tag_);
            writeFile(o, re[i]);
        }
        trainSystem.writeSeatFile(seat, seat_tag);
        std::cout << "0\n";
    }

    void clean() {
        amount_ = 0;
        order_map_.clear();
        order_queue_.clear();
    }
};

#endif //TICKETSYSTEM_SRC_ORDER_SYSTEM_H
