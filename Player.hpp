#ifndef PLAYER_HPP
#define PLAYER_HPP


class Player {
public:
    int health = 0;
    int gold = 0;
    int ammo = 0;

    // Constructor with default values
    Player(int hp = 100, int g = 0, int a = 0)
        : health(hp), gold(g), ammo(a) {}
};

#endif // PLAYER_HPP