#include "concrete_battery_meter.h"

std::size_t ConcreteBatteryMeter::getBatteryState() const {
    return batteryState_;
}

void ConcreteBatteryMeter::setBatteryState(std::size_t batteryState) {
    batteryState_ = batteryState;
}
