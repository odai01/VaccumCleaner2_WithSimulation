#ifndef MY_ALGORITHM_H_
#define MY_ALGORITHM_H_

#include "abstract_algorithm.h"
#include "wall_sensor.h"
#include "dirt_sensor.h"
#include "battery_meter.h"
#include "enums.h"
#include <queue>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <utility>
#include "Logger.h"

class MyAlgorithm : public AbstractAlgorithm {
public:
    MyAlgorithm();
    void setMaxSteps(std::size_t maxSteps) override;
    void setWallsSensor(const WallsSensor& wallsSensor) override;
    void setDirtSensor(const DirtSensor& dirtSensor) override;
    void setBatteryMeter(const BatteryMeter& batteryMeter) override;
    void setInitialPosition(std::shared_ptr<std::pair<std::size_t, std::size_t>> initialPosition) ;
    void setLogger(Logger& logger);
    Step nextStep() override;

private:
    std::size_t maxSteps;
    const WallsSensor* wallsSensor;
    const DirtSensor* dirtSensor;
    const BatteryMeter* batteryMeter;
    Logger* logger;
    std::shared_ptr<std::pair<std::size_t, std::size_t>> position;
    std::pair<std::size_t, std::size_t> dockingStation;
    std::unordered_map<std::string, bool> visited;
    std::queue<Step> stepsQueue;
    std::queue<Step> stepsToDockingQueue;

    std::vector<Step> bfs();
    std::vector<Step> bfsToDocking();
    std::string positionToString(const std::pair<std::size_t, std::size_t>& pos) const;
    std::pair<std::size_t, std::size_t> applyStep(const std::pair<std::size_t, std::size_t>& pos, Step step) const;
};

#endif  // MY_ALGORITHM_H_
