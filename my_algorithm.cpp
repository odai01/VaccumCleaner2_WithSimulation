#include "my_algorithm.h"
#include <queue>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iostream>

MyAlgorithm::MyAlgorithm()
    : stepsQueue(), stepsToDockingQueue() {
}
// Set the maximum steps allowed for the algorithm
void MyAlgorithm::setMaxSteps(std::size_t maxSteps) {
    this->maxSteps = maxSteps;
}

// Set the wall sensor
void MyAlgorithm::setWallsSensor(const WallsSensor& wallsSensor) {
    this->wallsSensor = &wallsSensor;
}

// Set the dirt sensor
void MyAlgorithm::setDirtSensor(const DirtSensor& dirtSensor) {
    this->dirtSensor = &dirtSensor;
}

// Set the battery meter(sensor)
void MyAlgorithm::setBatteryMeter(const BatteryMeter& batteryMeter) {
    this->batteryMeter = &batteryMeter;
}

// Set the initial position and docking station
void MyAlgorithm::setInitialPosition(std::shared_ptr<std::pair<std::size_t, std::size_t>> initialPosition) {
    position = initialPosition;
    dockingStation = *initialPosition; // we pass the initial position to be the same as the docking
    visited.clear(); // Clear visited map to make sure it starts clean when running bfs
}
// Set the logger 
void MyAlgorithm::setLogger(Logger& logger) {
    this->logger = &logger;
}

// Determine the next step for the vacuum cleaner and returns it for the simulator
Step MyAlgorithm::nextStep() {
    // Calculate the shortest path to the docking station, to check later if need charging
    std::vector<Step> pathToDocking = bfsToDocking();
    logger->log(Logger::INFO, "Distance From Docking -> " + std::to_string(pathToDocking.size()));
    // Check if the battery level is low, and that we are not in the way to the docking, then return it to docking
    if ((stepsToDockingQueue.empty()==true) && (batteryMeter->getBatteryState() <= pathToDocking.size()+1)) {
        while (!stepsQueue.empty()) {
            stepsQueue.pop();
        }
        logger->log(Logger::INFO, "Need Charging, Decided To Go Back To The Docking.");
        for (Step step : pathToDocking) {
            stepsToDockingQueue.push(step);
        }
        for(int i=0;i<20;i++){
            stepsToDockingQueue.push(Step::Stay);
        }

    }
    // if the steps queue to the docking is not empty then get the next step in the way
    if (!stepsToDockingQueue.empty()) {
        Step next = stepsToDockingQueue.front();
        if(pathToDocking.size()==0 ){
            if(next!=Step::Finish){logger->log(Logger::INFO, "Charging..."); }
        }
        else{logger->log(Logger::INFO, "Moving Towards The Docking.");}
        stepsToDockingQueue.pop();
        *position = applyStep(*position, next);  
        return next;
    }
    
    // If the steps queue is empty, perform BFS to find the nearest dirt
    if (stepsQueue.empty()) {
        logger->log(Logger::INFO, "Looking For Dirt.");
        std::vector<Step> path = bfs();
        for (Step step : path) {
            if(step==Step::Finish){
                logger->log(Logger::INFO, "Finished And Going Back To Docking ");
                for (Step step : pathToDocking) {
                    stepsToDockingQueue.push(step);
                }
                stepsToDockingQueue.push(Step::Finish);
                break;
            }
            stepsQueue.push(step);
        }
    }

    // If the steps queue is not empty, get the next step
    if (!stepsQueue.empty()) {
        logger->log(Logger::INFO, "On My Way To Clean.");
        Step next = stepsQueue.front();
        stepsQueue.pop();
        *position = applyStep(*position, next);  
        return next;
    }

    // If no steps are available, stay in place
    return  Step::Stay;
}

// Perform BFS to find the nearest dirt spot
std::vector<Step> MyAlgorithm::bfs() {
    visited.clear();
    // Queue for BFS: stores current position and the path taken to reach it
    std::queue<std::pair<std::pair<std::size_t, std::size_t>, std::vector<Step>>> queue;
    if(dirtSensor->dirtLevel() > 0){
        queue.push({*position, {Step::Stay}});
    }
    else{
        queue.push({*position, {}});
    }
    
    std::pair<std::size_t, std::size_t> startPosition=*position;
    // While there are positions to process in the queue
    while (!queue.empty()) {
        auto [currentPos, path] = queue.front();
        queue.pop();

        *position = currentPos;

        // If the position has already been visited, skip it
        if (visited[positionToString(currentPos)]) {
            continue;
        }
        visited[positionToString(currentPos)] = true;

        // If the current position has dirt, return the path to it
        if (dirtSensor->dirtLevel() > 0) {
            *position=startPosition;
            return path;
        }

        // Possible directions to move
        std::vector<std::pair<Step, std::pair<std::size_t, std::size_t>>> directions = {
            {Step::North, {currentPos.first - 1, currentPos.second}},
            {Step::East, {currentPos.first, currentPos.second + 1}},
            {Step::South, {currentPos.first + 1, currentPos.second}},
            {Step::West, {currentPos.first, currentPos.second - 1}}
        };

        // For each direction, check if it's not a wall and add it to the queue if not
        for (auto& [step, newPos] : directions) {
            if (!wallsSensor->isWall(static_cast<Direction>(step))) {
                std::vector<Step> newPath = path;
                newPath.push_back(step);
                queue.push({newPos, newPath});
            }
        }
    }
    // if we did not find any dirt the we return Finish, since the vacuum cleaned all the reachable dirts
    *position=startPosition;
    return {{Step::Finish}};
}

// Perform BFS to find the shortest path to the docking station
std::vector<Step> MyAlgorithm::bfsToDocking() {
    visited.clear();
    // Queue for BFS: stores current position and the path taken to reach it
    std::queue<std::pair<std::pair<std::size_t, std::size_t>, std::vector<Step>>> queue;
    queue.push({*position, {}});
    std::pair<std::size_t, std::size_t> startPosition=*position;
    // While there are positions to process in the queue
    while (!queue.empty()) {
        auto [currentPos, path] = queue.front();
        queue.pop();

        *position = currentPos;

        // If the position has already been visited, skip it
        if (visited[positionToString(currentPos)]) {
            continue;
        }
        visited[positionToString(currentPos)] = true;

        // If the current position is the docking station, return the path to it
        if (currentPos == dockingStation) {
            *position=startPosition;
            return path;
        }

        // Possible directions to move
        std::vector<std::pair<Step, std::pair<std::size_t, std::size_t>>> directions = {
            {Step::North, {currentPos.first - 1, currentPos.second}},
            {Step::East, {currentPos.first, currentPos.second + 1}},
            {Step::South, {currentPos.first + 1, currentPos.second}},
            {Step::West, {currentPos.first, currentPos.second - 1}}
        };

        // For each direction, check if it's not a wall and add it to the queue if not
        for (auto& [step, newPos] : directions) {
            if (!wallsSensor->isWall(static_cast<Direction>(step))) {
                std::vector<Step> newPath = path;
                newPath.push_back(step);
                queue.push({newPos, newPath});
            }
        }
    }
    // Return an empty path if no path to the docking station is found
    *position=startPosition;
    return {};
}

// Convert position to string for use in the visited map
std::string MyAlgorithm::positionToString(const std::pair<std::size_t, std::size_t>& pos) const {
    return std::to_string(pos.first) + "," + std::to_string(pos.second);
}

// Apply a step to a given position and return the new position
std::pair<std::size_t, std::size_t> MyAlgorithm::applyStep(const std::pair<std::size_t, std::size_t>& pos, Step step) const {
    switch (step) {
        case Step::North:
            return {pos.first - 1, pos.second};
        case Step::East:
            return {pos.first, pos.second + 1};
        case Step::South:
            return {pos.first + 1, pos.second};
        case Step::West:
            return {pos.first, pos.second - 1};
        case Step::Stay:
            return pos;
        case Step::Finish:
            return pos;
    }
    return pos;
}
