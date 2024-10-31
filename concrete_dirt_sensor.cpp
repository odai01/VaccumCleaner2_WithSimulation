#include "concrete_dirt_sensor.h"

ConcreteDirtSensor::ConcreteDirtSensor(const std::vector<std::vector<char>>& house, std::shared_ptr<std::pair<std::size_t, std::size_t>>& position)
    : house_(const_cast<std::vector<std::vector<char>>&>(house)), position_(position) {}

int ConcreteDirtSensor::dirtLevel() const {
    char cell = house_[position_->first][position_->second];
    return (cell >= '0' && cell <= '9') ? cell - '0' : 0;
}

void ConcreteDirtSensor::setPosition(const std::pair<std::size_t, std::size_t>& pos) {
    *position_ = pos;
}

void ConcreteDirtSensor::decrementDirt() {
    char& cell = house_[position_->first][position_->second];
    if (cell >= '1' && cell <= '9') {
        cell -= 1; 
    }
}
