#ifndef CONCRETE_WALL_SENSOR_H_
#define CONCRETE_WALL_SENSOR_H_

#include "wall_sensor.h"
#include <vector>
#include <utility>
#include <memory>

class ConcreteWallSensor : public WallsSensor {
public:
    ConcreteWallSensor(const std::vector<std::vector<char>>& house, std::shared_ptr<std::pair<std::size_t, std::size_t>>& position);

    bool isWall(Direction d) const override;

    void setPosition(const std::pair<std::size_t, std::size_t>& position) {
        *position_ = position;
    }

private:
    const std::vector<std::vector<char>>& house_;
    std::shared_ptr<std::pair<std::size_t, std::size_t>> position_;
};

#endif
