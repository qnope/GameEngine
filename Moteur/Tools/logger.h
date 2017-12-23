#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <string_view>

class Logger {
public:
    static Logger &getInstance();

    void log(std::string_view view);

private:
    Logger() = default;

private:
};

inline std::string_view to_string(const std::exception &exception) {
    return exception.what();
}

inline std::string_view to_string(const std::string &string) {
    return string;
}

template<typename Type>
inline void log(const Type &log) {
    //using std::to_string;
    Logger::getInstance().log(to_string(log));
}

#endif // LOGGER_H
