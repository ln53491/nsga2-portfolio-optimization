#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <random>
#include <eigen3/Eigen/Dense>

#include "algorithm/nsga2.cpp"
#include "algorithm/gridsearch.cpp"
#include "algorithm/function.cpp"
#include "algorithm/solution.cpp"
#include "utils/datalogger.cpp"
#include "utils/rng.cpp"

void runNSGA2()
{
  NSGA2Params params = {
    130,    // populationSize
    2,      // tournamentSize
    0.15,   // tournamentProbability
    0.14,   // crossoverParam
    0.45,   // mutationFactor
    0.16    // mutationProbability
  };
  int evaluations = 2;

  NSGA2* nsga2 = new NSGA2();
  Eigen::VectorXd result = nsga2->evaluate(params, evaluations, 10.0);
  std::cout << result.transpose() << std::endl;
}

void runGridSearch()
{
  Algorithm* nsga2 = new NSGA2();
  GridSearch* gridSearch = new GridSearch(nsga2, "../input/params.cnf");

  ParametersMap bestParams = gridSearch->run(10, 40, 2);

  for (const auto& kv : bestParams) {
      std::cout << kv.first << ": " << kv.second << ", ";
  }
}

int main(void) {
  int seed = 12345;
  RNG* rng = RNG::getInstance(seed);

  runNSGA2();

  return 0;
}
