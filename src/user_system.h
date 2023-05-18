#ifndef TICKETSYSTEM_SRC_USERSYSTEM_H
#define TICKETSYSTEM_SRC_USERSYSTEM_H

#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
#include "bpt.h"

class User {
public:
    int tag_ = -1;
    char username_[21];
    char password_[31];
    char name_[16];
    char mail_addr_[31];
    char privilege_;

    User() {
        memset(username_, 0, sizeof(username_));
        memset(password_, 0, sizeof(password_));
        memset(name_, 0, sizeof(name_));
        memset(mail_addr_, 0, sizeof(mail_addr_));
    }

    User(const int &tag, const char (&username)[31], const char (&password)[31], const char (&name)[31],
         const char (&mail_addr)[31], const char privilege) {
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

class UserSystem{
public:
    BPT<char[21]> user_map_;
    std::fstream user_inf_;
    std::string filename_="user_inf";

};


#endif //TICKETSYSTEM_SRC_USERSYSTEM_H
