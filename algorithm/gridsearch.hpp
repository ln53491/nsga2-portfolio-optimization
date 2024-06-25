#ifndef GRIDSEARCH_HPP
#define GRIDSEARCH_HPP

#include <vector>

#include "algorithm.hpp"

typedef std::map<std::string, double> ParametersMap;

class GridSearch
{
private:
    Algorithm* algorithm;
    std::vector<ParametersMap> parametersGrid;

    std::map<std::string, std::vector<double>> readParamsFromFile(const std::string& filename);
    void generateCombinations(
        std::map<std::string, std::vector<double>>::const_iterator current,
        std::map<std::string, std::vector<double>>::const_iterator end,
        ParametersMap& currentCombination,
        std::vector<ParametersMap>& allCombinations);

public:
    GridSearch(Algorithm* algorithm, const std::string& parametersFile);
    std::vector<ParametersMap> getParametersGrid();
    ParametersMap run(
        int minIterations,
        int maxIterations,
        int factor);
};

#endif // GRIDSEARCH_HPP
