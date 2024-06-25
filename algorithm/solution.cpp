#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <numeric>

#include "solution.hpp"
#include "../utils/rng.hpp"
#include "function.hpp"

Solution::Solution(int dimension)
{
    RNG* rng = RNG::getInstance();
    Function* function = Function::getInstance();
    Eigen::VectorXd weights(dimension);

    for (int i = 0; i < dimension; ++i) {
        weights(i) = rng->getDouble();
    }
    normalizeWeights(&weights);

    this->rank = NAN;
    this->crowdingDistance = NAN;
    this->weights = weights;
    this->objectives = function->calculateObjectives(weights);
}

Solution::Solution(Eigen::VectorXd weights)
{
    RNG* rng = RNG::getInstance();
    Function* function = Function::getInstance();
    this->rank = NAN;
    this->crowdingDistance = NAN;
    this->weights = weights;
    this->objectives = function->calculateObjectives(weights);
}

Solution::Solution(const Solution& other) :
    weights(other.weights),
    objectives(other.objectives),
    rank(other.rank),
    crowdingDistance(other.crowdingDistance){}

bool Solution::compareByDomination(Solution& other)
{
    bool and_condition = true;
    bool or_condition = false;

    for (int i = 0; i < objectives.size(); ++i) {
        if (objectives[i] < other.objectives[i]) and_condition = false;
        if (objectives[i] > other.objectives[i]) or_condition = true;
    }
    return and_condition && or_condition;
}

bool Solution::compareByCrowdingDistance(Solution& other)
{
    bool isBetterRank = rank < other.getRank();
    bool isBetterCrowdingDistance = rank == other.getRank() && crowdingDistance > other.getCrowdingDistance();
    return isBetterRank || isBetterCrowdingDistance;
}

std::function<bool(Solution*, Solution*)> Solution::objectiveComparator(int index)
{
    auto comparator = [index](Solution* a, Solution* b) {
        return a->getObjectives()[index] < b->getObjectives()[index];
    };
    return comparator;
}

std::function<bool(Solution*, Solution*)> Solution::crowdingDistanceComparator()
{
    auto comparator = [](Solution* a, Solution* b) {
        return a->getCrowdingDistance() > b->getCrowdingDistance();
    };
    return comparator;
}

void Solution::normalizeWeights(Eigen::VectorXd* weights)
{
    double sum = weights->sum();
    for (int i = 0; i < weights->size(); ++i) {
        (*weights)[i] /= sum;
    }
}

bool Solution::isValid()
{
    return fabs(weights.sum() - 1.0) < 1e-9;
}

int Solution::getRank() {
    return rank;
}

void Solution::setRank(int rank) {
    this->rank = rank;
}

double Solution::getCrowdingDistance() {
    return crowdingDistance;
}

void Solution::setCrowdingDistance(double crowdingDistance) {
    this->crowdingDistance = crowdingDistance;
}

void Solution::setObjectives(Eigen::VectorXd objectives) {
    this->objectives = objectives;
}

Eigen::VectorXd Solution::getWeights() {
    return weights;
}

Eigen::VectorXd Solution::getObjectives() {
    return objectives;
}

void Solution::toString()
{
    std::cout << "Weights: " << weights.transpose() << std::endl;
    std::cout << "Objectives: " << objectives.transpose() << std::endl;
}

void Solution::toPrettyString() {
    // TODO
}
