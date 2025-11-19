#pragma once

// Reset
#define COLOR_RESET "\e[0m"

// Normal
#define COLOR_BLACK "\e[0;30m"
#define COLOR_RED "\e[0;31m"
#define COLOR_GREEN "\e[0;32m"
#define COLOR_YELLOW "\e[0;33m"
#define COLOR_BLUE "\e[0;34m"
#define COLOR_PURPLE "\e[0;35m"
#define COLOR_CYAN "\e[0;36m"
#define COLOR_WHITE "\e[0;37m"

// Bold
#define COLOR_BLACK_BOLD "\e[1;30m"
#define COLOR_RED_BOLD "\e[1;31m"
#define COLOR_GREEN_BOLD "\e[1;32m"
#define COLOR_YELLOW_BOLD "\e[1;33m"
#define COLOR_BLUE_BOLD "\e[1;34m"
#define COLOR_PURPLE_BOLD "\e[1;35m"
#define COLOR_CYAN_BOLD "\e[1;36m"
#define COLOR_WHITE_BOLD "\e[1;37m"

// Underline
#define COLOR_BLACK_UNDERLINE "\e[4;30m"
#define COLOR_RED_UNDERLINE "\e[4;31m"
#define COLOR_GREEN_UNDERLINE "\e[4;32m"
#define COLOR_YELLOW_UNDERLINE "\e[4;33m"
#define COLOR_BLUE_UNDERLINE "\e[4;34m"
#define COLOR_PURPLE_UNDERLINE "\e[4;35m"
#define COLOR_CYAN_UNDERLINE "\e[4;36m"
#define COLOR_WHITE_UNDERLINE "\e[4;37m"

// Background
#define COLOR_BLACK_BACKGROUND "\e[40m"
#define COLOR_RED_BACKGROUND "\e[41m"
#define COLOR_GREEN_BACKGROUND "\e[42m"
#define COLOR_YELLOW_BACKGROUND "\e[43m"
#define COLOR_BLUE_BACKGROUND "\e[44m"
#define COLOR_PURPLE_BACKGROUND "\e[45m"
#define COLOR_CYAN_BACKGROUND "\e[46m"
#define COLOR_WHITE_BACKGROUND "\e[47m"

// High Intensity
#define COLOR_BLACK_HIGHINTENSITY "\e[0;90m"
#define COLOR_RED_HIGHINTENSITY "\e[0;91m"
#define COLOR_GREEN_HIGHINTENSITY "\e[0;92m"
#define COLOR_YELLOW_HIGHINTENSITY "\e[0;93m"
#define COLOR_BLUE_HIGHINTENSITY "\e[0;94m"
#define COLOR_PURPLE_HIGHINTENSITY "\e[0;95m"
#define COLOR_CYAN_HIGHINTENSITY "\e[0;96m"
#define COLOR_WHITE_HIGHINTENSITY "\e[0;97m"

// Bold High Intensity
#define COLOR_BLACK_BOLD_HIGHINTENSITY "\e[1;90m"
#define COLOR_RED_BOLD_HIGHINTENSITY "\e[1;91m"
#define COLOR_GREEN_BOLD_HIGHINTENSITY "\e[1;92m"
#define COLOR_YELLOW_BOLD_HIGHINTENSITY "\e[1;93m"
#define COLOR_BLUE_BOLD_HIGHINTENSITY "\e[1;94m"
#define COLOR_PURPLE_BOLD_HIGHINTENSITY "\e[1;95m"
#define COLOR_CYAN_BOLD_HIGHINTENSITY "\e[1;96m"
#define COLOR_WHITE_BOLD_HIGHINTENSITY "\e[1;97m"

// High Intensity Background
#define COLOR_BLACK_BACKGROUND_HIGHINTENSITY "\e[0;100m"
#define COLOR_RED_BACKGROUND_HIGHINTENSITY "\e[0;101m"
#define COLOR_GREEN_BACKGROUND_HIGHINTENSITY "\e[0;102m"
#define COLOR_YELLOW_BACKGROUND_HIGHINTENSITY "\e[0;103m"
#define COLOR_BLUE_BACKGROUND_HIGHINTENSITY "\e[0;104m"
#define COLOR_PURPLE_BACKGROUND_HIGHINTENSITY "\e[0;105m"
#define COLOR_CYAN_BACKGROUND_HIGHINTENSITY "\e[0;106m"
#define COLOR_WHITE_BACKGROUND_HIGHINTENSITY "\e[0;107m"

#define printfc(COLOR, fmt, ...)                                               \
    do                                                                         \
    {                                                                          \
        printf("%s" fmt "%s", COLOR, ##__VA_ARGS__, COLOR_RESET);              \
    }                                                                          \
    while (0)

#define printfc_prefix(COLOR, prefix, fmt, ...)                                \
    do                                                                         \
    {                                                                          \
        printf("%s" prefix "%s" fmt, COLOR, COLOR_RESET, ##__VA_ARGS__);       \
    }                                                                          \
    while (0)

#define printfc_prefix_file_location](COLOR, prefix, fmt, ...)                                     \
    do                                                                         \
    {                                                                          \
        printf("%s" prefix "%s" fmt, COLOR, COLOR_RESET, ##__VA_ARGS__);       \
    }                                                                          \
    while (0)

#define printfc_prefix_file_location(COLOR, prefix, fmt, ...)                  \
    do                                                                         \
    {                                                                          \
        printf("%s[%s]%s  " fmt "\n\tat %s:%d\n", COLOR, prefix, COLOR_RESET,  \
               ##__VA_ARGS__, __FILE_NAME__, __LINE__);                        \
    }                                                                          \
    while (0)

#define ERROR(fmt, ...)                                                        \
    do                                                                         \
    {                                                                          \
        printfc_prefix_file_location(COLOR_RED, "ERROR", fmt, ##__VA_ARGS__);  \
    }                                                                          \
    while (0)

#define ERROR2(module, fmt, ...)                                               \
    do                                                                         \
    {                                                                          \
        printf(module ": ");                                                   \
        printfc_prefix_file_location(COLOR_RED, "ERROR", fmt, ##__VA_ARGS__);  \
    }                                                                          \
    while (0)
