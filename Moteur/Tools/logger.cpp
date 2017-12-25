#include "Logger.h"
#include <iostream>

Logger &Logger::getInstance() {
    static Logger logger;
    return logger;
}

void Logger::log(std::string_view view) {
    std::cerr << view << std::endl;
}
