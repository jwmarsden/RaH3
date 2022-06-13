#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ranges.h"

#include "utils.hpp"

#include <chrono>
#include <vector>
#include <unordered_map>

#define KE_IN_LOGGER_NAME "ke_in_log"
#define KE_OUT_LOGGER_NAME "ke_out_log"
#define KE_DETAILED_LOGGER_NAME "ke_detail_log"
#define KE_DEFAULT_LOGGER_NAME "ke_default_log"

namespace k3::logging {

    struct K3LogReference {
        std::string file;
        std::string function;
        uint32_t line;

        bool operator==(const K3LogReference other) const {
            return file == other.file && function == other.function && line == other.line;
        }
    };

    class LogManger {
        
        public:

            const int LOG_SPAM_INTERVAL_MS = 10000;

            LogManger(LogManger const&)       = delete;
            void operator=(LogManger const&)  = delete;
   
            ~LogManger();

            static LogManger& getInstance() {
                static LogManger instance;
                return instance;
            }

            void initialise();

            bool check(spdlog::source_loc location);

        private:

            LogManger();

            
    };

}