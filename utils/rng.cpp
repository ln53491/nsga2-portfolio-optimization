#include <random>
#include <limits>
#include <algorithm>

#include "rng.hpp"

RNG* RNG::rng_= nullptr;

RNG::RNG() {
    std::random_device rd;
    this->gen = std::mt19937(rd());
}

RNG::RNG(int seed) {
    this->gen = std::mt19937(seed);
}

RNG *RNG::getInstance()
{
    if(rng_ == nullptr) {
        rng_ = new RNG();
        rng_->intDist = std::uniform_int_distribution<int>();
        rng_->doubleDist = std::uniform_real_distribution<double>();
        rng_->normalDist = std::normal_distribution<double>();
    }
    return rng_;
}

RNG *RNG::getInstance(int seed)
{
    if(rng_ == nullptr) {
        rng_ = new RNG(seed);
        rng_->intDist = std::uniform_int_distribution<int>();
        rng_->doubleDist = std::uniform_real_distribution<double>();
        rng_->normalDist = std::normal_distribution<double>();
    }
    return rng_;
}

std::vector<int> RNG::sampleInt(int min, int max, int size)
{
    std::vector<int> sample;
    while (sample.size() < size) {
        int randomNumber = intDist(gen);
        double normalized = static_cast<double>(randomNumber) / std::numeric_limits<int>::max();
        int scaledNumber = static_cast<int>(min + normalized * (max - min));

        if (max - min < size || std::find(sample.begin(), sample.end(), scaledNumber) == sample.end()) {
            sample.push_back(scaledNumber);
        }
    }
    return sample;
}

int RNG::getInt() {
    return intDist(gen);
}

double RNG::getDouble() {
    return doubleDist(gen);
}

double RNG::getNormal() {
    return normalDist(gen);
}
