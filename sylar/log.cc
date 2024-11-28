#include "log.h"
#include <map>
#include <functional>

namespace sylar {

const char* LogLevel::ToString(LogLevel::Level level) {
    switch (level)
    {
#define XX(name) \
    case LogLevel::name : \
        return #name; \
        break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

class MessageFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

class LevelFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << LogLevel::ToString(event->getLevel());
    }
};

class ElapseFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << logger->getName();
    }
};

class ThreadIdFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << event->getTheadId();
    }
};

class FiberIdFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class DateTimeFormat : public LogFormatter::FormatItem {
public:
    DateTimeFormat(const std::string& format = "%Y:%m:%d %H:%M:%S")
        : m_format(format) {
    }

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << event->getTime();
    }
private:
    std::string m_format;
};

class FileNameFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormat : public LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormat : public LogFormatter::FormatItem {
public:
    StringFormat(const std::string& str) 
        : FormatItem(str), m_string(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

Logger::Logger(const std::string& name) 
        : m_name(name) {

}

void Logger::addAppender(LogAppender::ptr appender) {
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        for (auto& i : m_appenders) {
            i->log(level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event) {
    debug(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event) {
    debug(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
    debug(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
    debug(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
    debug(LogLevel::FATAL, event);
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        std::cout << m_formatter->format(logger, event);
    }
}

FileLogAppender::FileLogAppender(const std::string& filename) 
    : m_filename(filename) {

}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        m_filestream << m_formatter->format(logger, event);
    }
}

bool FileLogAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

LogFormatter::LogFormatter(const std::string& pattern) 
    : m_pattern(pattern) {

}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogEvent::ptr event) {
    std::stringstream ss;
    for(auto& i : m_items) {
        i->format(ss, logger, event);
    }
    return ss.str();
}

void LogFormatter::init() {
    // todo

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt)); } }

        XX(m, MessageFormat),
        XX(p, LevelFormat),
        XX(r, ElapseFormat),
        XX(c, NameFormat),
        XX(t, ThreadIdFormat),
        XX(n, NewLineFormat),
        XX(d, DateTimeFormat),
        XX(f, FileNameFormat),
        XX(l, LineFormat)
#undef XX
    };

    // todo
    
}

}