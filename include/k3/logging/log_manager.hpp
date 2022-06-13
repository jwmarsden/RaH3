#pragma once

#include "log.h"

#include <chrono>

namespace k3::logging {

    struct LogReference {

    };

    class LogManger {
        
        public:
            LogManger(LogManger const&)       = delete;
            void operator=(LogManger const&)  = delete;
   
            ~LogManger();

            static LogManger& getInstance() {
                static LogManger instance;
                return instance;
            }

            void initialise();

            bool check(spdlog::source_loc loc);

        private:

            LogManger();
    };

}