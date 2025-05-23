#ifndef PLAYER_HPP
#define PLAYER_HPP


class Player {
public:
    int health = 0;
    int gold = 0;
    int ammo = 0;
    int lives = 3;
    float radius = 0.2f; // for collision detection

    // Constructor with default values
    Player(int hp = 100, int g = 0, int a = 0, int l = 3)
        : health(hp), gold(g), ammo(a), lives(l) {}
};

#endif // PLAYER_HPP