#include "k3/logging/log.hpp"

#include <memory>

namespace std {
    template <>
    struct hash<k3::logging::K3LogReference> {
        size_t operator()(k3::logging::K3LogReference const &logReference) const {
            size_t seed = 0;
            k3::hashCombine(seed, logReference.file, logReference.function, logReference.line);
            return seed;
        }
    };
}

namespace k3::logging {

    std::unordered_map<K3LogReference, K3LogStore> logReferences;

    LogManger::LogManger() {
        KE_IN(KE_NOARG);
        
        KE_OUT(KE_NOARG);
    }

    LogManger::~LogManger() {

    }

    void LogManger::initialise() {
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
    }

    void LogManger::shutdown() {
        spdlog::shutdown();
    }

    bool LogManger::check(std::string file, std::string function, int line) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        
        K3LogReference logReference {};
        logReference.file = file;
        logReference.function = function;
        logReference.line = line;

        if(logReferences.count(logReference) == 0) {
            K3LogStore store {0, now};
            logReferences[logReference] = store;
            return true;
        }  else {
            K3LogStore store = logReferences[logReference];
            std::chrono::steady_clock::time_point basis = store.basis;
            uint64_t delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - basis).count();

            if(delta >= LOG_SPAM_INTERVAL_MS) {
                store.basis = now;
                logReferences[logReference] = store;
                //KE_TRACE("{}.{}[{}]@{}", logReference.file, logReference.function, logReference.line, delta);
                return true;
            } else {
                store.counter++;
                logReferences[logReference] = store;
                return false;
            }
        }
    }

    int LogManger::rebase(std::string file, std::string function, int line) {
        K3LogReference logReference {};
        logReference.file = file;
        logReference.function = function;
        logReference.line = line;
        if(logReferences.count(logReference) != 0) {
            K3LogStore store = logReferences[logReference];
            int counter = store.counter;
            store.counter = 0;
            logReferences[logReference] = store;
            return counter;
        } else {
            return -1;
        }
    }
}
