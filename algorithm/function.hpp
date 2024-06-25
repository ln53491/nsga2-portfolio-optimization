#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <map>
#include <tuple>
#include <eigen3/Eigen/Dense>

#include "types.hpp"

class Function
{
protected:
    Function();
    static Function* function_;
    int dimension;
    Eigen::VectorXd returns;
    Eigen::VectorXd stDevs;
    Eigen::MatrixXd correlationMatrix;
    struct FunctionData functionData;

private:
    long double calcReturnResult(const Eigen::VectorXd& weights);
    long double calcRiskResult(const Eigen::VectorXd& weights);

public:
    Function(Function &other) = delete;
    void operator=(const Function &) = delete;
    static Function* getInstance();
    Eigen::VectorXd calculateObjectives(const Eigen::VectorXd& weights);

    int getDimension() const;
    FunctionData getFunctionData() const;
    Eigen::VectorXd getReturnsVector() const;
    Eigen::VectorXd getStDevsVector() const;
    Eigen::MatrixXd getCorrelationMatrix() const;
};

#endif // FUNCTION_HPP
