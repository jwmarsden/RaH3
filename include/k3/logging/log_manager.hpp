#pragma once

#include "log.h"

#include "utils.hpp"

#include <chrono>
#include <vector>
#include <unordered_map>


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

            bool check(std::string file, std::string function, int line);

        private:

            LogManger();

            
    };

}