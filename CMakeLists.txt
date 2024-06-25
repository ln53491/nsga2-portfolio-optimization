cmake_minimum_required(VERSION 3.0)

project(portfoliooptimizer)

add_executable(portfoliooptimizer main.cpp)

install(TARGETS portfoliooptimizer RUNTIME DESTINATION bin)
