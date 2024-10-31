#ifndef CONCRETE_DIRT_SENSOR_H
#define CONCRETE_DIRT_SENSOR_H

#include "dirt_sensor.h"
#include <vector>
#include <utility>
#include <memory>

class ConcreteDirtSensor : public DirtSensor {
public:
    ConcreteDirtSensor(const std::vector<std::vector<char>>& house, std::shared_ptr<std::pair<std::size_t, std::size_t>>& position);
    virtual void setPosition(const std::pair<std::size_t, std::size_t>& pos) ;
    virtual int dirtLevel() const override;
    void decrementDirt();

private:
    std::vector<std::vector<char>>& house_;
    std::shared_ptr<std::pair<std::size_t, std::size_t>> position_;
};

#endif 
