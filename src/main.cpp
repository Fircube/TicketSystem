#include <iostream>
#include <string>
#include "token_scanner.h"

int main() {
    std::string token;
    UserSystem userSystem;
    TrainSystem trainSystem;
    OrderSystem orderSystem;
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
                    break;
                } else throw std::string("Invalid\n");
            } else if (order == "clean") {
                if (!scanner.hasMore_) {
                    userSystem.clean();
                    trainSystem.clean();
                    orderSystem.clean();
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
            } else if (order == "add_train") {
                scanner.AddTrain(trainSystem);
            } else if (order == "delete_train") {
                scanner.DeleteTrain(trainSystem);
            } else if (order == "release_train") {
                scanner.ReleaseTrain(trainSystem);
            } else if (order == "query_train") {
                scanner.QueryTrain(trainSystem);
            } else if (order == "query_ticket") {
                scanner.QueryTicket(trainSystem);
            } else if (order == "query_transfer") {
                scanner.QueryTransfer(trainSystem);
            } else if (order == "buy_ticket") {
                scanner.BuyTicket(userSystem, trainSystem, orderSystem);
            } else if (order == "query_order") {
                scanner.QueryOrder(userSystem, trainSystem, orderSystem);
            } else if (order == "refund_ticket") {
                scanner.RefundTicket(userSystem, trainSystem, orderSystem);
            }
        }
        catch (std::string error_out) {
            std::cout << error_out;
        }
    }
    return 0;
}