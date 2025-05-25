#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <vector>
#include <string>
#include "Model.hpp"
#include "Player.hpp"

class StatusBar : public Model {
public:
    // Textures for the status bar
    std::vector<cv::Mat> face_textures;
    std::vector<cv::Mat> digit_textures;
    std::unordered_map<weapon_type, cv::Mat> weapon_textures_map;
    cv::Mat statusBarImgBackground;
    
    glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    float bar_height = 0.15f;

    int current_face = 0;
    int gold = 0;
    int ammo = 0;
    int health = 100;
    int lives = 0;
    int floor_number = 1;
    weapon_type current_weapon = KNIFE;

    StatusBar() = default;
    StatusBar(const Model& base) : Model(base) {}

    /* Load assets for the status bar
     * This function loads the background image, weapon textures, face textures, and digit textures.
     * It should be called once during initialization.
     */
    void loadAssets();

    void setGold(int value) { gold = value; updateStatusBarTexture();}
    void setAmmo(int value) { ammo = value; updateStatusBarTexture();}
    void setHealth(int value) { 
        health = value;
        current_face = face_from_health(value);
        updateStatusBarTexture();
    }
    
    void setLevel(int value) { 
        floor_number = value;
        updateStatusBarTexture();
    }

    /* Convert health to face index
     * @param health: Health value (0-100)
     * @return: Index of the face texture corresponding to the health value
     */
    int face_from_health(int health) {
        int face_index = 0;
        int number_of_faces = face_textures.size();
        if (health > 0) {
            face_index = int((number_of_faces - 1) * health / 100);
        }
        return face_index;
    }

    /* Check if the player has changed
     * @param player: Player object to compare with
     * @return: true if any of the player's attributes have changed, false otherwise
     */
    bool if_change(const Player& player) {
        return (this->gold != player.gold || this->ammo != player.ammo || this->health != player.health || this->lives != player.lives || this->current_weapon != player.current_weapon);
    }

    /* cache update
     * If the player has changed, update the status bar values and texture.
     */ 
    void update(const Player& player) {
        if (if_change(player)) {
            gold = player.gold;
            ammo = player.ammo;
            health = player.health;
            lives = player.lives;
            current_weapon = player.current_weapon;
            current_face = face_from_health(player.health);
            updateStatusBarTexture();
        }
    }

    /* Convert an integer to an image representation
     * @param number: The integer to convert
     * @param positions: Number of positions (digits) in the image
     * @return: cv::Mat representing the integer as an image
     */
    cv::Mat int_to_img(int number, int positions);

    /* Update the status bar texture
     * This function updates the status bar texture based on the current values of gold, ammo, health, lives, and face.
     * It is automatically called in the update function if the player has changed.
     */
    void updateStatusBarTexture();

    /* Draw the status bar
     * @param offset: Offset for the position of the status bar
     * @param rotation: Rotation of the status bar
     * @param scale_change: Scale change for the status bar
     */
    void draw(glm::vec3 const & offset, glm::vec3 const & rotation, glm::vec3 const & scale_change) override;

    virtual ~StatusBar() = default;
};

#endif // STATUSBAR_HPP