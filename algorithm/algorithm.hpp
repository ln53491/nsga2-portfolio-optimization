#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <iostream>
#include <string>
#include <map>
#include <eigen3/Eigen/Dense>

#include "../utils/datalogger.hpp"
#include "solution.hpp"

typedef std::vector<Solution*> SolutionsVec;

class Algorithm {
private:
    DataLogger* logger = nullptr;

public:
    Algorithm() {};
    DataLogger* getDataLogger() {
        return this->logger;
    };
    void setDataLogger(DataLogger* logger) {
        this->logger = logger;
    };
    virtual Eigen::VectorXd evaluate(std::map<std::string, double> params, int evaluations) {
        return Eigen::VectorXd(0);
    }
};

#endif // ALGORITHM_HPP
