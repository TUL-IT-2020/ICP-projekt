#include "Model.hpp"

glm::vec3 json_to_vec3(const nlohmann::json& json_array) {
    if (!json_array.is_array() || json_array.size() != 3) {
        throw std::invalid_argument("Invalid JSON format for vec3. Expected an array of size 3.");
    }
    return glm::vec3(json_array[0].get<float>(), json_array[1].get<float>(), json_array[2].get<float>());
}

cv::Mat createCheckerboardTexture() {
    // Vytvoření 2x2 matice s 3 kanály (RGB) a 8bitovými hodnotami
    cv::Mat checkerboard(2, 2, CV_8UC3);

    // Nastavení barev pixelů (černá a bílá)
    checkerboard.at<cv::Vec3b>(0, 0) = cv::Vec3b(0, 0, 0);   // Černá
    checkerboard.at<cv::Vec3b>(0, 1) = cv::Vec3b(255, 255, 255); // Bílá
    checkerboard.at<cv::Vec3b>(1, 0) = cv::Vec3b(255, 255, 255); // Bílá
    checkerboard.at<cv::Vec3b>(1, 1) = cv::Vec3b(0, 0, 0);   // Černá

    return checkerboard;
}

Model Model::parse_json_to_model(const nlohmann::json& model_data, Model& model,
                      std::unordered_map<std::string, Model> model_cache) {
    
    std::string name = model_data["model_name"];
    std::string token = model_data["token"];

    if (model_data.find("texture_path") != model_data.end()) {
        model.texture_id = textureInit(model_data["texture_path"]);
    }

    if (model_data.find("scale") != model_data.end()) {
        model.scale = json_to_vec3(model_data["scale"]);
    }

    // transparent
    if (model_data.find("transparent") != model_data.end()) {
        model.transparent = model_data["transparent"];
    }

    // collectible
    if (model_data.find("collectible") != model_data.end()) {
        model.collectible = model_data["collectible"];
    }

    // collect type
    if (model_data.find("collect_type") != model_data.end()) {
        model.collect_type = model_data["collect_type"];
    }

    // value
    if (model_data.find("value") != model_data.end()) {
        model.value = model_data["value"];
    }

    // light source
    if (model_data.find("light_source") != model_data.end()) {
        model.light_source = model_data["light_source"];
    }

    if (model_data.find("isEnemy") != model_data.end()) {
        model.isEnemy = model_data["isEnemy"];
    }
    if (model_data.find("radius") != model_data.end()) {
        model.radius = model_data["radius"];
    }
    if (model_data.find("health") != model_data.end()) {
        model.health = model_data["health"];
    }

    if (name == "sprite") {
        model.isSprite = true;
    }
    return model;
}