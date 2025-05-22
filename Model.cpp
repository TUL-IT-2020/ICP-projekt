#include "Model.hpp"
#include "Door.hpp"
#include "assets.hpp"

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

    if (model_data.find("type") != model_data.end()) {
        const std::string type = model_data["type"];
        if (type == "enemy") {
            model.isEnemy = true;
        } else if (type == "collectible") {
            model.collectible = true;
        } else if (type == "door") {
            model.isDoor = true;
        }
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