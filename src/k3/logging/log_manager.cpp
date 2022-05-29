#include "k3/logging/log_manager.hpp"

#include <memory>

namespace k3::logging {

    void LogManger::init() {
        auto inConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        inConsoleSink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e %s:%#] >%!%v%$");
        std::vector<spdlog::sink_ptr> inSinks {inConsoleSink};
        auto inLogger = std::make_shared<spdlog::logger>(KE_IN_LOGGER_NAME, inSinks.begin(), inSinks.end());
        inLogger->set_level(spdlog::level::trace);
        inLogger->flush_on(spdlog::level::trace);
        spdlog::register_logger(inLogger);

        auto outConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        outConsoleSink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e %s:%#] <%!%v%$");
        std::vector<spdlog::sink_ptr> outSinks {outConsoleSink};
        auto outLogger = std::make_shared<spdlog::logger>(KE_OUT_LOGGER_NAME, outSinks.begin(), outSinks.end());
        outLogger->set_level(spdlog::level::trace);
        outLogger->flush_on(spdlog::level::trace);
        spdlog::register_logger(outLogger);

        auto detailedConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        detailedConsoleSink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e %s:%#] %v%$");
        std::vector<spdlog::sink_ptr> detailSinks {detailedConsoleSink};
        auto detailLogger = std::make_shared<spdlog::logger>(KE_DETAILED_LOGGER_NAME, detailSinks.begin(), detailSinks.end());
        detailLogger->set_level(spdlog::level::trace);
        detailLogger->flush_on(spdlog::level::trace);
        spdlog::register_logger(detailLogger);
 
        auto defaultConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        defaultConsoleSink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] %v%$");
        std::vector<spdlog::sink_ptr> defaultSinks {defaultConsoleSink};
        auto defaultLogger = std::make_shared<spdlog::logger>(KE_DEFAULT_LOGGER_NAME, defaultSinks.begin(), defaultSinks.end());
        defaultLogger->set_level(spdlog::level::info);
        defaultLogger->flush_on(spdlog::level::info);
        spdlog::register_logger(defaultLogger);

        m_initFlag = true;
    }

    void LogManger::shutdown() {
        if(m_initFlag) {
            m_initFlag = false;
            spdlog::shutdown();
        }
    }

}
