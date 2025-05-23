#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <vector>
#include <string>
#include "Model.hpp"
#include "Player.hpp"

class StatusBar : public Model {
public:
    // Textury pro tváře a číslice v podobě obrázků
    std::vector<cv::Mat> face_textures;
    std::vector<cv::Mat> digit_textures;
    cv::Mat statusBarImgBackground;
    
    glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    float bar_height = 0.15f;

    int current_face = 0;
    int gold = 0;
    int ammo = 0;
    int health = 100;

    StatusBar() = default;
    StatusBar(const Model& base) : Model(base) {}

    // Načti textury tváří a číslic
    void loadAssets();

    // Nastav aktuální hodnoty
    void setGold(int value) { gold = value; }
    void setAmmo(int value) { ammo = value; }
    void setHealth(int value) { health = value; }

    
    int face_from_health(int health) {
        int face_index = 0;
        int number_of_faces = face_textures.size();
        if (health > 0) {
            face_index = int((number_of_faces - 1) * health / 100);
        }
        return face_index;
    }

    bool if_change(int gold, int ammo, int health) {
        return (this->gold != gold || this->ammo != ammo || this->health != health);
    }

    // Update podle hráče
    void update(const Player& player) {
        int new_face = face_from_health(player.health);
        if (gold != player.gold || ammo != player.ammo || health != player.health) {
            gold = player.gold;
            ammo = player.ammo;
            health = player.health;
            current_face = new_face;
            updateStatusBarTexture();
        }
    }

    cv::Mat int_to_img(int number, int positions);
    void updateStatusBarTexture();

    //
    void draw(glm::vec3 const & offset, glm::vec3 const & rotation, glm::vec3 const & scale_change) override;

    virtual ~StatusBar() = default;
};

#endif // STATUSBAR_HPP