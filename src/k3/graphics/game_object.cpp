#include "k3/graphics/game_object.hpp"

namespace k3::graphics {

    void KeGameObject::init() {
        KE_IN("(): m_id:{}", getId());
        assert(!m_initFlag && "Already had init.");
        m_initFlag = true;
        KE_OUT("(): m_id:{}", getId());
    }

    void KeGameObject::shutdown() {
        KE_IN("(): m_id:{}", getId());
        assert(m_initFlag && "Must have been init to shutdown.");
        if(m_initFlag) {
            m_initFlag = false;
            model = nullptr;
        }
        KE_OUT("(): m_id:{}", getId());
    }
}