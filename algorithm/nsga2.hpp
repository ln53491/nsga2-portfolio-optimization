#ifndef NSGA2_HPP
#define NSGA2_HPP

#include <map>
#include <tuple>
#include "function.hpp"
#include "solution.hpp"
#include "../utils/rng.hpp"
#include "algorithm.hpp"

struct NSGA2Params {
    int populationSize;
    int tournamentSize;
    double tournamentProbability;
    double crossoverParam;
    double mutationFactor;
    double mutationProbability;
};

class NSGA2 : public Algorithm
{
private:
    RNG* rng;
    NSGA2Params params;
    Function* function;
    std::vector<SolutionsVec> fronts;

    void initialize();
    SolutionsVec createPopulation();
    std::vector<SolutionsVec> fastNonDominatedSort(SolutionsVec& population);
    SolutionsVec tournamentSelection(SolutionsVec& population);
    SolutionsVec crossover(SolutionsVec& parents);
    Solution* mutation(Solution& solution);
    void calcCrowdingDistance(SolutionsVec& front);

public:
    NSGA2() : Algorithm() {};
    static NSGA2Params constructParams(const std::map<std::string, double>& paramsMap);
    SolutionsVec run(int evaluations);
    std::vector<SolutionsVec> getFronts();
    Solution* computeBestSolution(SolutionsVec& population, double cutOffValue);

    Eigen::VectorXd evaluate(std::map<std::string, double> params, int evaluations) override;
    Eigen::VectorXd evaluate(NSGA2Params params, int evaluations, double cutOffValue = 10.0);
};

#endif // NSGA2_HPP

