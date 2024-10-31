#ifndef CONCRETE_BATTERY_METER_H_
#define CONCRETE_BATTERY_METER_H_

#include "battery_meter.h"

class ConcreteBatteryMeter : public BatteryMeter {
public:
    ConcreteBatteryMeter(std::size_t batteryState) : batteryState_(batteryState) {}

    std::size_t getBatteryState() const override;
    void setBatteryState(std::size_t batteryState);

private:
    std::size_t batteryState_;
};

#endif  
