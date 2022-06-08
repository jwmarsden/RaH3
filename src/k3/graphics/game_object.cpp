#include "k3/graphics/game_object.hpp"

namespace k3::graphics {

    KeGameObject::KeGameObject(id_t objId, std::string name) : m_id {objId}, m_name {name} {
        KE_IN("(): m_id:{} name:\"{}\"", m_id, m_name);

        KE_OUT("(): m_id:{} name:\"{}\"", m_id, m_name);
    }

    KeGameObject::KeGameObject(id_t objId) : m_id {objId}, m_name {"Unknown"} {
        KE_IN("(): m_id:{} name:\"{}\"", m_id, m_name);

        KE_OUT("(): m_id:{} name:\"{}\"", m_id, m_name);
    }

    KeGameObject::~KeGameObject() {
        KE_IN("(): m_id:{} name:\"{}\"", m_id, m_name);
        if(model != nullptr) {
            model = nullptr;
        }
        KE_OUT("(): m_id:{} name:\"{}\"", m_id, m_name);
    }
}