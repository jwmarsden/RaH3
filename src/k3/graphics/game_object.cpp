#include "k3/graphics/game_object.hpp"

namespace k3::graphics {

    KeGameObject::KeGameObject() {
        KE_IN("(): m_id:{}", getId());

        KE_OUT("(): m_id:{}", getId());
    }

    KeGameObject::~KeGameObject() {
        KE_IN("(): m_id:{}", getId());
        if(model != nullptr) {
            model = nullptr;
        }
        KE_OUT("(): m_id:{}", getId());
    }
}