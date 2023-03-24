
#ifndef MACROS_HPP
#define MACROS_HPP

#include <iostream>

/**
 * This file is a collection of useful macros.
 */

// Color macros
#define RESET "\033[0m"

// Background
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"
#define BG_DEFAULT "\033[49m"
#define BG_GRAY "\033[100m"
#define BG_LIGHT_RED "\033[101m"
#define BG_LIGHT_GREEN "\033[102m"
#define BG_LIGHT_YELLOW "\033[103m"
#define BG_LIGHT_BLUE "\033[104m"
#define BG_LIGHT_MAGENTA "\033[105m"
#define BG_LIGHT_CYAN "\033[106m"
#define BG_LIGHT_WHITE "\033[107m"

// Foreground
#define FG_BLACK "\033[30m"
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"
#define FG_DEFAULT "\033[39m"
#define FG_GRAY "\033[90m"
#define FG_LIGHT_RED "\033[91m"
#define FG_LIGHT_GREEN "\033[92m"
#define FG_LIGHT_YELLOW "\033[93m"
#define FG_LIGHT_BLUE "\033[94m"
#define FG_LIGHT_MAGENTA "\033[95m"
#define FG_LIGHT_CYAN "\033[96m"
#define FG_LIGHT_WHITE "\033[97m"


// Text attributes
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define UNDERLINED "\033[4m"
#define BLINK "\033[5m"
#define REVERSE "\033[7m"
#define HIDDEN "\033[8m"

// Text reset
#define BOLD_OFF "\033[21m"
#define DIM_OFF "\033[22m"
#define UNDERLINED_OFF "\033[24m"
#define BLINK_OFF "\033[25m"
#define REVERSE_OFF "\033[27m"
#define HIDDEN_OFF "\033[28m"

// Other
#define CLEAR_SCREEN "\033[2J"
#define CLEAR_LINE "\033[2K"
#define CURSOR_HOME "\033[H"
#define CURSOR_UP "\033[A"
#define CURSOR_DOWN "\033[B"
#define CURSOR_RIGHT "\033[C"
#define CURSOR_LEFT "\033[D"

#define UNUSED(x) (void)(x)


#endif //MACROS_HPP
