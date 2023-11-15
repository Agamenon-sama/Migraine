#pragma once

#include <string>

/**
 * @brief Provides a way to open Message boxes and log messages
*/
class MessageBox {
public:
    static void info(const std::string &message, const std::string &title = "Information");
    static void warning(const std::string &message, const std::string &title = "Warning");
    static void error(const std::string &message, const std::string &title = "Error");
};