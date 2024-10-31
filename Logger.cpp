#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>

Logger::Logger(const std::string& filename) {
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(Level level, const std::string& message) {
    if (logFile.is_open()) {
        logFile << getTimestamp() << " [" << levelToString(level) << "] " << message << std::endl;
    }
}

std::string Logger::getTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
void Logger::clearLog() {
    std::lock_guard<std::mutex> guard(logMutex);
    if (logFile.is_open()) {
        logFile.close();
    }
    logFile.open("vacuum.log", std::ios_base::trunc);  // Open the file in trunc mode to clear its contents
    if (!logFile.is_open()) {
        std::cerr << "Failed to clear log file: vacuum_robot.log" << std::endl;
    }
}
