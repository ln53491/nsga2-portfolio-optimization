#ifndef DATALOGGER_HPP
#define DATALOGGER_HPP

#include <map>
#include <tuple>
#include <vector>
#include <eigen3/Eigen/Dense>

#include "../algorithm/types.hpp"
#include "../algorithm/solution.hpp"

class DataLogger
{
protected:
    DataLogger();
    static DataLogger* dataLogger_;

private:
    int currLogNumber = 0;

public:
    DataLogger(DataLogger &other) = delete;
    void operator=(const DataLogger &) = delete;
    static DataLogger* getInstance();
    void displayData();
    void initialize();
    void saveGeneration(std::vector<std::vector<Solution*>> fronts, int generationNumber);
};

#endif // DATALOGGER_HPP
