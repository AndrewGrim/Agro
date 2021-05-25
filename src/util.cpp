#include "util.hpp"
#include "common/color.hpp"

static void printLog(std::string message_type, std::string message, std::string text, Color color = Color("#ffffff")) {
    printf(
        "\033[38;2;%d;%d;%dm%s::%s %s\033[0m\n",
        ((int)(color.r * 255)),
        ((int)(color.g * 255)),
        ((int)(color.b * 255)),
        message_type.c_str(),
        message.c_str(),
        text.c_str()
    );
}

void warn(std::string message, std::string text) {
    printLog("WARNING", message, text, Color("#ffff00"));
}

void error(std::string message, std::string text) {
    printLog("ERROR", message, text, Color("#ff0000"));
    exit(1);
}
