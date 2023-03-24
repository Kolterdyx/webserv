//
// Created by Ciro Garcia belmonte on 3/23/23.
//

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
	_patterns["%level%"] = std::to_string(_level);
	_patterns["%name%"] = _name;

	_text_format["$red$"] = FG_RED;
	_text_format["$green$"] = FG_GREEN;
	_text_format["$yellow$"] = FG_YELLOW;
	_text_format["$blue$"] = FG_BLUE;
	_text_format["$magenta$"] = FG_MAGENTA;
	_text_format["$cyan$"] = FG_CYAN;
	_text_format["$white$"] = FG_WHITE;
	_text_format["$reset$"] = RESET;
	_text_format["$bold$"] = BOLD;
	_text_format["$blink$"] = BLINK;
	_text_format["$reverse$"] = REVERSE;
	_text_format["$underline$"] = UNDERLINED;
	_text_format["$black$"] = FG_BLACK;
	_text_format["$gray$"] = FG_GRAY;
	_text_format["$lightred$"] = FG_LIGHT_RED;
	_text_format["$lightgreen$"] = FG_LIGHT_GREEN;
	_text_format["$lightyellow$"] = FG_LIGHT_YELLOW;
	_text_format["$lightblue$"] = FG_LIGHT_BLUE;
	_text_format["$lightmagenta$"] = FG_LIGHT_MAGENTA;
	_text_format["$lightcyan$"] = FG_LIGHT_CYAN;
	_text_format["$lightwhite$"] = FG_LIGHT_WHITE;
	_text_format["$reset$"] = RESET;
	_text_format["$default$"] = FG_DEFAULT;

	_levelnames[DEBUG] = "DEBUG";
	_levelnames[INFO] = "INFO";
	_levelnames[WARN] = "WARN";
	_levelnames[ERROR] = "ERROR";
	_levelnames[CRITICAL] = "CRITICAL";

	_levelnames_color[DEBUG] = FG_BLUE + _levelnames[DEBUG] + RESET;
	_levelnames_color[INFO] = FG_GREEN + _levelnames[INFO] + RESET;
	_levelnames_color[WARN] = FG_LIGHT_RED + _levelnames[WARN] + RESET;
	_levelnames_color[ERROR] = FG_RED + _levelnames[ERROR] + RESET;
	_levelnames_color[CRITICAL] = FG_RED + _levelnames[CRITICAL] + RESET;
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

	if (copy._file.is_open())
		_file.open(_path, std::ios::out | std::ios::app);
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
	if (useFile)
		_file << applyFormat(message, false, level) << std::endl;
}

void Logger::setFormat(const std::string &format) {
	_format = format;
}

std::string Logger::applyFormat(const std::string &message, bool color, int level) {

	if (level < INFO) {
		_patterns["%message%"] = FG_GRAY + message;
	}
	else if (level < WARN) {
		_patterns["%message%"] = message;
	}
	else if (level < ERROR) {
		_patterns["%message%"] = FG_YELLOW + message;
	}
	else if (level < CRITICAL) {
		_patterns["%message%"] = FG_RED + message;
	}
	else {
		_patterns["%message%"] = BG_RED + message;
	}

	std::time_t time = std::time(nullptr);
	std::tm *tm = std::localtime(&time);
	std::ostringstream oss1;
	std::ostringstream oss2;
	oss1 << std::put_time(tm, "%Y-%m-%d");
	oss2 << std::put_time(tm, "%H:%M:%S");
	_patterns["%date%"] = oss1.str();
	_patterns["%time%"] = oss2.str();

	for (std::map<int, std::string>::iterator it = _levelnames_color.begin(); it != _levelnames_color.end(); ++it) {
		if (level < it->first) {
			break;
		}
		_patterns["%levelname%"] = it->second;
	}

	std::string formatted = _format;
	std::string::size_type pos = 0;
	for (std::map<std::string, std::string>::iterator it = _text_format.begin(); it != _text_format.end(); ++it) {
		pos = formatted.find(it->first);
		while (pos != std::string::npos) {
			formatted.replace(pos, it->first.length(), it->second);
			pos = formatted.find(it->first);
		}
	}
	for (std::map<std::string, std::string>::iterator it = _patterns.begin(); it != _patterns.end(); ++it) {
		pos = formatted.find(it->first);
		while (pos != std::string::npos) {
			if (color)
				formatted.replace(pos, it->first.length(), it->second);
			else
				formatted.replace(pos, it->first.length(), "");
			pos = formatted.find(it->first);
		}
	}
	return formatted + RESET;
}

void Logger::setLevel(int level) {
	_level = level;
}

void Logger::setPath(const std::string &path) {
	_path = path;
	_file.open(_path, std::ios::out | std::ios::app);
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
