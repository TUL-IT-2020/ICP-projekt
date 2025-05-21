#ifndef BULLET_HPP
#define BULLET_HPP

#include <glm/glm.hpp>

class Bullet {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float speed = 20.0f;
    bool active = true;
    int damage = 10;

    Bullet(const glm::vec3& pos, const glm::vec3& dir)
        : position(pos), direction(glm::normalize(dir)) {}
};

#endif // PLAYER_HPP