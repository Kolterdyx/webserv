//
// Created by Ciro Garcia belmonte on 3/23/23.
//

#include "util.hpp"
#include "Logger.hpp"

const int Logger::DEBUG = 0;
const int Logger::INFO = 10;
const int Logger::WARN = 20;
const int Logger::ERROR = 30;
const int Logger::CRITICAL = 40;

Logger::Logger() {
	_name = "default";
	init();
}

Logger::Logger(const std::string &name) {
	_name = name;
	init();
}

void Logger::init() {
	useFile = false;
	useStdout = true;
	_level = 0;
	_format = "[%date% %time%] [$bold$$white$%name%$default$$reset$] [%levelname%] %message%";

	_patterns["%date%"] = "%date%";
	_patterns["%time%"] = "%time%";
	_patterns["%level%"] = util::to_string(_level);
	_patterns["%name%"] = _name;

}

Logger::Logger(const Logger &copy) {
	*this = copy;
}

Logger &Logger::operator=(const Logger &copy) {
	_name = copy._name;
	_path = copy._path;
	useFile = copy.useFile;
	useStdout = copy.useStdout;
	_format = copy._format;
	_patterns = copy._patterns;
	_levelnames_color = copy._levelnames_color;
	_text_format = copy._text_format;

	return *this;
}

Logger::~Logger() {
	if (_file.is_open())
		_file.close();
}

void Logger::print(const std::string &message, int level) {
	if (level < _level)
		return;
	if (useStdout)
		std::cout << applyFormat(message, true, level) << std::endl;
	if (useFile) {
        _file.open(_path.c_str(), std::ios::out | std::ios::app);
        _file << applyFormat(message, false, level) << std::endl;
        _file.close();
    }
}

void Logger::setFormat(const std::string &format) {
	_format = format;
}

std::string Logger::applyFormat(const std::string &message, bool color, int level) {

    if (color) {
        if (level < INFO) {
            _patterns["%message%"] = FG_GRAY + message;
        } else if (level < WARN) {
            _patterns["%message%"] = message;
        } else if (level < ERROR) {
            _patterns["%message%"] = FG_YELLOW + message;
        } else if (level < CRITICAL) {
            _patterns["%message%"] = FG_RED + message;
        } else {
            _patterns["%message%"] = BG_RED + message;
        }
    } else {
        _patterns["%message%"] = message;
    }

	_patterns["%date%"] = util::datetime("%Y-%m-%d");
	_patterns["%time%"] = util::datetime("%H:%M:%S");

    std::map<int, std::string> levelnames = color ? _levelnames_color : _levelnames;
	for (std::map<int, std::string>::iterator it = levelnames.begin(); it != levelnames.end(); ++it) {
		if (level < it->first) {
			break;
		}
		_patterns["%levelname%"] = it->second;
	}

	std::string formatted = _format;
	std::string::size_type pos = 0;

    // Replace patterns
	for (std::map<std::string, std::string>::iterator it = _patterns.begin(); it != _patterns.end(); ++it) {
		pos = formatted.find(it->first);
		while (pos != std::string::npos) {
			formatted.replace(pos, it->first.length(), it->second);
			pos = formatted.find(it->first);
		}
	}

    // Replace text format patterns
	for (std::map<std::string, std::string>::iterator it = _text_format.begin(); it != _text_format.end(); ++it) {
		pos = formatted.find(it->first);
		while (pos != std::string::npos) {
			if (color)
				formatted.replace(pos, it->first.length(), it->second);
			else
				formatted.replace(pos, it->first.length(), "");
			pos = formatted.find(it->first);
		}
	}
	return formatted + (color ? RESET : "");
}

void Logger::setLevel(int level) {
	_level = level;
}

void Logger::setPath(const std::string &path) {
	_path = path;
}

void Logger::setUseFile(bool useFile) {
	this->useFile = useFile;
}

void Logger::setUseStdout(bool useStdout) {
	this->useStdout = useStdout;
}


void Logger::log(const std::string &message, int level) {
		print(message, level);
}

void Logger::debug(const std::string &message) {
		print(message, DEBUG);
}

void Logger::info(const std::string &message) {
		print(message, INFO);
}

void Logger::warn(const std::string &message) {
		print(message, WARN);
}

void Logger::warning(std::string message) {
	warn(message);
}

void Logger::error(const std::string &message) {
		print(message, ERROR);
}

void Logger::critical(const std::string &message) {
		print(message, CRITICAL);
}

unsigned int Logger::getLevel() {
	return _level;
}

static std::map<int, std::string> initLevelNames() {
    std::map<int, std::string> levelnames;
    levelnames[Logger::DEBUG] = "DEBUG";
    levelnames[Logger::INFO] = "INFO";
    levelnames[Logger::WARN] = "WARN";
    levelnames[Logger::ERROR] = "ERROR";
    levelnames[Logger::CRITICAL] = "CRITICAL";
    return levelnames;
}

static std::map<int, std::string> initLevelNamesColor() {
    std::map<int, std::string> levelnames = initLevelNames();
    std::map<int, std::string> levelnamescolor;
    levelnamescolor[Logger::DEBUG] = FG_GRAY + levelnames[Logger::DEBUG] + RESET;
    levelnamescolor[Logger::INFO] = FG_WHITE + levelnames[Logger::INFO] + RESET;
    levelnamescolor[Logger::WARN] = FG_YELLOW + levelnames[Logger::WARN] + RESET;
    levelnamescolor[Logger::ERROR] = FG_RED + levelnames[Logger::ERROR] + RESET;
    levelnamescolor[Logger::CRITICAL] = BG_RED + levelnames[Logger::CRITICAL] + RESET;
    return levelnamescolor;
}

static std::map<std::string, std::string> initTextFormat() {
    std::map<std::string, std::string> text_format;
    text_format["$red$"] = FG_RED;
    text_format["$green$"] = FG_GREEN;
    text_format["$yellow$"] = FG_YELLOW;
    text_format["$blue$"] = FG_BLUE;
    text_format["$magenta$"] = FG_MAGENTA;
    text_format["$cyan$"] = FG_CYAN;
    text_format["$white$"] = FG_WHITE;
    text_format["$reset$"] = RESET;
    text_format["$bold$"] = BOLD;
    text_format["$blink$"] = BLINK;
    text_format["$reverse$"] = REVERSE;
    text_format["$underline$"] = UNDERLINED;
    text_format["$black$"] = FG_BLACK;
    text_format["$gray$"] = FG_GRAY;
    text_format["$lightred$"] = FG_LIGHT_RED;
    text_format["$lightgreen$"] = FG_LIGHT_GREEN;
    text_format["$lightyellow$"] = FG_LIGHT_YELLOW;
    text_format["$lightblue$"] = FG_LIGHT_BLUE;
    text_format["$lightmagenta$"] = FG_LIGHT_MAGENTA;
    text_format["$lightcyan$"] = FG_LIGHT_CYAN;
    text_format["$lightwhite$"] = FG_LIGHT_WHITE;
    text_format["$reset$"] = RESET;
    text_format["$default$"] = FG_DEFAULT;
    return text_format;
}

std::map<int, std::string> Logger::_levelnames = initLevelNames();
std::map<int, std::string> Logger::_levelnames_color = initLevelNamesColor();
std::map<std::string, std::string> Logger::_text_format = initTextFormat();

Logger logger("Global");

