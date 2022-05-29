#pragma once

#ifdef KE_CONFIG_RELEASE
#   if !defined(SPDLOG_ACTIVE_LEVEL)
#       define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#   endif
#else
#   if !defined(SPDLOG_ACTIVE_LEVEL)
#       define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#   endif
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ranges.h"

#define KE_IN_LOGGER_NAME "ke_in_log"
#define KE_OUT_LOGGER_NAME "ke_out_log"
#define KE_DETAILED_LOGGER_NAME "ke_detail_log"
#define KE_DEFAULT_LOGGER_NAME "ke_default_log"

#define KE_NOARG "()"

#ifndef KE_CONFIG_RELEASE
#define KE_IN(...)          if (spdlog::get(KE_DETAILED_LOGGER_NAME) != nullptr) {(spdlog::get(KE_IN_LOGGER_NAME))->log(spdlog::source_loc{__FILE__, __LINE__, static_cast<const char *>(__FUNCTION__)}, spdlog::level::trace, __VA_ARGS__);}
#define KE_OUT(...)         if (spdlog::get(KE_DETAILED_LOGGER_NAME) != nullptr) {(spdlog::get(KE_OUT_LOGGER_NAME))->log(spdlog::source_loc{__FILE__, __LINE__, static_cast<const char *>(__FUNCTION__)}, spdlog::level::trace, __VA_ARGS__);}
#define KE_TRACE(...)       if (spdlog::get(KE_DETAILED_LOGGER_NAME) != nullptr) {SPDLOG_LOGGER_TRACE(spdlog::get(KE_DETAILED_LOGGER_NAME), __VA_ARGS__);}
#define KE_DEBUG(...)       if (spdlog::get(KE_DETAILED_LOGGER_NAME) != nullptr) {SPDLOG_LOGGER_DEBUG(spdlog::get(KE_DETAILED_LOGGER_NAME), __VA_ARGS__);}
#define KE_INFO(...)        if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__);}
#define KE_WARN(...)        if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__);}
#define KE_ERROR(...)       if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__);}
#define KE_CRITICAL(...)    if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__);}
#else
#define KE_IN(...)          (void) 0
#define KE_OUT(...)         (void) 0
#define KE_TRACE(...)       (void) 0
#define KE_DEBUG(...)       (void) 0
#define KE_INFO(...)        if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__);}
#define KE_WARN(...)        if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__);}
#define KE_ERROR(...)       if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__);}
#define KE_CRITICAL(...)    if (spdlog::get(KE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(KE_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__);}
#endif