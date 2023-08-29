// Copyright 2023 VisualGMQ
#pragma once

#include <iostream>
#include <string_view>
#include <vector>
#include <fstream>
#include <map>
#include <utility>
#include <string>
#include <memory>

namespace logger {

enum Level {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    FatalError,
    All,
};

class Logger final {
 public:
    Logger(std::ostream& o): stream_(o), level_(All) {}

    template <typename... Args>
    void Trace(std::string_view funcName, std::string_view filename, unsigned int line, Args&&... args) {
        log(Level::Trace, funcName, filename, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Debug(std::string_view funcName, std::string_view filename, unsigned int line, Args&&... args) {
        log(Level::Debug, funcName, filename, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Info(std::string_view funcName, std::string_view filename, unsigned int line, Args&&... args) {
        log(Level::Info, funcName, filename, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Warning(std::string_view funcName, std::string_view filename, unsigned int line, Args&&... args) {
        log(Level::Warning, funcName, filename, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Error(std::string_view funcName, std::string_view filename, unsigned int line, Args&&... args) {
        log(Level::Error, funcName, filename, line, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void FatalError(std::string_view funcName, std::string_view filename, unsigned int line, Args&&... args) {
        log(Level::FatalError, funcName, filename, line, std::forward<Args>(args)...);
    }

    void SetLevel(Level level) { level_ = level; }

 private:
    std::ostream& stream_;
    Level level_;

    template <typename... Args>
    void log(Level level, std::string_view funcName, std::string_view filename, unsigned int line, Args&&... args) {
        if (level <= level_) {
            printf("[%s][%s][%s][%u]", Level2Str(level).data(), filename.data(), funcName.data(), line);
            doLog(std::forward<Args>(args)...);
            stream_ << std::endl;
        }
    }

    template <typename ParamT, typename... Params>
    void doLog(ParamT&& param, Params&&... params) {
        stream_ << param;
        doLog(std::forward<Params>(params)...);
    }

    template <typename ParamT>
    void doLog(ParamT&& param) {
        stream_ << param;
    }

    std::string_view Level2Str(Level level) {
        #define CASE(level) case Level::level: return #level;
        switch (level) {
            CASE(Trace)
            CASE(Debug)
            CASE(Info)
            CASE(Warning)
            CASE(Error)
            CASE(FatalError)
            default:
                return "";
        }
        #undef CASE
    }
};

class LoggerMgr final {
 public:
    static LoggerMgr& Instance() {
        static std::unique_ptr<LoggerMgr> instance;
        if (!instance) {
            instance.reset(new LoggerMgr());
        }
        return *instance;
    }

    static Logger& CreateFromFile(const std::string& name, const std::string& filename, bool append = false) {
        auto& instance = Instance();
        instance.files_.emplace_back(filename, append ? std::ios::app : std::ios::trunc);
        return instance.loggers_.emplace(name, Logger(instance.files_.back())).first->second;
    }

    static Logger& CreateFromOstream(const std::string& name, std::ostream& o) {
        auto& instance = Instance();
        return instance.loggers_.emplace(name, o).first->second;
    }

    Logger& GetDefault() { return *defaultLogger_; }

 private:
    std::vector<std::ofstream> files_;
    std::map<std::string, Logger> loggers_;
    std::unique_ptr<Logger> defaultLogger_;

    LoggerMgr() {
        defaultLogger_.reset(new Logger(std::cout));
    }
};

#define LOGT(tag, ...) logger::LoggerMgr::Instance().GetDefault().Trace(__FUNCTION__, __FILE__, __LINE__, "[", tag, "]:", ## __VA_ARGS__)
#define LOGD(tag, ...) logger::LoggerMgr::Instance().GetDefault().Debug(__FUNCTION__, __FILE__, __LINE__, "[", tag, "]:", ## __VA_ARGS__)
#define LOGI(tag, ...) logger::LoggerMgr::Instance().GetDefault().Info(__FUNCTION__, __FILE__, __LINE__, "[", tag, "]:", ## __VA_ARGS__)
#define LOGW(tag, ...) logger::LoggerMgr::Instance().GetDefault().Warning(__FUNCTION__, __FILE__, __LINE__,"[", tag, "]:", ## __VA_ARGS__)
#define LOGE(tag, ...) logger::LoggerMgr::Instance().GetDefault().Error(__FUNCTION__, __FILE__, __LINE__,"[", tag, "]:", ## __VA_ARGS__)
#define LOGF(tag, ...) logger::LoggerMgr::Instance().GetDefault().FatalError(__FUNCTION__, __FILE__, __LINE__,"[", tag, "]:", ## __VA_ARGS__)

}  // namespace logger
