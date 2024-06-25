#ifndef RNG_HPP
#define RNG_HPP

#include <random>
#include <vector>

class RNG
{
protected:
    RNG();
    RNG(int seed);
    static RNG* rng_;
    std::mt19937 gen;
    std::uniform_int_distribution<> intDist;
    std::uniform_real_distribution<> doubleDist;
    std::normal_distribution<> normalDist;

public:
    RNG(RNG &other) = delete;
    void operator=(const RNG &) = delete;
    static RNG *getInstance();
    static RNG *getInstance(int seed);

    int getInt();
    double getDouble();
    double getNormal();
    std::vector<int> sampleInt(int min, int max, int size);
};

#endif // RNG_HPP
