#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<string>
#include<fstream>
#include "token_scanner.h"

int main() {
    freopen("../Ticket-System-master/testcases/data/basic_2/1.in","r",stdin);
    freopen("my.ans","w",stdout);
    std::string token;
    UserSystem userSystem;
//    TrainSystem trainSystem;
    while (getline(std::cin, token)) {
        try {
            std::string order;
            TokenScanner scanner(token);
            std::cout << scanner.nextToken() << ' ';
            order = scanner.nextToken();
            if (order.empty()) {
            } else if (order == "exit") {
                if (!scanner.hasMore_) {
                    std::cout << "bye\n";
                    exit(0);
                } else throw std::string("Invalid\n");
            } else if (order == "clean") {
                if (!scanner.hasMore_) {
                    userSystem.clean();
//                    trainSystem.clean();
                    std::cout << "0\n";
                } else throw std::string("Invalid\n");
            } else if (order == "add_user") {
                scanner.AddUser(userSystem);
            } else if (order == "login") {
                scanner.Login(userSystem);
            } else if (order == "logout") {
                scanner.Logout(userSystem);
            } else if (order == "query_profile") {
                scanner.QueryProfile(userSystem);
            } else if (order == "modify_profile") {
                scanner.ModifyProfile(userSystem);
            }
//            else if (order == "buy") {
//                if (accountSystem.login_stack.empty()) throw std::string("Invalid\n");
//                scanner.buy(accountSystem, bookSystem, logSystem);
//            } else if (order == "select") {
//                if (accountSystem.login_stack.empty()) throw std::string("Invalid\n");
//                scanner.select(accountSystem, bookSystem, logSystem);
//            } else if (order == "modify") {
//                if (accountSystem.login_stack.empty()) throw std::string("Invalid\n");
//                scanner.modify(accountSystem, bookSystem, logSystem);
//            } else if (order == "import") {
//                if (accountSystem.login_stack.empty()) throw std::string("Invalid\n");
//                scanner.import(accountSystem, bookSystem, logSystem);
//            } else if (order == "log") {
//                if (accountSystem.login_stack.empty()) throw std::string("Invalid\n");
//                logSystem.showLog();
//            } else if (order == "show") {
//                int point_ = scanner.point;
//                order = scanner.nextToken();
//                if (order == "finance") { // show finance
//                    if (accountSystem.login_stack.empty()) throw std::string("Invalid\n");
//                    scanner.show_finance(accountSystem, logSystem);
//                } else { // show in bookSystem
//                    scanner.point = point_;
//                    scanner.hasMore_ = true;
//                    if (accountSystem.login_stack.empty()) throw std::string("Invalid\n");
//                    scanner.show(accountSystem, bookSystem, logSystem);
//                }
//            } else {
//                throw std::string("Invalid\n");
//            }
        }
        catch (std::string error_out) {
            std::cout << error_out;
        }
    }

}