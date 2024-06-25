#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <vector>

#include "datalogger.hpp"

DataLogger* DataLogger::dataLogger_= nullptr;

DataLogger *DataLogger::getInstance()
{
    if (dataLogger_ == nullptr) {
        dataLogger_ = new DataLogger();
    }
    return dataLogger_;
}

void DataLogger::initialize()
{
    std::string currNumber;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("cd ..;python3 -u utils/SearchData.py", "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        currNumber += buffer;
    }
    this->currLogNumber = std::stoi(currNumber);
    std::string command = "cd ..;cd output;mkdir -p " + currNumber;
    system(command.c_str());
}

void DataLogger::displayData()
{
    FILE* pythonPipe = popen("cd ..;python3 -u utils/DisplayData.py", "w");
    if (!pythonPipe) {
        std::cerr << "Error opening pipe." << std::endl;
    } else {
        std::cout << "Displaying data..." << std::endl;
    }
    pclose(pythonPipe);
}

void DataLogger::saveGeneration(std::vector<std::vector<Solution*>> fronts, int generationNumber)
{
    std::string outFileCommand = "../output/" + std::to_string(currLogNumber) + "/" + std::to_string(generationNumber) + ".txt";
    std::ofstream outFile(outFileCommand.c_str(), std::ios::trunc);
    for (std::vector<Solution*> front : fronts) {
        if (front.size() != 0) {
            for (Solution* currSol : front) {
            outFile << currSol->getObjectives().transpose() << ":";
            outFile << "[" << currSol->getWeights().transpose() << "]" << std::endl;
            }
            outFile << "===" << std::endl;
        }
    }
    outFile.close();
}

DataLogger::DataLogger() {
    std::cout << "Creating data logger instance..." << std::endl;
    return;
}
