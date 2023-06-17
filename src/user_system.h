#ifndef TICKETSYSTEM_SRC_USERSYSTEM_H
#define TICKETSYSTEM_SRC_USERSYSTEM_H

#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include "bpt.h"
#include "map.h"
#include "my_string.h"

class User {
public:
    int tag_ = -1;
    char username_[21];
    char password_[31];
    char name_[16];
    char mail_addr_[31];
    short int privilege_ = -1;

    User() {
        memset(username_, 0, sizeof(username_));
        memset(password_, 0, sizeof(password_));
        memset(name_, 0, sizeof(name_));
        memset(mail_addr_, 0, sizeof(mail_addr_));
    }

    User(const int &tag, const char (&username)[31], const char (&password)[31], const char (&name)[31],
         const char (&mail_addr)[31], const short int &privilege) {
        memset(username_, 0, sizeof(username_));
        memset(password_, 0, sizeof(password_));
        memset(name_, 0, sizeof(name_));
        memset(mail_addr_, 0, sizeof(mail_addr_));
        tag_ = tag;
        strcpy(username_, username);
        strcpy(password_, password);
        strcpy(name_, name);
        strcpy(mail_addr_, mail_addr);
        privilege_ = privilege;
    }

    User(const User &rhs) {
        memset(username_, 0, sizeof(username_));
        memset(password_, 0, sizeof(password_));
        memset(name_, 0, sizeof(name_));
        memset(mail_addr_, 0, sizeof(mail_addr_));
        tag_ = rhs.tag_;
        strcpy(username_, rhs.username_);
        strcpy(password_, rhs.password_);
        strcpy(name_, rhs.name_);
        strcpy(mail_addr_, rhs.mail_addr_);
        privilege_ = rhs.privilege_;
    }

    User &operator=(const User &rhs) {
        if (this == &rhs) return *this;
        tag_ = rhs.tag_;
        memset(username_, 0, sizeof(username_));
        memset(password_, 0, sizeof(password_));
        memset(name_, 0, sizeof(name_));
        memset(mail_addr_, 0, sizeof(mail_addr_));
        strcpy(username_, rhs.username_);
        strcpy(password_, rhs.password_);
        strcpy(name_, rhs.name_);
        strcpy(mail_addr_, rhs.mail_addr_);
        privilege_ = rhs.privilege_;
        return *this;
    }
};

class UserSystem {
public:
    BPT<sjtu::String<21>, int> user_map_; // username指向tag
    sjtu::map<std::string, int> user_login_; // 是否登录，logout后指向-1
    std::fstream user_inf_;
    std::string filename_ = "user_inf";
    int amount = 0; // user人数

    UserSystem() : user_map_("username_tree", "username_record", "user_tag") {
        user_inf_.open(filename_);
        if (!user_inf_.good()) { // 是否成功打开
            user_inf_.open(filename_, std::fstream::out); // 新建
            user_inf_.close();
            user_inf_.open(filename_); // 变为可读可写
        } else {
            user_inf_.seekg(0);
            user_inf_.read(reinterpret_cast <char *> (&amount), sizeof(amount));
        }
    }

    ~UserSystem() {
        user_inf_.seekp(0);
        user_inf_.write(reinterpret_cast <char *> (&amount), sizeof(amount));
        user_inf_.close();
    }

    void readFile(User &read, const int &location) {
        user_inf_.seekg(sizeof(User) * location + sizeof(amount));
        user_inf_.read(reinterpret_cast<char *>(&read), sizeof(User));
    }

    void writeFile(User &write, const int &location) {
        user_inf_.seekp(sizeof(User) * location + sizeof(amount));
        user_inf_.write(reinterpret_cast<char *>(&write), sizeof(User));
    }

    int AddFirstUser(const char (&new_user)[21], User &new_inf) {
        user_map_.insert(new_user, amount);
        new_inf.tag_ = amount;
        writeFile(new_inf, amount);
        ++amount;
        return 0;
    }

    int AddUser(const char (&cur_user)[21], const char (&new_user)[21], User &new_inf) {
        auto cur_find = user_login_.find(cur_user);
        if (cur_find == user_login_.end() || cur_find->second == -1) { // 当前用户未登录
            return -1;
        }
        User cur_inf;
        readFile(cur_inf, cur_find->second);
        if (cur_inf.privilege_ <= new_inf.privilege_) return -1; // 当前权限低
        int new_tag = user_map_.find(new_user);
        if (new_tag != -1) return -1; // 新用户存在

        user_map_.insert(new_user, amount);
        new_inf.tag_ = amount;
        writeFile(new_inf, amount);
        ++amount;
        return 0;
    }

    int Login(const char (&user)[21], const char (&password)[31]) {
        auto find = user_login_.find(user);
        if (find != user_login_.end() && find->second != -1) { // 用户已经登录
            return -1;
        }
        int tag = user_map_.find(user);
        if (tag == -1) return -1; // 用户不存在
        User inf;
        readFile(inf, tag);
        if (strcmp(inf.password_, password) != 0) return -1; // 密码错误

        user_login_[user] = inf.tag_;
        return 0;
    }

    int Logout(const char (&user)[21]) {
        auto find = user_login_.find(user);
        if (find == user_login_.end() || find->second == -1) { // 用户已经登录
            return -1;
        }
        find->second = -1;
        return 0;
    }

    void QueryProfile(const char (&cur_user)[21], const char (&q_user)[21]) {
        auto cur_find = user_login_.find(cur_user);
        if (cur_find == user_login_.end() || cur_find->second == -1) { // 当前用户未登录
            std::cout << "-1\n";
            return;
        }
        User q_inf;
        if (strcmp(cur_user, q_user) == 0) { // 查询自己
            readFile(q_inf, cur_find->second);
        } else {
            int q_tag = user_map_.find(q_user);
            if (q_tag == -1) { // 查询用户不存在
                std::cout << "-1\n";
                return;
            }
            User cur_inf;
            readFile(cur_inf, cur_find->second);
            readFile(q_inf, q_tag);
            if (cur_inf.privilege_ <= q_inf.privilege_) { // 当前权限低
                std::cout << "-1\n";
                return;
            }
        }
        std::cout << q_user << ' ' << q_inf.name_ << ' ' << q_inf.mail_addr_ << ' ' << q_inf.privilege_ << '\n';
    }

    bool ModifyProfile(const char (&cur_user)[21], const char (&m_user)[21], const short &m_privilege, User &pre_inf) {
        auto cur_find = user_login_.find(cur_user);
        if (cur_find == user_login_.end() || cur_find->second == -1) { // 当前用户未登录
            return false;
        }
        if (strcmp(cur_user, m_user) == 0) { // 修改自己
            readFile(pre_inf, cur_find->second);
            if (pre_inf.privilege_ <= m_privilege) return false; // 当前权限低
        } else {
            int m_tag = user_map_.find(m_user);
            if (m_tag == -1) { // 查询用户不存在
                return false;
            }
            User cur_inf;
            readFile(cur_inf, cur_find->second);
            readFile(pre_inf, m_tag);
            if (cur_inf.privilege_ <= pre_inf.privilege_ || cur_inf.privilege_ <= m_privilege) { // 当前权限低
                return false;
            }
        }
        return true;
    }

    void clean() {
        user_login_.clear();
        user_map_.clear();
    }
};


#endif //TICKETSYSTEM_SRC_USERSYSTEM_H
