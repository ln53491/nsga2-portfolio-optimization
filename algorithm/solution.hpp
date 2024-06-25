#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <vector>
#include <eigen3/Eigen/Dense>

class Solution
{
private:
    Eigen::VectorXd weights;
    Eigen::VectorXd objectives;
    int rank;
    double crowdingDistance;

public:
    Solution(int dimension);
    Solution(const Solution& other);
    Solution(Eigen::VectorXd weights);
    int getRank();
    void setRank(int rank);
    double getCrowdingDistance();
    void setCrowdingDistance(double crowdingDistance);
    Eigen::VectorXd getWeights();
    Eigen::VectorXd getObjectives();
    static void normalizeWeights(Eigen::VectorXd* weights);
    void setObjectives(Eigen::VectorXd objectives);

    bool compareByDomination(Solution& other);
    bool compareByCrowdingDistance(Solution& other);
    static std::function<bool(Solution*, Solution*)> objectiveComparator(int index);
    static std::function<bool(Solution*, Solution*)> crowdingDistanceComparator();

    bool isValid();
    void toString();
    void toPrettyString();
};

#endif // SOLUTION_HPP
