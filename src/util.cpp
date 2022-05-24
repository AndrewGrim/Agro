#include "util.hpp"
#include "common/color.hpp"

static void printLog(String message_type, String message, String text, Color color = Color("#ffffff")) {
    printf(
        "\033[38;2;%d;%d;%dm%s::%s %s\033[0m\n",
        ((i32)(color.r * 255)),
        ((i32)(color.g * 255)),
        ((i32)(color.b * 255)),
        message_type.data(),
        message.data(),
        text.data());
}

void info(String message, String text) {
    printLog("INFO", message, text, Color("#ff00ff"));
}

void warn(String message, String text) {
    printLog("WARN", message, text, Color("#ffff00"));
}

void fail(String message, String text) {
    printLog("FAIL", message, text, Color("#ff0000"));
    exit(1);
}
