#include "concrete_wall_sensor.h"

ConcreteWallSensor::ConcreteWallSensor(const std::vector<std::vector<char>>& house, std::shared_ptr<std::pair<std::size_t, std::size_t>>& position)
    : house_(house), position_(position) {}

bool ConcreteWallSensor::isWall(Direction d) const {
    std::size_t x = position_->first;
    std::size_t y = position_->second;
    switch (d) {
        case Direction::North:
            return house_[x - 1][y] == 'W';
        case Direction::East:
            return house_[x][y + 1] == 'W';
        case Direction::South:
            return house_[x + 1][y] == 'W';
        case Direction::West:
            return house_[x][y - 1] == 'W';
    }
    return false; //will never reach here
}
