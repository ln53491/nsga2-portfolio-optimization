#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <eigen3/Eigen/Dense>

#include "nsga2.hpp"

void NSGA2::initialize()
{
    this->rng = RNG::getInstance();
    this->function = Function::getInstance();
    std::vector<SolutionsVec> fronts;
    this->fronts = fronts;
}

Eigen::VectorXd NSGA2::evaluate(std::map<std::string, double> params, int evaluations)
{
    initialize();
    this->params = constructParams(params);
    double cutOffValue = 10.0;

    SolutionsVec population = run(evaluations);
    Solution* bestSolution = computeBestSolution(population, cutOffValue);
    return bestSolution->getObjectives();
}

Eigen::VectorXd NSGA2::evaluate(NSGA2Params params, int evaluations, double cutOffValue)
{
    initialize();
    this->params = params;
    this->setDataLogger(DataLogger::getInstance());

    SolutionsVec population = run(evaluations);
    Solution* bestSolution = computeBestSolution(population, cutOffValue);
    return bestSolution->getObjectives();
}

SolutionsVec NSGA2::run(int evaluations)
{
    initialize();
    DataLogger* logger = this->getDataLogger();
    if (logger) logger->initialize();

    SolutionsVec population = createPopulation();
    std::vector<SolutionsVec> fronts = fastNonDominatedSort(population);
    for (SolutionsVec front : fronts) {
        calcCrowdingDistance(front);
    }
    SolutionsVec parents = tournamentSelection(population);
    SolutionsVec children = crossover(parents);
    children[0] = mutation(*children[0]);
    children[1] = mutation(*children[1]);

    if (logger) logger->saveGeneration(fronts, 0);

    for (int i = 0; i < evaluations; ++i) {
        std::cout << "Generation: " << i+1 << "/" << evaluations << std::endl;

        SolutionsVec newPopulation;
        population.push_back(children[0]);
        population.push_back(children[1]);

        std::vector<SolutionsVec> fronts = fastNonDominatedSort(population);

        int currFront = 0;
        while (newPopulation.size() + fronts[currFront].size() <= params.populationSize) {
            calcCrowdingDistance(fronts[currFront]);
            for (Solution* solution : fronts[currFront]) {
                newPopulation.push_back(new Solution(*solution));
            }
            if (currFront + 1 == fronts.size() && newPopulation.size() == params.populationSize) break;
            currFront++;
        }
        calcCrowdingDistance(fronts[currFront]);
        std::sort(fronts[currFront].begin(), fronts[currFront].end(), Solution::crowdingDistanceComparator());

        int currNewPopulationSize = newPopulation.size();
        for (int j = 0; j < params.populationSize - currNewPopulationSize; ++j) {
            newPopulation.push_back(fronts[currFront][j]);
        }
        population = newPopulation;

        fronts = fastNonDominatedSort(population);
        for (SolutionsVec front : fronts) {
            calcCrowdingDistance(front);
        }
        SolutionsVec parents = tournamentSelection(population);
        SolutionsVec newChildren = crossover(parents);

        children[0] = mutation(*newChildren[0]);
        children[1] = mutation(*newChildren[1]);

        if (logger) logger->saveGeneration(fronts, i+1);
    }
    if (logger) logger->displayData();
    return population;
}

void NSGA2::calcCrowdingDistance(SolutionsVec& front)
{
    if (front.size() > 0) {
        for (Solution* currSolution : front) {
            currSolution->setCrowdingDistance(0.0);
        }
        for (int i = 0; i < front[0]->getObjectives().size(); ++i) {
            std::sort(front.begin(), front.end(), Solution::objectiveComparator(i));

            front.front()->setCrowdingDistance(INT_MAX);
            front.back()->setCrowdingDistance(INT_MAX);

            std::vector<double> currObjValues;
            for (Solution* currSolution : front) {
                currObjValues.push_back(currSolution->getObjectives()[i]);
            }
            double minValue = *std::min_element(currObjValues.begin(), currObjValues.end());
            double maxValue = *std::max_element(currObjValues.begin(), currObjValues.end());
            double scale = maxValue - minValue;
            if (scale == 0.0) scale = 1.0;

            for (int j = 1; j < front.size() - 1; ++j) {
                double newDistance = (front[j+1]->getObjectives()[i] - front[j-1]->getObjectives()[i]) / scale;
                front[j]->setCrowdingDistance(front[j]->getCrowdingDistance() + newDistance);
            }
        }
    }
}

std::vector<SolutionsVec> NSGA2::fastNonDominatedSort(SolutionsVec& population)
{
    std::vector<SolutionsVec> fronts(1);
    std::map<Solution*, int> dominatedByMap;
    std::map<Solution*, SolutionsVec> dominatesMap;

    for (Solution* currSolution : population) {
        SolutionsVec dominatedSolutions;
        dominatedByMap[currSolution] = 0;

        for (Solution* otherSolution : population) {
            if (otherSolution != currSolution) {
                if (currSolution->compareByDomination(*otherSolution)) {
                    dominatedSolutions.push_back(otherSolution);
                } else if (otherSolution->compareByDomination(*currSolution)) {
                    dominatedByMap[currSolution]++;
                }
            }
        }
        if (dominatedByMap[currSolution] == 0) {
            currSolution->setRank(0);
            fronts[0].push_back(currSolution);
        }
        dominatesMap[currSolution] = dominatedSolutions;
    }
    for (int i = 0; fronts[i].size() > 0; ++i) {
        SolutionsVec newFront;
        for (Solution* currSolution : fronts[i]) {
            for (Solution* otherSolution : dominatesMap[currSolution]) {
                dominatedByMap[otherSolution]--;
                if (dominatedByMap[otherSolution] == 0) {
                    otherSolution->setRank(i + 1);
                    newFront.push_back(otherSolution);
                }
            }
        }
        fronts.push_back(newFront);
    }
    this->fronts = fronts;
    return fronts;
}

SolutionsVec NSGA2::tournamentSelection(SolutionsVec& population)
{
    std::vector<int> parentsIndexes;
    while (parentsIndexes.size() < 2) {
        std::vector<int> participants = rng->sampleInt(0, population.size(), params.tournamentSize);
        int bestParticipant = participants.front();

        for (int participant : participants) {
            if (population[participant]->compareByCrowdingDistance(*population[bestParticipant]) &&
                rng->getDouble() <= params.tournamentProbability
            ) {
                bestParticipant = participant;
            }
        }
        if (std::find(parentsIndexes.begin(), parentsIndexes.end(), bestParticipant) == parentsIndexes.end()) {
            parentsIndexes.push_back(bestParticipant);
        }
    }
    SolutionsVec parents;
    for (int parentIndex : parentsIndexes) {
        parents.push_back(population[parentIndex]);
    }
    return parents;
}

SolutionsVec NSGA2::crossover(SolutionsVec& parents)
{
    int dimension = function->getDimension();
    Eigen::VectorXd child1(dimension);
    Eigen::VectorXd child2(dimension);
    Eigen::VectorXd parent1 = parents[0]->getWeights();
    Eigen::VectorXd parent2 = parents[1]->getWeights();

    for (int i = 0; i < dimension; ++i) {
        if (rng->getDouble() < params.crossoverParam) {
            child1[i] = parent1[i];
            child2[i] = parent2[i];
        } else {
            child1[i] = parent2[i];
            child2[i] = parent1[i];
        }
    }
    Solution::normalizeWeights(&child1);
    Solution::normalizeWeights(&child2);
    return {new Solution(child1), new Solution(child2)};
}

Solution* NSGA2::mutation(Solution& solution)
{
    int dimension = function->getDimension();
    Eigen::VectorXd mutatedSolution(dimension);
    Eigen::VectorXd solutionWeights = solution.getWeights();

    for (int i = 0; i < dimension; ++i) {
        if (rng->getDouble() < params.mutationProbability) {
            double perturbation;
            do {
                perturbation = params.mutationFactor * rng->getNormal() + solutionWeights[i];
            } while (perturbation < 0 || perturbation > 1);
            mutatedSolution[i] = perturbation;
        } else {
            mutatedSolution[i] = solutionWeights[i];
        }
    }
    Solution::normalizeWeights(&mutatedSolution);
    return new Solution(mutatedSolution);
}

SolutionsVec NSGA2::createPopulation()
{
    SolutionsVec population(params.populationSize);
    for (int i = 0; i < population.size(); ++i) {
        population[i] = new Solution(function->getDimension());
    }
    return population;
}

Solution* NSGA2::computeBestSolution(SolutionsVec& population, double cutOffValue)
{
    SolutionsVec satisfiableSolutions;
    for (Solution* currSol : population) {
        if (-currSol->getObjectives()[1] <= cutOffValue) {
            satisfiableSolutions.push_back(currSol);
        }
    }
    std::sort(satisfiableSolutions.begin(), satisfiableSolutions.end(), Solution::objectiveComparator(0));
    return satisfiableSolutions.back();
}

std::vector<SolutionsVec> NSGA2::getFronts()
{
    return this->fronts;
}

NSGA2Params NSGA2::constructParams(const std::map<std::string, double>& paramsMap)
{
    NSGA2Params params;
    params.populationSize = static_cast<int>(paramsMap.at("populationSize"));
    params.tournamentSize = static_cast<int>(paramsMap.at("tournamentSize"));
    params.tournamentProbability = paramsMap.at("tournamentProbability");
    params.crossoverParam = paramsMap.at("crossoverParam");
    params.mutationFactor = paramsMap.at("mutationFactor");
    params.mutationProbability = paramsMap.at("mutationProbability");
    return params;
}
