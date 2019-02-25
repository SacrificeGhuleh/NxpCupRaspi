//
// Created by Richard Zvonek on 13-Nov-18.
//

#ifndef NXPCUPRASPI_LOGGER_H
#define NXPCUPRASPI_LOGGER_H

#include "colors.h"

/*
#include <cstdarg>
#include <cstdio>
#include "Singleton.h"*/

#define NXP_PRINT_TRACE 1
#define NXP_PRINT_INFO 1
#define NXP_PRINT_WARN 1
#define NXP_PRINT_ERROR 1

namespace nxpbc {
    /*enum class LoggerLevel {
        trace,
        info,
        warn,
        error
    };

    class Logger : public Singleton {
    protected:
        inline Logger() {};

        virtual ~Logger();

    public:
        template<typename... Args>
        inline void trace(const char *msg, const Args &... args) {
            log(LoggerLevel::trace, msg, args...);
        }

        template<typename... Args>
        inline void info(const char *msg, const Args &... args) {
            log(LoggerLevel::info, msg, args...);
        }

        template<typename... Args>
        inline void warn(const char *msg, const Args &... args) {
            log(LoggerLevel::warn, msg, args...);
        }

        template<typename... Args>
        inline void error(const char *msg, const Args &... args) {
            log(LoggerLevel::error, msg, args...);
        }

        template<typename... Args>
        inline void log(LoggerLevel lvl, const char *msg, const Args &... args) {
            switch (lvl) {
                case LoggerLevel::trace:
                    printf(KWHT);
                    break;
                case LoggerLevel::info:
                    printf(KGRN);
                    break;
                case LoggerLevel::warn:
                    printf(KYEL);
                    break;
                case LoggerLevel::error:
                    printf(KRED);
                    break;
            };
            printf(msg);
            printf(RST);
        }
    };*/

    /*const std::string currentDateTime() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

        return buf;
    }
*/
#if NXP_PRINT_TRACE
#define NXP_TRACEP(msg, ...)\
    printf(KWHT);\
    printf("[NXP TRACE]\t|");\
    printf(msg, __VA_ARGS__);\
    printf(RST)
#define NXP_TRACE(msg)\
    printf(KWHT);\
    printf("[NXP TRACE]\t|");\
    printf(msg);\
    printf(RST)
#else
#define NXP_TRACEP(msg, ...)
#define NXP_TRACE(msg)
#endif

#if NXP_PRINT_INFO
#define NXP_INFOP(msg, ...)\
    printf("[NXP INFO]\t|");\
    printf(KGRN);\
    printf(msg, __VA_ARGS__);\
    printf(RST)
#define NXP_INFO(msg) \
    printf("[NXP INFO]\t|");\
    printf(KGRN);\
    printf(msg);\
    printf(RST)
#else
#define NXP_INFOP(msg, ...)
#define NXP_INFO(msg)
#endif

#if NXP_PRINT_WARN
#define NXP_WARNP(msg, ...) \
    printf("[NXP WARN]\t|");\
    printf(KYEL);\
    printf(msg, __VA_ARGS__);\
    printf(RST)
#define NXP_WARN(msg) \
    printf(KYEL);\
    printf("[NXP WARN]\t|");\
    printf(msg);\
    printf(RST)
#else
#define NXP_WARNP(msg, ...)
#define NXP_WARN(msg)
#endif

#if NXP_PRINT_ERROR
#define NXP_ERRORP(msg, ...) \
    printf(KRED);\
    printf("[NXP ERROR]\t|");\
    printf(msg, __VA_ARGS__);\
    printf(RST)
#define NXP_ERROR(msg) \
    printf("[NXP ERROR]\t|");\
    printf(KRED);\
    printf(msg);\
    printf(RST)
#else
#define NXP_ERRORP(msg, ...)
#define NXP_ERROR(msg)
#endif

}

#endif //NXPCUPRASPI_LOGGER_H
