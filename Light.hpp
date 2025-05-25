// Light.hpp
#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

struct Light {
    bool isActive = false;
    glm::vec3 position{0.0f};
    glm::vec3 ambient{0.1f, 0.1f, 0.1f};
    glm::vec3 diffuse{0.0f, 0.0f, 0.0f};
    glm::vec3 specular{0.0f, 0.0f, 0.0f};

    Light() = default;
    Light(glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        : position(pos), ambient(ambient), diffuse(diffuse), specular(specular) {
        isActive = true;
    }
};

#endif // LIGHT_HPP