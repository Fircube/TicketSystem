#ifndef TICKETSYSTEM_SRC_TokenScanner_H
#define TICKETSYSTEM_SRC_TokenScanner_H

#include <iostream>
#include <string>
#include <cstring>
#include "train_system.h"
#include "user_system.h"

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
//        char cur_user[21], new_user[21];
//        memset(cur_user, 0, sizeof(cur_user));
//        memset(new_user, 0, sizeof(new_user));
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
                int i = 0;
                tmp = nextToken();
                TokenScanner scanner_key(tmp);
                std::string key = scanner_key.nextKey();
                while (!key.empty()) {
                    new_inf.prices_[i] = atoi(key.c_str());
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
                if(tmp=="-") continue;
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
                new_inf.start_date_=key;
                key = scanner_key.nextKey();
                new_inf.end_date_=key;
            } else if (tmp == "-y") {
                tmp = nextToken();
                new_inf.type_ = tmp[0];
            }
            tmp = nextToken();
        }
        if(train_system.train_map_.find(new_inf.trainID_)!=-1) std::cout<<"-1\n";
        else{
            int stop=new_inf.station_num_-1;
            for(int i=0;i<stop;++i){
                new_inf.arrive_time_[i]=new_inf.leave_time_[i]+travel_times[i];
                new_inf.leave_time_[i+1]=new_inf.arrive_time_[i]+stopover_times[i];
            }
        }
    }
//    void show(UserSystem &user_system, TrainSystem &train_system) {
//        std::string tmp = nextToken();
//        if (tmp.empty()) {
//            train_system.show(user_system);
//            log_.AddLog2(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 6, " ");
//            return;
//        }
//        if (tmp.length() < 6) throw std::string("Invalid\n");
//
//        if (tmp[1] == 'I') {
//            char ISBN_[21];
//            memset(ISBN_, 0, sizeof(ISBN_));
//            strcpy(ISBN_, check_assign_ISBN(tmp).c_str());
//
//            if (hasMore_) throw std::string("Invalid\n");
//
//            train_system.ISBN_show(ISBN_, user_system);
//        } else {
//            char type[61];
//            memset(type, 0, sizeof(type));
//            strcpy(type, check_assign(tmp).c_str());
//
//            if (hasMore_) throw std::string("Invalid\n");
//
//            if (tmp[1] == 'n') {
//                train_system.BookName_show(type, user_system);
//            } else if (tmp[1] == 'a') {
//                train_system.Author_show(type, user_system);
//            } else if (tmp[1] == 'k') {
//                train_system.Keyword_show(type, user_system);
//            } else throw std::string("Invalid\n");
//        }
//
//        log_.AddLog2(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 6, tmp.c_str());
//    }
//
//    void buy(UserSystem &user_system, TrainSystem &train_system) {
//        std::string tmp = nextToken();
//        check_Type4(tmp);
//        char ISBN_[21];
//        memset(ISBN_, 0, sizeof(ISBN_));
//        strcpy(ISBN_, tmp.c_str());
//
//        tmp = nextToken();
//        int Quantity_ = check_Type6(tmp);
//        double cost = 0.0;
//
//        train_system.buy(ISBN_, Quantity_, user_system, cost);
//
//        log_.AddFinance(true, cost);
//
//        log_.AddLog3(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 7, ISBN_, Quantity_,
//                     cost);
//    }
//
//    void select(UserSystem &user_system, TrainSystem &train_system) {
//        if (user_system.login_stack.top().Privilege < '3') throw std::string("Invalid\n");
//
//        std::string tmp = nextToken();
//        check_Type4(tmp);
//        char ISBN_[21];
//        memset(ISBN_, 0, sizeof(ISBN_));
//        strcpy(ISBN_, tmp.c_str());
//
//        train_system.select(ISBN_, user_system);
//
//        log_.AddLog4(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 8, ISBN_);
//    }
//
//    void modify(UserSystem &user_system, TrainSystem &train_system) {
//        if (user_system.login_stack.top().Privilege < '3') throw std::string("Invalid\n");
//        int index_ = user_system.User_select.top();
//        if (index_ == -1) throw std::string("Invalid\n");
//        if (!hasMore_) throw std::string("Invalid\n");
//
//        std::string tmp = nextToken();
//        bool used[5];
//        char ISBN_[21];
//        char BookName_[61];
//        char Author_[61];
//        char Keyword_[61];
//        double Price_ = 0.0;
//        std::string Price;
//        std::string Key;
//        std::unordered_map<std::string, bool> hasKey; // 是否修改过
//        std::vector<std::string> Key_store; // 先存起来，保证没错误再改变
//
//        memset(used, false, sizeof(used));
//        hasKey.clear();
//        Key_store.clear();
//        while (!tmp.empty()) {
//            if (tmp.length() < 6) throw std::string("Invalid\n");
//            if (tmp[1] == 'I') {
//                if (used[0]) throw std::string("Invalid\n");
//
//                memset(ISBN_, 0, sizeof(ISBN_));
//                strcpy(ISBN_, check_assign_ISBN(tmp).c_str());
//                int index = train_system.ISBN_map.search(ISBN_);
//                if (index != -1) throw std::string("Invalid\n");
//
//                used[0] = true;
//            } else if (tmp[1] == 'k') {
//                if (used[3]) throw std::string("Invalid\n");
//
//                memset(Keyword_, 0, sizeof(Keyword_));
//                if (tmp.length() < 11) throw std::string("Invalid\n");
//                std::string temp = tmp.substr(1, 9);
//                if (temp != "keyword=\"" || tmp[tmp.size() - 1] != '\"') throw std::string("Invalid\n");
//                temp = tmp.substr(10, tmp.length() - 11);
//                check_key(temp);
//                strcpy(Keyword_, temp.c_str());
//                Key = temp;
//
//                // 拆分keyword
//                TokenScanner scanner_key(temp);
//                std::string key = scanner_key.nextKey();
//                while (!key.empty()) {
//                    if (hasKey.count(key)) throw std::string("Invalid\n");
//                    hasKey[key] = true;
//                    Key_store.push_back(key);
//                    key = scanner_key.nextKey();
//                }
//
//                used[3] = true;
//            } else {
//                char type[61];
//                memset(type, 0, sizeof(type));
//                strcpy(type, check_assign(tmp).c_str());
//
//                if (tmp[1] == 'n') {
//                    if (used[1]) throw std::string("Invalid\n");
//                    used[1] = true;
//                    memset(BookName_, 0, sizeof(BookName_));
//                    strcpy(BookName_, type);
//                } else if (tmp[1] == 'a') {
//                    if (used[2]) throw std::string("Invalid\n");
//                    used[2] = true;
//                    memset(Author_, 0, sizeof(Author_));
//                    strcpy(Author_, type);
//                } else if (tmp[1] == 'p') {
//                    if (used[4]) throw std::string("Invalid\n");
//                    used[4] = true;
//                    Price = type;
//                    Price_ = atof(type);
//                } else throw std::string("Invalid\n");
//            }
//            tmp = nextToken();
//        }
//
//        // 检验没有语法问题后统一进行变换
//        Book tmpBook;
//        train_system.readFile(tmpBook, index_);
//
//        if (used[0]) {
//            std::string order = ISBN_;
//            order = "change ISBN to " + order;
//            log_.AddLog5(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 9, tmpBook.ISBN,
//                         order);
//            train_system.ISBN_modify(ISBN_, user_system);
//        }
//        if (used[1]) {
//            train_system.BookName_modify(BookName_, user_system);
//            std::string order = BookName_;
//            order = "change BookName to " + order;
//            log_.AddLog5(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 9, tmpBook.ISBN,
//                         order);
//        }
//        if (used[2]) {
//            train_system.Author_modify(Author_, user_system);
//            std::string order = Author_;
//            order = "change Author " + order;
//            log_.AddLog5(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 9, tmpBook.ISBN,
//                         order);
//        }
//        if (used[4]) {
//            train_system.Price_modify(Price_, user_system);
//            std::string order = "change BookName to " + Price;
//            log_.AddLog5(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 9, tmpBook.ISBN,
//                         order);
//        }
//        if (used[3]) {
//            Book tmp_;
//            train_system.Book_inf.seekg(sizeof(Book) * index_ + sizeof(int));
//            train_system.Book_inf.read(reinterpret_cast<char *>(&tmp_), sizeof(Book));
//
//            // 删除原来的key
//            char empty[61];
//            memset(empty, 0, sizeof(empty));
//            if (strcmp(empty, tmp_.Keyword) != 0) {
//                TokenScanner scanner_key_(tmp_.Keyword);
//                std::string key_ = scanner_key_.nextKey();
//                while (!key_.empty()) {
//                    char key_c[61];
//                    strcpy(key_c, key_.c_str());
//                    train_system.Keyword_map.erase(key_c, tmp_.ISBN, tmp_.tag);
//                    key_ = scanner_key_.nextKey();
//                }
//            }
//
//            strcpy(tmp_.Keyword, Keyword_);
//            train_system.Book_inf.seekp(sizeof(Book) * index_ + sizeof(int));
//            train_system.Book_inf.write(reinterpret_cast<char *>(&tmp_), sizeof(Book));
//
//            // 一个个插入新key
//            for (auto &iter: Key_store) {
//                char key_c[61];
//                strcpy(key_c, iter.c_str());
//                train_system.Keyword_map.insert(key_c, tmp_.ISBN, tmp_.tag);
//            }
//            std::string order = "change Keyword to " + Key;
//            log_.AddLog5(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 9, tmpBook.ISBN,
//                         order);
//        }
//    }
//
//    void import(UserSystem &user_system, TrainSystem &train_system) {
//        if (user_system.login_stack.top().Privilege < '3') throw std::string("Invalid\n");
//
//        int index_ = user_system.User_select.top();
//        if (index_ == -1) throw std::string("Invalid\n");
//
//        std::string tmp = nextToken();
//        int Quantity_ = check_Type6(tmp);
//
//        tmp = nextToken();
//        check_Type7(tmp);
//        double TotalCost_ = atof(tmp.c_str());
//
//        train_system.import(Quantity_, TotalCost_, user_system);
//
//        log_.AddFinance(false, TotalCost_);
//
//        Book tmpBook;
//        train_system.readFile(tmpBook, index_);
//        log_.AddLog3(user_system.login_stack.top().Privilege, user_system.login_stack.top().UserID, 7, tmpBook.ISBN,
//                     Quantity_,
//                     TotalCost_);
//    }
//
//    void show_finance(UserSystem &user_system) {
//        if (user_system.login_stack.top().Privilege < '7') throw std::string("Invalid\n");
//        std::string tmp = nextToken();
//        if (tmp.empty()) {
//            log_.ShowFinance(-1);
//            return;
//        }
//        int Quantity_ = check_Type6(tmp);
//        log_.ShowFinance(Quantity_);
//    }

};

#endif //TICKETSYSTEM_SRC_TokenScanner_H
