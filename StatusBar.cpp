#include <numeric>
#include "StatusBar.hpp"
#include "App.hpp"

void StatusBar::loadAssets() {
    statusBarImgBackground = cv::imread("resources/statusBar/background.png", cv::IMREAD_UNCHANGED);

    // Load face textures
    for (int i = 109; i <= 132; ++i) {
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

void StatusBar::updateStatusBarTexture() {
    int height_px = statusBarImgBackground.rows;
    int width_px = statusBarImgBackground.cols;
    cv::Mat statusBarImg = cv::Mat(height_px, width_px, CV_8UC4);
    statusBarImg = statusBarImgBackground.clone();
    // floor number
    int floor_number = 0;
    cv::Mat floor_number_img = digit_textures[floor_number];

    // score 2 digits (gold)
    int score = this->gold;

    // lives 2 digits

    // draw face
    cv::Mat face = face_textures[current_face];
    // copy face to statusBarImg

    // health 2 digits
    int health = this->health;

    int health_tens = health / 10;
    int health_units = health % 10;
    cv::Mat health_tens_img = digit_textures[health_tens];
    cv::Mat health_units_img = digit_textures[health_units];

    // ammo 2 digits
    int ammo = this->ammo;
    int ammo_tens = ammo / 10;
    int ammo_units = ammo % 10;
    cv::Mat ammo_tens_img = digit_textures[ammo_tens];
    cv::Mat ammo_units_img = digit_textures[ammo_units];

    // rotate statusBarImg
    cv::Mat flipedImg;
    cv::flip(statusBarImg, flipedImg, 0);
    this->texture_id = App::gen_tex(flipedImg, TextureFilter::TrilinearMipmap);
}

void StatusBar::draw(glm::vec3 const & offset, glm::vec3 const & rotation, glm::vec3 const & scale_change) {
    if (meshes.empty()) {
        std::cerr << "StatusBar: meshes is empty!" << std::endl;
        return;
    }
    ShaderProgram &shader = meshes[0].shader;
    shader.activate();

    shader.setUniform("uP_m", this->ortho);
    shader.setUniform("uV_m", glm::mat4(1.0f));

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