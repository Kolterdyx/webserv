
#ifndef MACROS_HPP
#define MACROS_HPP

#include <iostream>

/**
 * This file is a collection of useful macros.
 */

// Color macros
#define RESET std::string("\033[0m")

// Background
#define BG_BLACK std::string("\033[40m")
#define BG_RED std::string("\033[41m")
#define BG_GREEN std::string("\033[42m")
#define BG_YELLOW std::string("\033[43m")
#define BG_BLUE std::string("\033[44m")
#define BG_MAGENTA std::string("\033[45m")
#define BG_CYAN std::string("\033[46m")
#define BG_WHITE std::string("\033[47m")
#define BG_DEFAULT std::string("\033[49m")
#define BG_GRAY std::string("\033[100m")
#define BG_LIGHT_RED std::string("\033[101m")
#define BG_LIGHT_GREEN std::string("\033[102m")
#define BG_LIGHT_YELLOW std::string("\033[103m")
#define BG_LIGHT_BLUE std::string("\033[104m")
#define BG_LIGHT_MAGENTA std::string("\033[105m")
#define BG_LIGHT_CYAN std::string("\033[106m")
#define BG_LIGHT_WHITE std::string("\033[107m")

// Foreground
#define FG_BLACK std::string("\033[30m")
#define FG_RED std::string("\033[31m")
#define FG_GREEN std::string("\033[32m")
#define FG_YELLOW std::string("\033[33m")
#define FG_BLUE std::string("\033[34m")
#define FG_MAGENTA std::string("\033[35m")
#define FG_CYAN std::string("\033[36m")
#define FG_WHITE std::string("\033[37m")
#define FG_DEFAULT std::string("\033[39m")
#define FG_GRAY std::string("\033[90m")
#define FG_LIGHT_RED std::string("\033[91m")
#define FG_LIGHT_GREEN std::string("\033[92m")
#define FG_LIGHT_YELLOW std::string("\033[93m")
#define FG_LIGHT_BLUE std::string("\033[94m")
#define FG_LIGHT_MAGENTA std::string("\033[95m")
#define FG_LIGHT_CYAN std::string("\033[96m")
#define FG_LIGHT_WHITE std::string("\033[97m")


// Text attributes
#define BOLD std::string("\033[1m")
#define DIM std::string("\033[2m")
#define UNDERLINED std::string("\033[4m")
#define BLINK std::string("\033[5m")
#define REVERSE std::string("\033[7m")
#define HIDDEN std::string("\033[8m")

// Text reset
#define BOLD_OFF std::string("\033[21m")
#define DIM_OFF std::string("\033[22m")
#define UNDERLINED_OFF std::string("\033[24m")
#define BLINK_OFF std::string("\033[25m")
#define REVERSE_OFF std::string("\033[27m")
#define HIDDEN_OFF std::string("\033[28m")

// Other
#define CLEAR_SCREEN std::string("\033[2J")
#define CLEAR_LINE std::string("\033[2K")
#define CURSOR_HOME std::string("\033[H")
#define CURSOR_UP std::string("\033[A")
#define CURSOR_DOWN std::string("\033[B")
#define CURSOR_RIGHT std::string("\033[C")
#define CURSOR_LEFT std::string("\033[D")

#define UNUSED(x) (void)(x)


#endif //MACROS_HPP
