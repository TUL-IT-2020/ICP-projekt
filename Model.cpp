#include "Model.hpp"
#include "Door.hpp"
#include "assets.hpp"

void load_value_from_json(const nlohmann::json& json, const std::string& key, int& value) {
    if (json.find(key) != json.end()) {
        value = json[key];
    }
}

void load_value_from_json(const nlohmann::json& json, const std::string& key, bool& value) {
    if (json.find(key) != json.end()) {
        value = json[key];
    }
}

void load_value_from_json(const nlohmann::json& json, const std::string& key, float& value) {
    if (json.find(key) != json.end()) {
        value = json[key];
    }
}

void load_value_from_json(const nlohmann::json& json, const std::string& key, std::string& value) {
    if (json.find(key) != json.end()) {
        value = json[key];
    }
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

    load_value_from_json(model_data, "transparent", model.transparent);
    load_value_from_json(model_data, "solid", model.isSolid);
    load_value_from_json(model_data, "collectible", model.collectible);
    load_value_from_json(model_data, "collect_type", model.collect_type);
    load_value_from_json(model_data, "value", model.value);
    load_value_from_json(model_data, "light_source", model.light_source);
    load_value_from_json(model_data, "radius", model.radius);
    load_value_from_json(model_data, "health", model.health);

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

    if (name == "sprite") {
        model.isSprite = true;
    }
    return model;
}