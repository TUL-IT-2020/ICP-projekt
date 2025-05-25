#ifndef PLAYER_HPP
#define PLAYER_HPP

enum weapon_type {
    KNIFE,
    PISTOL,
    CHAINGUN,
    MACHINEGUN,
};

class Player {
public:
    int health = 0;
    int gold = 0;
    int ammo = 0;
    int lives = 3;
    float radius = 0.2f; // for collision detection
    weapon_type current_weapon = KNIFE;

    // Constructor with default values
    Player(int hp = 100, int g = 0, int a = 0, int l = 3)
        : health(hp), gold(g), ammo(a), lives(l) {
            if (ammo < 0) {
                ammo = 0; // Ensure ammo is not negative
            } else if (ammo > 0) {
                current_weapon = PISTOL; // If ammo is positive, switch to pistol
            }
        }
    
    void addAmmo(int amount) {
        ammo += amount;
        if (ammo > 0 && current_weapon == KNIFE) {
            current_weapon = PISTOL; // Switch to pistol if ammo is added
        }
    }

    bool reduceAmmo(int amount) {
        if (ammo >= amount) {
            ammo -= amount;
            return true; // Ammo successfully reduced
        }
        if (ammo == 0) {
            this->current_weapon = KNIFE; // Switch to knive if no ammo left
        }
        return false; // Not enough ammo
    }

    bool reduceAmmo() {
        return reduceAmmo(1); // Default to reducing by 1
    }
};

#endif // PLAYER_HPP