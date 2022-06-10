#pragma once

#include "log.h"

#include "entt.hpp"

namespace k3::scene {

    class K3Scene {

        public:

            K3Scene();

            ~K3Scene();

        private:

            entt::registry m_registry {};
    };

}