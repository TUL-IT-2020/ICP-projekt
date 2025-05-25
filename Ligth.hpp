// Light.hpp
#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

struct Light {
    bool active = false;         // Příznak, zda je světlo aktivní
    glm::vec3 position{0.0f};
    glm::vec3 ambient_intensity{0.1f, 0.1f, 0.1f};
    glm::vec3 diffuse_intensity{0.0f, 0.0f, 0.0f};  // Necháme tmavé, dokud není aktivní
    glm::vec3 specular_intensity{0.0f, 0.0f, 0.0f}; // Necháme tmavé, dokud není aktivní
};

#endif // LIGHT_HPP