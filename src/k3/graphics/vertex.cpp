#include "k3/graphics/vertex.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <cassert>
#include <cstring>


namespace std {
    template <>
    struct hash<k3::graphics::K3Vertex> {
        size_t operator()(k3::graphics::K3Vertex const &vertex) const {
            size_t seed = 0;
            k3::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

namespace k3::graphics {

    std::vector<VkVertexInputBindingDescription> K3Vertex::getBindingDescriptions() {
        KE_IN(KE_NOARG);
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(K3Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        KE_OUT(KE_NOARG);
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> K3Vertex::getAttributeDescriptions() {
        KE_IN(KE_NOARG);
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(K3Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(K3Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(K3Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(K3Vertex, uv)});
        KE_OUT(KE_NOARG);
        return attributeDescriptions;
    }

    void K3Builder::loadModel(const std::string &filePath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
            KE_CRITICAL("{} {}", warn, err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<K3Vertex, uint32_t> uniqueVertices {};

        for(const auto &shape : shapes) {
            for(const auto &index : shape.mesh.indices) {
                K3Vertex vertex{};

                if(index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    auto colorIndex = 3 * index.vertex_index + 2;
                    if(colorIndex < attrib.colors.size()) {
                        vertex.color = {
                            attrib.colors[colorIndex - 2],
                            attrib.colors[colorIndex - 1],
                            attrib.colors[colorIndex - 0],
                        };
                    } else {
                        vertex.color = {1.f, 1.f, 1.f};
                    }
                }

                if(index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if(index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                if(uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                } 

                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}