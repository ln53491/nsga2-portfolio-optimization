#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <eigen3/Eigen/Dense>

#include "function.hpp"

Function* Function::function_= nullptr;

Function *Function::getInstance()
{
    if(function_ == nullptr) {
        function_ = new Function();

        FILE* pythonPipe = popen("cd ..;python3 -u utils/FetchData.py", "w");

        if (!pythonPipe) {
            std::cerr << "Error opening pipe." << std::endl;
            return function_;
        } else {
            std::cout << "Starting to fetch finance data..." << std::endl;
        }
        pclose(pythonPipe);

        std::ifstream file("../input/output.cnf");
        if (!file.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
            return function_;
        } else {
            std::cout << "Reading finance output file..." << std::endl;
        }

        int dimension = 0;
        FunctionData functionData;
        Eigen::VectorXd returns(0);
        Eigen::VectorXd stDevs(0);
        Eigen::MatrixXd correlationMatrix;
        std::vector<std::string> tickers;

        std::string line;
        int lineNumber = 0;
        bool isCorrMatrix = false;

        while (std::getline(file, line)) {
            ++lineNumber;
            switch (lineNumber) {

                case 1: {
                    functionData.dateStart = line;
                } break;

                case 2: {
                    functionData.dateEnd = line;
                } break;

                case 3: {
                    std::stringstream lineStream(line);
                    std::string currTicker;
                    while (std::getline(lineStream, currTicker, ',')) {
                        tickers.push_back(currTicker);
                        dimension++;
                    }
                    functionData.tickers = tickers;
                    returns.resize(dimension);
                    stDevs.resize(dimension);
                    correlationMatrix.resize(dimension, dimension);
                } break;

                default:
                    if (line == "=====") {
                        isCorrMatrix = true;
                        break;
                    }
                    std::stringstream lineStream(line);
                    std::string currToken;
                    int counter = 0;
                    while (std::getline(lineStream, currToken, ',')) {
                        if (isCorrMatrix) {
                            correlationMatrix(lineNumber - dimension - 5, counter) = std::stod(currToken);
                        } else {
                            if (counter == 0) returns(lineNumber - 4) = std::stod(currToken);
                            if (counter == 1) stDevs(lineNumber - 4) = std::stod(currToken);
                        }
                        counter++;
                    }
            }
        }
        function_->dimension = dimension;
        function_->returns = returns;
        function_->stDevs = stDevs;
        function_->functionData = functionData;
        function_->correlationMatrix = correlationMatrix;

        file.close();
    }
    return function_;
}

long double Function::calcReturnResult(const Eigen::VectorXd& weights)
{
    return returns.dot(weights);
}

long double Function::calcRiskResult(const Eigen::VectorXd& weights)
{
    Eigen::MatrixXd covarianceMatrix = stDevs.asDiagonal() * correlationMatrix * stDevs.asDiagonal();
    return -std::sqrt(weights.transpose() * covarianceMatrix * weights) * 100;
}

Eigen::VectorXd Function::calculateObjectives(const Eigen::VectorXd& weights)
{
    long double expectedReturn = calcReturnResult(weights);
    long double portfolioRisk = calcRiskResult(weights);

    Eigen::VectorXd objectives(2);
    objectives << expectedReturn, portfolioRisk;
    return objectives;
}

Function::Function() {
    std::cout << "Creating function instance..." << std::endl;
    return;
}

FunctionData Function::getFunctionData() const {
    return functionData;
}

int Function::getDimension() const {
    return dimension;
}

Eigen::VectorXd Function::getReturnsVector() const {
    return returns;
}

Eigen::VectorXd Function::getStDevsVector() const {
    return stDevs;
}

Eigen::MatrixXd Function::getCorrelationMatrix() const {
    return correlationMatrix;
}
