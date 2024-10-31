# Assignment 2 - Vacuum Cleaner Simulation
## Contributors
- [Odai Agbaria: 212609440, Mohammad Mahamid: 322776063]

## Overview
This project is a simulation of a vacuum cleaner that navigates a house to clean dirt. The vacuum cleaner's path and behavior are determined by a cleaning algorithm(based on BFS). The simulation reads input from a text file describing the house layout, and it outputs the cleaning steps and results to a text file. Additionally, a Pygame-based visualization is provided to animate the cleaning process, and a  logger file to debug the process.

## Approach
The project consists of several components:(there is also the abstract interfaces provided by the skeleton file)
1. **MyAlgorithm Class**: This class represents the algorithm that guides the vacuum cleaner through the house, cleaning dirt. This time, the algorithm uses Breadth-First Search (BFS) for efficient navigation.
2. **Sensor Classes**: 
   - **ConcreteWallSensor**: Detects walls in the house.
   - **ConcreteDirtSensor**: Detects dirt levels in the house.
   - **ConcreteBatteryMeter**: Manages the battery state of the vacuum cleaner.
3. **Input Parsing**: The `MySimulator` class reads and parses the house layout, docking station, maximum battery capacity, and maximum iterations from an input file.
4. **Output Writing**: The `MySimulator` class writes the cleaning steps and results to an output file.
5. **Logger Class**: Captures detailed runtime information for debugging and analysis.
6. **Visualization**: A Pygame-based visualization that animates the cleaning process, including battery and steps progress bars.

## Input and Output Format(as specified in the instruction)
### Input Format
The input file should be structured as follows:
1. **Line 1**: House name/description (can be ignored by the simulation).
2. **Line 2**: MaxSteps for the simulation on this house, formatted as `MaxSteps = <NUM>`.
3. **Line 3**: MaxBattery (in steps) for the simulation on this house, formatted as `MaxBattery = <NUM>`.
4. **Line 4**: Number of Rows in house, formatted as `Rows = <NUM>`.
5. **Line 5**: Number of Cols in house, formatted as `Cols = <NUM>`.
6. **Lines 6 and on**: The house matrix, which includes:
   - `-1` represents a wall.
   - `0` represents an empty (clean) space.
   - Positive integers (1-9) represent the amount of dirt in that cell.
   - `D` represents the docking station.

Notes:
- All values in lines 2-5 may or may not have spaces around the `=` sign.
- If there are missing lines, they should be filled in with empty lines; if there are missing columns, they should be filled in with spaces.
- The house is assumed to be surrounded by walls on the boundaries beyond the provided rows and cols.

### Output Format
The output file will contain:
1. **NumSteps**: The total number of steps performed.
2. **DirtLeft**: The amount of dirt left in the house.
3. **Status**: The status of the vacuum cleaner (`FINISHED/WORKING/DEAD`).
4. **Steps**: The list of characters representing the steps performed by the vacuum cleaner, in one line without spaces. Valid characters are `NESWsF` (North, East, South, West, Stay, Finish).


## How to Build and Run
1. **Building the Project**: First, run: `make`
    (Ensure you have `g++` version 11.4 or above installed on your system.)
2. **Running the Project**: Then, run: `./myrobot input_a.txt`
    This will produce an output file named `output_input_a.txt`, two JSON files, and a log file named `vacuum.log`.
3. **Running the Visualization**: Then, run: `python visualize.py`
    (Ensure you have Python and Pygame installed on your system)

## Logger
The project includes a robust logging mechanism that captures detailed runtime information for debugging and analysis. The log file, `vacuum.log`, records various events and states such as battery level, current position, distance from the docking station, and overall steps. Additionally, it logs specific actions like moving towards the docking station, charging, cleaning, and finishing the simulation.

## Visualization Enhancements
- **Battery Status**: The battery level is represented by a green bar at the top left corner that fills up as the battery charges and decreases as it depletes.
- **Steps Progress**: The steps progress bar is located at the top right corner, indicating the proportion of steps taken relative to the maximum steps allowed, with a gradient color change.
- **Logger**: The simulation now includes a logger to capture detailed runtime information for debugging and analysis.

## There is also HLD pdf file which includes UML Class diagram and UML Sequence Diagram.

## Hope you enjoy the simulation and the logger file
