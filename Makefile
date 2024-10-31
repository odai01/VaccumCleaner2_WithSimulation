# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic

# Include directories
INCLUDES = -I.

# Source files
SRCS = my_simulator.cpp concrete_wall_sensor.cpp concrete_dirt_sensor.cpp concrete_battery_meter.cpp my_algorithm.cpp main.cpp Logger.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = myrobot

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
