#pragma once

#include "log.h"

namespace k3::logging {

    class LogManger {
        
        public:

            LogManger() = default;
            
            ~LogManger() {shutdown();}

            void init();

            void shutdown();

        private:

            bool m_initFlag = false;
    };

}