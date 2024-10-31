#ifndef MY_SIMULATOR_H
#define MY_SIMULATOR_H

#include <vector>
#include <memory>
#include <string>
#include <utility>
#include "concrete_wall_sensor.h"
#include "concrete_dirt_sensor.h"
#include "concrete_battery_meter.h"
#include "my_algorithm.h"
#include "Logger.h"

class MySimulator {
public:
    bool readHouseFile(const std::string& houseFilePath);
    void setAlgorithm(MyAlgorithm& algo,Logger& logger);
    void writeStepsHistory(const std::string& filename, const std::vector<char>& stepLog, const std::pair<std::size_t, std::size_t>& dockingStation);
    void writeHouseMatrix(const std::string& filename);
    void run();

private:
    std::string houseName;
    std::string inputFileName;
    std::size_t maxSteps;
    std::size_t maxBattery;
    std::size_t rows;
    std::size_t cols;
    Logger* logger; 
    std::pair<std::size_t, std::size_t> dockingStation;
    std::pair<std::size_t, std::size_t> position;
    std::shared_ptr<std::pair<std::size_t, std::size_t>> sharedPosition;
    std::vector<std::vector<char>> house;

    std::unique_ptr<ConcreteWallSensor> concreteWallSensor;
    std::unique_ptr<ConcreteDirtSensor> concreteDirtSensor;
    std::unique_ptr<ConcreteBatteryMeter> concreteBatteryMeter;

    WallsSensor* wallsSensor;
    DirtSensor* dirtSensor;
    BatteryMeter* batteryMeter;
    MyAlgorithm* algorithm;
};

#endif // MY_SIMULATOR_H
