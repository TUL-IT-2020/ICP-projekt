#include <numeric>
#include <filesystem>
#include <iostream>
#include "StatusBar.hpp"
#include "App.hpp"
#include "Player.hpp"

void StatusBar::loadAssets() {
    statusBarImgBackground = cv::imread("resources/statusBar/background.png", cv::IMREAD_UNCHANGED);

    // load weapon textures
    std::vector<std::string> weapon_names = {"knife", "pistol", "chainGun", "machineGun"};
    for (const auto& weapon_name : weapon_names) {
        std::string path = "resources/statusBar/weapons/" + weapon_name + ".png";
        cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
        if (img.empty()) {
            std::cerr << "Failed to load weapon texture: " << path << std::endl;
            continue;
        }
        weapon_textures_map[str_to_enum(weapon_name)] = img;
    }

    // TEST for weapon on player
    std::unordered_map< weapon_type, std::vector<cv::Mat> > player_weapon_textures_map;
    std::string base_path = "resources/sprites/weapons/";
    for (const auto& weapon_name : weapon_names) {
        std::string weapon_path = base_path + weapon_name + "/";
        for (const auto& entry : std::filesystem::directory_iterator(weapon_path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_UNCHANGED);
                if (img.empty()) {
                    std::cerr << "Failed to load weapon texture: " << entry.path() << std::endl;
                    continue;
                }
                player_weapon_textures_map[str_to_enum(weapon_name)].push_back(img);
            }
        }
        std::cout << "Loaded " << player_weapon_textures_map[str_to_enum(weapon_name)].size() << " textures for weapon: " << weapon_name << std::endl;
    }  

    // Load face textures
    for (int i = 130; i > 108; --i) {
        std::string path = "resources/statusBar/PIC00" + std::to_string(i) + ".png";
        cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
        if (img.empty()) {
            std::cerr << "Failed to load face texture: " << path << std::endl;
            continue;
        }
        face_textures.push_back(img);
    }

    // Load digit textures
    for (int i = 0; i < 10; ++i) {
        std::string path = "resources/statusBar/font/" + std::to_string(i) + ".png";
        cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
        if (img.empty()) {
            std::cerr << "Failed to load digit texture: " << path << std::endl;
            continue;
        }
        digit_textures.push_back(img);
    }
    std::string path = "resources/statusBar/font/_.png";
    cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
    digit_textures.push_back(img);

    this->texture_id = App::gen_tex(statusBarImgBackground, TextureFilter::TrilinearMipmap);
}

cv::Mat StatusBar::int_to_img(int number, int positions) {
    int number_width = digit_textures[0].cols;
    int number_height = digit_textures[0].rows;
    int type = digit_textures[0].type();

    cv::Mat img = cv::Mat::zeros(number_height, positions * number_width, type);
    for (int i = 0; i < positions; ++i) {
        int digit = (number / static_cast<int>(std::pow(10, positions - i - 1))) % 10;
        if (digit < 0 || digit >= static_cast<int>(digit_textures.size())) {
            digit = 0; // fallback to 0 if out of range
        }
        cv::Mat digit_img = digit_textures[digit];
        digit_img.copyTo(img(cv::Rect(i * number_width, 0, digit_img.cols, digit_img.rows)), digit_img.channels() == 4 ? digit_img : cv::noArray());
    }
    return img;
}

void StatusBar::updateStatusBarTexture() {
    int height_px = statusBarImgBackground.rows;
    int width_px = statusBarImgBackground.cols;
    cv::Mat statusBarImg = statusBarImgBackground.clone();

    auto check_and_copy = [&](const cv::Mat& src, cv::Mat& dst, cv::Point pos) {
        if (src.empty()) {
            std::cerr << "Warning: source image is empty!" << std::endl;
            return;
        }
        if (pos.x < 0 || pos.y < 0 ||
            pos.x + src.cols > dst.cols ||
            pos.y + src.rows > dst.rows) {
            std::cerr << "Warning: ROI out of bounds! pos=(" << pos.x << "," << pos.y
                    << "), size=(" << src.cols << "," << src.rows
                    << "), dst size=(" << dst.cols << "," << dst.rows << ")" << std::endl;
            return;
        }
        src.copyTo(dst(cv::Rect(pos, src.size())), src);
    };

    // Example positions for each element (adjust as needed)
    cv::Point floor_pos(10, 13);
    cv::Point gold_pos(45, 13);
    cv::Point lives_pos(100, 13);
    cv::Point face_pos(125, 1);
    cv::Point health_pos(160, 13);
    cv::Point ammo_pos(202, 13);
    cv::Point weapon_pos(241, 1);

    // floor number
    cv::Mat floor_number_img = int_to_img(this->floor_number, 2);
    check_and_copy(floor_number_img, statusBarImg, floor_pos);

    // score
    cv::Mat gold_img = int_to_img(this->gold, 5);
    check_and_copy(gold_img, statusBarImg, gold_pos);

    // lives
    cv::Mat lives_img = int_to_img(this->lives, 2);
    check_and_copy(lives_img, statusBarImg, lives_pos);

    // draw face
    cv::Mat face = face_textures[current_face];
    check_and_copy(face, statusBarImg, face_pos);

    // health 2 digits
    cv::Mat health_img = int_to_img(this->health, 3);
    check_and_copy(health_img, statusBarImg, health_pos);

    // ammo 2 digits
    cv::Mat ammo_img = int_to_img(this->ammo, 3);
    check_and_copy(ammo_img, statusBarImg, ammo_pos);

    // weapon
    cv::Mat weapon_img = weapon_textures_map[this->current_weapon];
    if (weapon_img.empty()) {
        std::cerr << "Warning: weapon texture is empty for weapon type: " << this->current_weapon << std::endl;
    } else {
        check_and_copy(weapon_img, statusBarImg, weapon_pos);
    }

    // rotate statusBarImg
    cv::Mat flipedImg;
    cv::flip(statusBarImg, flipedImg, 0);
    this->texture_id = App::gen_tex(flipedImg, TextureFilter::TrilinearMipmap);
    std::cout << "StatusBar texture updated." << std::endl;
}

void StatusBar::draw(glm::vec3 const & offset, glm::vec3 const & rotation, glm::vec3 const & scale_change) {
    if (meshes.empty()) {
        std::cerr << "StatusBar: meshes is empty!" << std::endl;
        return;
    }
    ShaderProgram &shader = meshes[0].shader;
    shader.activate();

    shader.setUniform("v_m", this->ortho);
    shader.setUniform("p_m", glm::mat4(1.0f));

    float bar_height = this->bar_height;
    glm::vec3 sb_scale = glm::vec3(1.0f, bar_height, 1.0f);
    glm::vec3 sb_offset = glm::vec3(0.0f, -1.0f + bar_height, 0.0f);
    glm::vec3 sb_rotation = glm::vec3(0.0f);

    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), sb_offset) *
                             glm::rotate(glm::mat4(1.0f), sb_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
                             glm::rotate(glm::mat4(1.0f), sb_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
                             glm::rotate(glm::mat4(1.0f), sb_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
                             glm::scale(glm::mat4(1.0f), sb_scale);

    if (texture_id == 0) {
        std::cerr << "Error: Texture ID is 0! Texture not loaded properly." << std::endl;
    }
    if (texture_id != 0) {
        int texture_unit = 0;
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        int texUniformLocation = glGetUniformLocation(shader.getID(), "tex0");
        glUniform1i(texUniformLocation, texture_unit);
    }
    Model::draw(model_matrix);
}

bool ShaderProgram::hasUniform(const std::string& name) {
    // Check if the location is already cached (even if it's -1, meaning not found)
    if (uniform_location_cache.count(name)) {
        return uniform_location_cache[name] != -1;
    }

    // If not in cache, get the location from OpenGL
    GLint location = glGetUniformLocation(ID, name.c_str());

    // Store the result in the cache (this will store -1 if not found)
    uniform_location_cache[name] = location;

    return location != -1;
}