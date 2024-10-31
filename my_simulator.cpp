#include "my_simulator.h"
#include "concrete_wall_sensor.h"
#include "concrete_dirt_sensor.h"
#include "concrete_battery_meter.h"
#include "enums.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <cmath>

bool MySimulator::readHouseFile(const std::string& houseFilePath) {
    std::ifstream file(houseFilePath);
    if (!file) {
        std::cerr << "Error: Could not open file " << houseFilePath << std::endl;
        return false;
    }
    
    inputFileName = houseFilePath;
    std::string line;
    std::getline(file, houseName); // Line 1: house name / description

    std::getline(file, line); // Line 2: MaxSteps
    std::stringstream(line.substr(line.find('=') + 1)) >> maxSteps;

    std::getline(file, line); // Line 3: MaxBattery
    std::stringstream(line.substr(line.find('=') + 1)) >> maxBattery;

    std::getline(file, line); // Line 4: Rows
    std::stringstream(line.substr(line.find('=') + 1)) >> rows;

    std::getline(file, line); // Line 5: Cols
    std::stringstream(line.substr(line.find('=') + 1)) >> cols;

    // Read the house structure
    bool DockingFound=false;
    house = std::vector<std::vector<char>>(rows, std::vector<char>(cols, ' '));
    for (std::size_t i = 0; i < rows; ++i) {
        if (std::getline(file, line)) {
            for (std::size_t j = 0; j < line.size() && j < cols; ++j) {
                house[i][j] = line[j];
                if (line[j] == 'D') {
                    DockingFound=true;
                    dockingStation = {i, j};
                }
            }
        }
    }
    if(DockingFound==false){
        std::cerr << "Error: No Docking Station Was Found." << houseFilePath << std::endl;
        return false;
    }

    // Add walls around the house
    std::vector<std::vector<char>> houseWithWalls(rows + 2, std::vector<char>(cols + 2, 'W'));
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            houseWithWalls[i + 1][j + 1] = house[i][j];
        }
    }

    // Update house and docking station
    house = std::move(houseWithWalls);
    dockingStation.first += 1;
    dockingStation.second += 1;

    // Initialize shared position
    sharedPosition = std::make_shared<std::pair<std::size_t, std::size_t>>(dockingStation);
    // Initialize sensors
    concreteWallSensor = std::make_unique<ConcreteWallSensor>(house, sharedPosition);
    concreteDirtSensor = std::make_unique<ConcreteDirtSensor>(house, sharedPosition);
    concreteBatteryMeter = std::make_unique<ConcreteBatteryMeter>(maxBattery);

    wallsSensor = concreteWallSensor.get();
    dirtSensor = concreteDirtSensor.get();
    batteryMeter = concreteBatteryMeter.get();

    return true;
}

void MySimulator::setAlgorithm(MyAlgorithm& algo,Logger& logger) {
    algorithm = &algo;
    algo.setLogger(logger);
    algo.setMaxSteps(maxSteps);
    algo.setWallsSensor(*wallsSensor);
    algo.setDirtSensor(*dirtSensor);
    algo.setBatteryMeter(*batteryMeter);
    algo.setInitialPosition(sharedPosition);
    this->logger = &logger;
}
// Function to write steps history to JSON file(for the visualize.py)
void MySimulator::writeStepsHistory(const std::string& filename, const std::vector<char>& stepLog, const std::pair<std::size_t, std::size_t>& dockingStation) {
    std::ofstream outFile(filename);
    outFile << "[\n";
    outFile << "  {\"x\": " << dockingStation.first << ", \"y\": " << dockingStation.second << "}";

    std::pair<std::size_t, std::size_t> currentPos = dockingStation;
    for (char step : stepLog) {
        switch (step) {
            case 'N':
                currentPos.first -= 1;
                break;
            case 'E':
                currentPos.second += 1;
                break;
            case 'S':
                currentPos.first += 1;
                break;
            case 'W':
                currentPos.second -= 1;
                break;
            case 's':
            case 'F':
                break;
        }
        outFile << ",\n  {\"x\": " << currentPos.first << ", \"y\": " << currentPos.second << "}";
    }
    outFile << "\n]\n";
    outFile.close();
}

// Function to write house matrix to JSON file(for the visualize.py)
void MySimulator::writeHouseMatrix(const std::string& filename) {
    std::ofstream outFile(filename);
    outFile << "{\n";
    outFile << "  \"maxSteps\": " << maxSteps << ",\n";
    outFile << "  \"maxBattery\": " << maxBattery << ",\n";
    outFile << "  \"house\": [\n";
    for (std::size_t i = 0; i < house.size(); ++i) {
        outFile << "    [";
        for (std::size_t j = 0; j < house[i].size(); ++j) {
            if (house[i][j] == 'W') {
                outFile << -1;
            } else if (house[i][j] >= '1' && house[i][j] <= '9') {
                outFile << house[i][j] - '0';
            } else if (house[i][j] == 'D') {
                outFile << 0;
            } else {
                outFile << 0;
            }
            if (j < house[i].size() - 1) {
                outFile << ", ";
            }
        }
        outFile << "]";
        if (i < house.size() - 1) {
            outFile << ",\n";
        }
    }
    outFile << "\n  ]\n";
    outFile << "}\n";
    outFile.close();
}



void MySimulator::run() {
    logger->log(Logger::INFO, "Simulation started");
    std::size_t steps = 0;
    std::size_t dirtRemaining = 0;
    bool isFinished = false;
    bool isDead = false;
    std::pair<std::size_t, std::size_t> position = dockingStation;
    std::vector<char> stepLog;

    // Calculate initial dirt
    for (const auto& row : house) {
        for (char cell : row) {
            if (cell >= '1' && cell <= '9') {
                dirtRemaining += (cell - '0');
            }
        }
    }
    writeHouseMatrix("initial_house.json");
    while (steps < maxSteps && !isFinished && !isDead) {
        Step nextStep = algorithm->nextStep();

        logger->log(Logger::INFO, "Battery State: " + std::to_string(batteryMeter->getBatteryState()));
        logger->log(Logger::INFO, "Current Position: (" + std::to_string(position.first) + ", " + std::to_string(position.second) + ")");
        logger->log(Logger::INFO, "Overall Steps: " + std::to_string(steps));
        std::string houseState;
        for (const auto& row : house) {
            for (char cell : row) {
                houseState += cell;
                houseState += ' ';
            }
            houseState += '\n';
        }
        logger->log(Logger::INFO, "House State:\n" + houseState);

        switch (nextStep) {
            case Step::North:
                position.first -= 1;
                stepLog.push_back('N');
                concreteBatteryMeter->setBatteryState(concreteBatteryMeter->getBatteryState()-1);
                break;
            case Step::East:
                position.second += 1;
                stepLog.push_back('E');
                concreteBatteryMeter->setBatteryState(concreteBatteryMeter->getBatteryState()-1);
                break;
            case Step::South:
                position.first += 1;
                stepLog.push_back('S');
                concreteBatteryMeter->setBatteryState(concreteBatteryMeter->getBatteryState()-1);
                break;
            case Step::West:
                position.second -= 1;
                stepLog.push_back('W');
                concreteBatteryMeter->setBatteryState(concreteBatteryMeter->getBatteryState()-1);
                break;
            case Step::Stay:
                stepLog.push_back('s');
                if (dirtSensor->dirtLevel() > 0) {
                    concreteDirtSensor->decrementDirt();
                    dirtRemaining -= 1;
                }
                if(position==dockingStation){//this means we are charging the vacuum
                    double chargingValue = static_cast<double>(maxBattery) / 20.0;
                    std::size_t roundedChargingValue = static_cast<std::size_t>(std::ceil(chargingValue));
                    std::size_t newBattery = concreteBatteryMeter->getBatteryState() + roundedChargingValue;
                    //std::size_t newBattery=concreteBatteryMeter->getBatteryState()+maxBattery/20;
                    newBattery=std::min(maxBattery,newBattery);
                    concreteBatteryMeter->setBatteryState(newBattery);
                }
                if(position!=dockingStation){
                    concreteBatteryMeter->setBatteryState(concreteBatteryMeter->getBatteryState()-1);
                }
                break;
            case Step::Finish:
                stepLog.push_back('F');
                isFinished = true;
                break;
        }

        steps += 1;

        if (batteryMeter->getBatteryState() <= 0 && position!=dockingStation) {
            isDead=true;
        }

    }
    writeStepsHistory("steps_history.json", stepLog, dockingStation);

    logger->log(Logger::INFO, "Battery State: " + std::to_string(batteryMeter->getBatteryState()));
    logger->log(Logger::INFO, "Current Position: (" + std::to_string(position.first) + ", " + std::to_string(position.second) + ")");
    logger->log(Logger::INFO, "Overall Steps: " + std::to_string(steps));
    std::string houseState;
    for (const auto& row : house) {

        for (char cell : row) {
            houseState += cell;
            houseState += ' ';
        }
        houseState += '\n';
    }
    logger->log(Logger::INFO, "House State:\n" + houseState);
    //std::string outputFileName = "output_" + houseName + ".txt";
    // Extract the base name of the input file
    std::string baseFileName = inputFileName.substr(inputFileName.find_last_of("/\\") + 1);
    std::string outputFileName = "output_" + baseFileName;//the input file name has already .txt
    std::ofstream outputFile(outputFileName);

    if (outputFile.is_open()) {
        outputFile << "NumSteps = " << steps << "\n";
        outputFile << "DirtLeft = " << dirtRemaining << "\n";
        outputFile << "Status = " << (isFinished ? "FINISHED" : (isDead ? "DEAD" : "WORKING")) << "\n";
        outputFile << "Steps: ";
        for (char step : stepLog) {
            outputFile << step;
        }
        outputFile.close();
    }
    if  (isFinished){
        logger->log(Logger::INFO, "House Cleaned.");
    }
    else{
        if(isDead){logger->log(Logger::WARNING, "Vacuum Dead.");}
        else{logger->log(Logger::WARNING, "Reached Maximum Number of Steps.");}
    }
    logger->log(Logger::INFO, "Simulation Finished.");
}

