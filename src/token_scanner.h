#ifndef TICKETSYSTEM_SRC_TokenScanner_H
#define TICKETSYSTEM_SRC_TokenScanner_H

#include <iostream>
#include <string>
#include <cstring>
#include "order_system.h"

class TokenScanner {
public:
    std::string tokens_;
    bool hasMore_ = true;
    int point_ = 0;

    TokenScanner() = default;

    explicit TokenScanner(const std::string &s) {
        tokens_ = s;
    }

    std::string nextToken() { // 空格分隔下一个字段
        while (tokens_[point_] == ' ' and point_ != tokens_.length()) ++point_;
        if (point_ == tokens_.length()) return "";
        int begin = point_;
        int end = point_;
        while (tokens_[end] != ' ' and end != tokens_.length()) ++end;
        std::string tmp;
        tmp = tokens_.substr(begin, end - begin);
        point_ = end;
        while (tokens_[point_] == ' ' and point_ != tokens_.length()) ++point_;
        if (point_ == tokens_.length()) hasMore_ = false;
        return tmp;
    }

    std::string nextKey() { // |分隔下一个key
        int begin = point_;
        int end = point_;
        while (tokens_[end] != '|' and end != tokens_.length()) ++end;
        std::string tmp;
        tmp = tokens_.substr(begin, end - begin);
        point_ = end;
        while (tokens_[point_] == '|' and point_ != tokens_.length()) ++point_;
        if (point_ == tokens_.length()) hasMore_ = false;
        return tmp;
    }

    /*
    * UserSystem
    */

    void AddUser(UserSystem &user_system) {
        std::string tmp = nextToken();
        char cur_user[21], new_user[21];
        memset(cur_user, 0, sizeof(cur_user));
        memset(new_user, 0, sizeof(new_user));
        User new_inf;
        while (!tmp.empty()) {
            if (tmp == "-c") {
                tmp = nextToken();
                strcpy(cur_user, tmp.c_str());
            } else if (tmp == "-u") {
                tmp = nextToken();
                strcpy(new_user, tmp.c_str());
                strcpy(new_inf.username_, tmp.c_str());
            } else if (tmp == "-p") {
                tmp = nextToken();
                strcpy(new_inf.password_, tmp.c_str());
            } else if (tmp == "-n") {
                tmp = nextToken();
                strcpy(new_inf.name_, tmp.c_str());
            } else if (tmp == "-m") {
                tmp = nextToken();
                strcpy(new_inf.mail_addr_, tmp.c_str());
            } else if (tmp == "-g") {
                tmp = nextToken();
                if (tmp.length() > 1) new_inf.privilege_ = 10;
                else new_inf.privilege_ = tmp[0] - '0';
            }
            tmp = nextToken();
        }
        if (user_system.amount) {
            std::cout << user_system.AddUser(cur_user, new_user, new_inf) << '\n';
        } else {
            new_inf.privilege_ = 10;
            std::cout << user_system.AddFirstUser(new_user, new_inf) << '\n';
        }
    }

    void Login(UserSystem &user_system) {
        std::string tmp = nextToken();
        char user[21], password[31];
        memset(user, 0, sizeof(user));
        memset(password, 0, sizeof(password));
        while (!tmp.empty()) {
            if (tmp == "-u") {
                tmp = nextToken();
                strcpy(user, tmp.c_str());
            } else if (tmp == "-p") {
                tmp = nextToken();
                strcpy(password, tmp.c_str());
            }
            tmp = nextToken();
        }
        std::cout << user_system.Login(user, password) << '\n';
    }

    void Logout(UserSystem &user_system) {
        std::string tmp = nextToken();
        char user[21];
        memset(user, 0, sizeof(user));
        while (!tmp.empty()) {
            if (tmp == "-u") {
                tmp = nextToken();
                strcpy(user, tmp.c_str());
            }
            tmp = nextToken();
        }
        std::cout << user_system.Logout(user) << '\n';
    }

    void QueryProfile(UserSystem &user_system) {
        std::string tmp = nextToken();
        char cur_user[21], q_user[21];
        memset(cur_user, 0, sizeof(cur_user));
        memset(q_user, 0, sizeof(q_user));
        while (!tmp.empty()) {
            if (tmp == "-c") {
                tmp = nextToken();
                strcpy(cur_user, tmp.c_str());
            } else if (tmp == "-u") {
                tmp = nextToken();
                strcpy(q_user, tmp.c_str());
            }
            tmp = nextToken();
        }
        user_system.QueryProfile(cur_user, q_user);
    }

    void ModifyProfile(UserSystem &user_system) {
        std::string tmp = nextToken();
        bool if_modify[4];
        memset(if_modify, false, sizeof(bool) * 4);
        User m_inf;
        char cur_user[21], m_user[21];
        memset(cur_user, 0, sizeof(cur_user));
        memset(m_user, 0, sizeof(m_user));
        while (!tmp.empty()) {
            if (tmp == "-c") {
                tmp = nextToken();
                strcpy(cur_user, tmp.c_str());
            } else if (tmp == "-u") {
                tmp = nextToken();
                strcpy(m_user, tmp.c_str());
            } else if (tmp == "-p") {
                tmp = nextToken();
                strcpy(m_inf.password_, tmp.c_str());
                if_modify[0] = true;
            } else if (tmp == "-n") {
                tmp = nextToken();
                strcpy(m_inf.name_, tmp.c_str());
                if_modify[1] = true;
            } else if (tmp == "-m") {
                tmp = nextToken();
                strcpy(m_inf.mail_addr_, tmp.c_str());
                if_modify[2] = true;
            } else if (tmp == "-g") {
                tmp = nextToken();
                if (tmp.length() > 1) m_inf.privilege_ = 10;
                else m_inf.privilege_ = (short) (tmp[0] - '0');
                if_modify[3] = true;
            }
            tmp = nextToken();
        }
        User pre_inf;
        if (user_system.ModifyProfile(cur_user, m_user, m_inf.privilege_, pre_inf)) {
            if (if_modify[0]) {
                strcpy(pre_inf.password_, m_inf.password_);
            }
            if (if_modify[1]) {
                strcpy(pre_inf.name_, m_inf.name_);
            }
            if (if_modify[2]) {
                strcpy(pre_inf.mail_addr_, m_inf.mail_addr_);
            }
            if (if_modify[3]) {
                pre_inf.privilege_ = m_inf.privilege_;
            }
            user_system.writeFile(pre_inf, pre_inf.tag_);
            std::cout << pre_inf.username_ << ' ' << pre_inf.name_ << ' ' << pre_inf.mail_addr_ << ' '
                      << pre_inf.privilege_ << '\n';
        } else {
            std::cout << "-1\n";
        }
    }

    /*
     * TrainSystem
     */

    void AddTrain(TrainSystem &train_system) {
        std::string tmp = nextToken();
        sjtu::vector<int> travel_times;
        sjtu::vector<int> stopover_times;

        Train new_inf;
        while (!tmp.empty()) {
            if (tmp == "-i") {
                tmp = nextToken();
                strcpy(new_inf.trainID_, tmp.c_str());
            } else if (tmp == "-n") {
                tmp = nextToken();
                new_inf.station_num_ = atoi(tmp.c_str());
            } else if (tmp == "-m") {
                tmp = nextToken();
                new_inf.seat_num_ = atoi(tmp.c_str());
            } else if (tmp == "-s") {
                int i = 0;
                tmp = nextToken();
                TokenScanner scanner_key(tmp);
                std::string key = scanner_key.nextKey();
                while (!key.empty()) {
                    strcpy(new_inf.stations_[i], key.c_str());
                    key = scanner_key.nextKey();
                    ++i;
                }
            } else if (tmp == "-p") {
                int i = 1;
                tmp = nextToken();
                TokenScanner scanner_key(tmp);
                std::string key = scanner_key.nextKey();
                new_inf.prices_[0] = atoi(key.c_str());
                key = scanner_key.nextKey();
                while (!key.empty()) {
                    new_inf.prices_[i] = atoi(key.c_str()) + new_inf.prices_[i - 1];
                    key = scanner_key.nextKey();
                    ++i;
                }
            } else if (tmp == "-x") {
                tmp = nextToken();
                new_inf.leave_time_[0] = tmp;
            } else if (tmp == "-t") {
                tmp = nextToken();
                TokenScanner scanner_key(tmp);
                std::string key = scanner_key.nextKey();
                while (!key.empty()) {
                    travel_times.push_back(atoi(key.c_str()));
                    key = scanner_key.nextKey();
                }
            } else if (tmp == "-o") {
                tmp = nextToken();
                if (tmp == "_") continue;
                TokenScanner scanner_key(tmp);
                std::string key = scanner_key.nextKey();
                while (!key.empty()) {
                    stopover_times.push_back(atoi(key.c_str()));
                    key = scanner_key.nextKey();
                }
            } else if (tmp == "-d") {
                tmp = nextToken();
                TokenScanner scanner_key(tmp);
                std::string key = scanner_key.nextKey();
                new_inf.start_date_ = key;
                key = scanner_key.nextKey();
                new_inf.end_date_ = key;
            } else if (tmp == "-y") {
                tmp = nextToken();
                new_inf.type_ = tmp[0];
            }
            tmp = nextToken();
        }
        if (train_system.train_map_.find(new_inf.trainID_) != -1) std::cout << "-1\n";
        else {
            int stop = new_inf.station_num_ - 2;
            for (int i = 0; i < stop; ++i) {
                new_inf.arrive_time_[i] = new_inf.leave_time_[i] + travel_times[i];
                new_inf.leave_time_[i + 1] = new_inf.arrive_time_[i] + stopover_times[i];
            }
            new_inf.arrive_time_[stop] = new_inf.leave_time_[stop] + travel_times[stop];

            train_system.AddTrain(new_inf);
        }
    }

    void DeleteTrain(TrainSystem &train_system) {
        std::string tmp = nextToken();
        char trainID[21];
        memset(trainID, 0, sizeof(trainID));
        while (!tmp.empty()) {
            if (tmp == "-i") {
                tmp = nextToken();
                strcpy(trainID, tmp.c_str());
            }
            tmp = nextToken();
        }
        std::cout << train_system.DeleteTrain(trainID) << '\n';
    }

    void ReleaseTrain(TrainSystem &train_system) {
        std::string tmp = nextToken();
        char trainID[21];
        memset(trainID, 0, sizeof(trainID));

        tmp = nextToken();
        strcpy(trainID, tmp.c_str());

        std::cout << train_system.ReleaseTrain(trainID) << '\n';
    }

    void QueryTrain(TrainSystem &train_system) {
        std::string tmp = nextToken();
        char trainID[21];
        sjtu::Date date;
        memset(trainID, 0, sizeof(trainID));
        while (!tmp.empty()) {
            if (tmp == "-i") {
                tmp = nextToken();
                strcpy(trainID, tmp.c_str());
            } else if (tmp == "-d") {
                tmp = nextToken();
                date = tmp;
            }
            tmp = nextToken();
        }
        train_system.QueryTrain(trainID, date);
    }

    void QueryTicket(TrainSystem &train_system) {
        std::string tmp = nextToken();
        char start[31], target[31];
        sjtu::Date date;
        memset(start, 0, sizeof(start));
        memset(target, 0, sizeof(target));
        bool p = false;
        while (!tmp.empty()) {
            if (tmp == "-s") {
                tmp = nextToken();
                strcpy(start, tmp.c_str());
            } else if (tmp == "-t") {
                tmp = nextToken();
                strcpy(target, tmp.c_str());
            } else if (tmp == "-d") {
                tmp = nextToken();
                date = tmp;
            } else if (tmp == "-p") {
                tmp = nextToken();
                if (tmp == "time") p = false;
                else p = true;
            }
            tmp = nextToken();
        }
        train_system.QueryTicket(start, target, date, p);
    }

    void QueryTransfer(TrainSystem &train_system) {
        std::string tmp = nextToken();
        char start[31], target[31];
        sjtu::Date date;
        memset(start, 0, sizeof(start));
        memset(target, 0, sizeof(target));
        bool p;
        while (!tmp.empty()) {
            if (tmp == "-s") {
                tmp = nextToken();
                strcpy(start, tmp.c_str());
            } else if (tmp == "-t") {
                tmp = nextToken();
                strcpy(target, tmp.c_str());
            } else if (tmp == "-d") {
                tmp = nextToken();
                date = tmp;
            } else if (tmp == "-p") {
                tmp = nextToken();
                if (tmp == "time") p = false;
                else p = true;
            }
            tmp = nextToken();
        }
        train_system.QueryTransfer(start, target, date, p);
    }

    /*
     * OrderSystem
     */

    void BuyTicket(UserSystem &user_system, TrainSystem &train_system, OrderSystem &order_system) {
        std::string tmp = nextToken();
        char username[21], trainID[21];
        char from[31], to[31];
        sjtu::Date date;
        int num;
        bool if_queue;
        memset(username, 0, sizeof(username));
        memset(trainID, 0, sizeof(trainID));
        memset(from, 0, sizeof(from));
        memset(to, 0, sizeof(to));
        while (!tmp.empty()) {
            if (tmp == "-u") {
                tmp = nextToken();
                strcpy(username, tmp.c_str());
            } else if (tmp == "-i") {
                tmp = nextToken();
                strcpy(trainID, tmp.c_str());
            } else if (tmp == "-d") {
                tmp = nextToken();
                date = tmp;
            } else if (tmp == "-n") {
                tmp = nextToken();
                num = atoi(tmp.c_str());
            } else if (tmp == "-f") {
                tmp = nextToken();
                strcpy(from, tmp.c_str());
            } else if (tmp == "-t") {
                tmp = nextToken();
                strcpy(to, tmp.c_str());
            } else if (tmp == "-q") {
                tmp = nextToken();
                if (tmp == "true") if_queue = true;
                else if_queue = false;
            }
            tmp = nextToken();
        }
        order_system.BuyTicket(username, trainID, from, to, date, num, if_queue, user_system, train_system);
    }

    void QueryOrder(UserSystem &user_system, TrainSystem &train_system, OrderSystem &order_system) {
        std::string tmp = nextToken();
        tmp = nextToken();
        char username[21];
        memset(username, 0, sizeof(username));
        strcpy(username, tmp.c_str());

        order_system.QueryOrder(username, user_system, train_system);
    }

    void RefundTicket(UserSystem &user_system, TrainSystem &train_system, OrderSystem &order_system) {
        std::string tmp = nextToken();
        char username[21];
        memset(username, 0, sizeof(username));
        int order = 1;
        while (!tmp.empty()) {
            if (tmp == "-u") {
                tmp = nextToken();
                strcpy(username, tmp.c_str());
            } else if (tmp == "-n") {
                tmp = nextToken();
                order = atoi(tmp.c_str());
            }
            tmp = nextToken();
        }
        order_system.RefundTicket(username, order, user_system, train_system);
    }
};

#endif //TICKETSYSTEM_SRC_TokenScanner_H
