#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <string>
#include <iterator>
#include <eigen3/Eigen/Dense>

#include "gridsearch.hpp"

GridSearch::GridSearch(Algorithm* algorithm, const std::string& parametersFile)
{
    this->algorithm = algorithm;
    std::map<std::string, std::vector<double>> paramMap;
    paramMap = readParamsFromFile(parametersFile);

    ParametersMap currentCombination;
    std::vector<ParametersMap> paramsGrid;
    generateCombinations(paramMap.cbegin(), paramMap.cend(), currentCombination, paramsGrid);
    this->parametersGrid = paramsGrid;
}

std::map<std::string, std::vector<double>> GridSearch::readParamsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::map<std::string, std::vector<double>> param_map;

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open params file: " + filename);
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, ':')) {
            std::string values_str;
            if (std::getline(iss, values_str)) {
                std::vector<std::string> values;
                std::string token;
                std::istringstream tokenStream(values_str);
                while (std::getline(tokenStream, token, ',')) {
                    values.push_back(token);
                }
                std::vector<double> double_values;
                for (const std::string& value : values) {
                    double_values.push_back(std::stod(value));
                }
                param_map[key] = double_values;
            }
        }
    }
    file.close();
    return param_map;
}

void GridSearch::generateCombinations(
    std::map<std::string, std::vector<double>>::const_iterator current,
    std::map<std::string, std::vector<double>>::const_iterator end,
    ParametersMap& currentCombination,
    std::vector<ParametersMap>& paramsGrid)
{
    if (current == end) {
        paramsGrid.push_back(currentCombination);
        return;
    }
    std::string key = current->first;
    for (double value : current->second) {
        currentCombination[key] = value;
        generateCombinations(std::next(current), end, currentCombination, paramsGrid);
    }
}

ParametersMap GridSearch::run(int minIterations, int maxIterations, int factor) {
    std::vector<ParametersMap> paramGrid(this->parametersGrid);
    int candidatesNumber = paramGrid.size();
    int evaluations = minIterations;
    ParametersMap bestParams;
    double bestScore = -1;

    while (evaluations <= maxIterations && candidatesNumber > 1) {
        std::vector<std::pair<double, ParametersMap>> scores;

        for (auto& params : paramGrid) {
            double score = algorithm->evaluate(params, evaluations)[0];
            scores.push_back({score, params});
        }
        std::sort(scores.begin(), scores.end(),
                  [](const std::pair<double, ParametersMap>& a,
                     const std::pair<double, ParametersMap>& b) {
            return a.first > b.first;
        });
        candidatesNumber = std::ceil(candidatesNumber / factor);
        paramGrid.clear();

        for (int i = 0; i < candidatesNumber; ++i) {
            paramGrid.push_back(scores[i].second);
        }
        evaluations *= factor;
    }

    if (!paramGrid.empty()) {
        double finalScore = algorithm->evaluate(paramGrid[0], maxIterations)[0];
        if (finalScore > bestScore) {
            bestScore = finalScore;
            bestParams = paramGrid[0];
        }
    }
    return bestParams;
}

std::vector<ParametersMap> GridSearch::getParametersGrid()
{
    return this->parametersGrid;
}

