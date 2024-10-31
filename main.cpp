#include "my_simulator.h"
#include "my_algorithm.h"
#include <iostream>
#include "Logger.h"

int main(int argc, char** argv) {
    Logger logger("vacuum.log");
    logger.clearLog();
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <house_file_path>" << std::endl;
        return 1;
    }
    std::string houseFilePath = argv[1];
    
    MySimulator simulator;
    if (!simulator.readHouseFile(houseFilePath)) {
        std::cerr << "Failed to read house file." << std::endl;
        return 1;
    }

    MyAlgorithm algo;
    simulator.setAlgorithm(algo,logger);
    simulator.run();

    return 0;
}
