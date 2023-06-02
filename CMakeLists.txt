cmake_minimum_required(VERSION 3.16)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 14)

add_executable(TicketSystem src/vector.h src/function.h src/schedule.h src/token_scanner.h src/bpt.h src/user_system.h src/train_system.h src/main.cpp src/multi_object_bpt.h src/order_system.h)
