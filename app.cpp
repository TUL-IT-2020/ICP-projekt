// C++
#include <GL/glew.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include <string>
#include <thread>
#ifdef _WIN32
#include <GL/wglew.h>
#endif

// OpenGL math
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGUI headers
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "App.hpp"

App::App() {
    // default constructor
    camera = Camera();
    player = Player(25, 0, 25, 3);
}

void App::init_glew(void) {
    //
    // Initialize all valid generic GL extensions with GLEW.
    // Usable AFTER creating GL context! (create with glfwInit(), glfwCreateWindow(),
    // glfwMakeContextCurrent()
    //
    {
        glewExperimental = GL_TRUE;
        GLenum glew_ret;
        glew_ret = glewInit();
        if (glew_ret != GLEW_OK) {
            throw std::runtime_error(std::string("GLEW failed with error: ") +
                                     reinterpret_cast<const char*>(glewGetErrorString(glew_ret)));
        } else {
            std::cout << "GLEW successfully initialized to version: " << glewGetString(GLEW_VERSION)
                      << "\n";
        }

        //// Platform specific init. (Change to GLXEW or ELGEW if necessary.)
        // GLenum wglew_ret = wglewInit();
        // if (wglew_ret != GLEW_OK) {
        //	throw std::runtime_error(std::string("WGLEW failed with error: ") +
        //reinterpret_cast<const char*>(glewGetErrorString(wglew_ret)));
        // }
        // else {
        //	std::cout << "WGLEW successfully initialized platform specific functions.\n";
        // }
    }
}

void App::init_glfw(void) {
    /* Initialize the library */
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        throw std::runtime_error("GLFW can not be initialized.");
    }

    // try to open OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    // open window, but hidden - it will be enabled later, after asset initialization
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1000, 800, "ICP", nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("GLFW window can not be created.");
    }

    glfwSetWindowUserPointer(window, this);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Debug: vypiš skutečnou verzi OpenGL a renderer
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    std::cout << "OpenGL version: " << (version ? (const char*)version : "UNKNOWN") << std::endl;
    std::cout << "OpenGL renderer: " << (renderer ? (const char*)renderer : "UNKNOWN") << std::endl;

    // Workaround for GLEW+Mesa+Core profile bug
    glGetError();

    // disable mouse cursor
    // disable cursor, so that it can not leave window, and we can process movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLFW callbacks registration

    /*glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
            app->cursorPositionCallback(window, xpos, ypos);
    }); */
    glfwSetFramebufferSizeCallback(window, fbsize_callback);  // On GL framebuffer resize callback.
    glfwSetScrollCallback(window, glfw_scroll_callback);      // On mouse wheel.
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetKeyCallback(window, glfw_key_callback);

    // use Z buffer
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    // enable back face culling
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
}

GLuint App::gen_tex(cv::Mat& image, TextureFilter filter = TextureFilter::TrilinearMipmap) {
    GLuint ID;

    if (image.empty()) {
        throw std::runtime_error("Image empty?\n");
    }

    // Generates an OpenGL texture object
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << err << std::endl;
        std::cout << "Image channels: " << image.channels() << std::endl;
    }

    switch (image.channels()) {
        case 3:
            // Create and clear space for data - immutable format
            glTextureStorage2D(ID, 1, GL_RGB8, image.cols, image.rows);
            // Assigns the image to the OpenGL Texture object
            glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE,
                                image.data);
            break;
        case 4:
            glTextureStorage2D(ID, 1, GL_RGBA8, image.cols, image.rows);
            glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGRA, GL_UNSIGNED_BYTE,
                                image.data);
            break;
        default:
            throw std::runtime_error(
                "texture failed");  // Check the image, we want Alpha in this example
    }

    switch (filter) {
        case TextureFilter::Nearest:
            glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;

        case TextureFilter::Bilinear:
            glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;

        case TextureFilter::TrilinearMipmap:
            glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // bilinear magnifying
            glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_LINEAR);  // trilinear minifying
            glGenerateTextureMipmap(ID);                   // Generate mipmaps
            break;
    }

    // Configures the way the texture repeats
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return ID;
}

bool App::webcam_init() {
    //open capture device
    //open first available camera
#ifdef _WIN32
    // Windows
    capture = cv::VideoCapture(cv::CAP_DSHOW);
#elif __linux__
    // Linux
    capture = cv::VideoCapture(0, cv::CAP_V4L2);
#else
    // Other OS (macOS, etc.)
    capture = cv::VideoCapture(0);
#endif
    
    //open video file
    //capture = cv::VideoCapture("video.mkv");

    if (!capture.isOpened()) { 
        std::cerr << "no source?" << std::endl;
        return false;
    } else {
        std::cout << "Source: " << 
            ": width=" << capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
            ", height=" << capture.get(cv::CAP_PROP_FRAME_HEIGHT) << '\n';
    }
    return true;
}

void App::clasificator_init() {
    // Initialize the face cascade classifier with the appropriate file
    std::string face_cascade_name = "resources/haarcascade_frontalface_default.xml";
    if (!face_cascade.load(face_cascade_name)) {
        throw std::runtime_error(
            "Error loading face cascade classifier from file: " + face_cascade_name);
    }
}

bool App::init() {
    try {
        std::cout << "Current working directory: "
                  << std::filesystem::current_path().generic_string() << '\n';

        if (!std::filesystem::exists("bin"))
            throw std::runtime_error("Directory 'bin' not found. DLLs are expected to be there.");

        if (!std::filesystem::exists("resources"))
            throw std::runtime_error(
                "Directory 'resources' not found. Various media files are expected to be there.");

        init_opencv();

        clasificator_init();
        if (!webcam_init()) {
            std::cerr << "Webcam not available, using keyboard input instead." << std::endl;
            webcam_connected = false;  // if webcam is not available, use keyboard input
        }

        init_glfw();
        init_glew();

        init_gl_debug();

        print_opencv_info();
        print_glfw_info();
        print_gl_info();
        print_glm_info();

        glfwSwapInterval(is_vsync_on ? 1 : 0);  // vsync

        // For future implementation: init assets (models, sounds, textures, level map, ...)
        // (this may take a while, app window is hidden in the meantime)...
        if (!GLEW_ARB_direct_state_access) {
            throw std::runtime_error("No DSA :-(");
        }
        init_assets();

        // When all is loaded, show the window.

        glfwShowWindow(window);
        // glfwFocusWindow(window);

        // Initialize ImGUI (see https://github.com/ocornut/imgui/wiki/Getting-Started)
        init_imgui();
    } catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << std::endl;
        throw;
    }

    return true;
}

void App::init_imgui() {
    // see https://github.com/ocornut/imgui/wiki/Getting-Started

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    std::cout << "ImGUI version: " << ImGui::GetVersion() << "\n";
}

void App::init_gl_debug() {
    if (GLEW_ARB_debug_output) {
        glDebugMessageCallback(MessageCallback, 0);
        glEnable(GL_DEBUG_OUTPUT);
        std::cout << "GL_DEBUG enabled." << std::endl;

        // filter notification noise...
        // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0,
        // nullptr, GL_FALSE);
    }
}

GLuint textureInit(const std::filesystem::path& file_name) {
    cv::Mat image =
        cv::imread(file_name.string(), cv::IMREAD_UNCHANGED);  // Read with (potential) Alpha
    if (image.empty()) {
        image = createCheckerboardTexture();
        // throw std::runtime_error("No texture in file: " + file_name.string());
        std::cerr << "No texture in file: " << file_name.string() << std::endl;
    }

    cv::flip(image, image, 0);

    GLuint texture = App::gen_tex(image, TextureFilter::TrilinearMipmap);

    return texture;
}

/*
 * Initialize the map for the given level and generate the scene.
 * This function reads the level from a file, initializes the map,
 * and places models in the scene based on the map data.
 * @param level: The level number to initialize.
 */
void App::init_map_for_level_and_generate_scene(int level) {
	// Initialize the map with a default level
	// map = Map(10, 25);
	// path to the level file
	std::string level_file = "resources/level0" + std::to_string(level) + ".txt";
    map = Map(level_file);
    map.printMap();

    // size offset
    glm::vec3 offset = glm::vec3(1.0, 0.0, 1.0);

	// clear models
	models.clear();

    // place models to the scene
    int light_source_count = 1;
    glm::vec3 lightPositionWorld = glm::vec3(10.0f, 15.0f, 10.0f);
    glm::vec3 ambientIntensity = glm::vec3(0.3f);
    glm::vec3 diffuseIntensity = glm::vec3(0.1f);
    glm::vec3 specularIntensity = glm::vec3(0.1f);
    Light light = Light(
        lightPositionWorld, ambientIntensity, diffuseIntensity, specularIntensity);
    lights.push_back(light);

    for (int j = 0; j < map.getRows(); j++) {
        for (int i = 0; i < map.getCols(); i++) {
            std::string token = std::string(1, map.fetchMapValue(i, j));
            if (map_2_model_dict.find(token) != map_2_model_dict.end()) {
                Model& base = map_2_model_dict[token];
                glm::vec3 pos = glm::vec3(i, 0, j) + offset;
                if (base.isDoor) {
                    auto door = std::make_unique<Door>(base);
                    door->origin = pos;
                    models.push_back(std::move(door));
                } else {
                    auto model = std::make_unique<Model>(base);
                    model->origin = pos;
                    models.push_back(std::move(model));
                }
                if (base.light_source) {
                    // add light source
                    Light light = Light(
                        pos,
                        //base.origin
                        base.ambientLight,
                        base.diffuseLight,
                        base.specularLight);
                    
                    lights.push_back(light);
                    light_source_count++;
                }
            }
        }
    }

    std::cout << "Light source added (" << light_source_count << " total)" << std::endl;
    if (light_source_count > MAX_LIGHTS) {
        std::cerr << "Warning: More light sources than supported by MAX_LIGHTS (" << MAX_LIGHTS
                  << "). Some lights may not be rendered." << std::endl;
    }

    // add floor
    Model floor = map_2_model_dict["floor"];
    // change scale
    floor.scale = glm::vec3(map.getCols(), 1.0, map.getRows());
    // change origin
    floor.origin = glm::vec3(0.5, -1.0, 0.5);
    models.push_back(std::make_unique<Model>(floor));

    // set player position in 3D space (transform X-Y in map to XYZ in GL)
    camera.Position.x = (map.start_position.x) + 1.0 / 2.0f;
    camera.Position.z = (map.start_position.y) + 1.0 / 2.0f;
    camera.Position.y = camera.camera_height;

    std::cout << "Scene generated." << std::endl;
}

/*
 * Initialize pipeline: compile, link and use shaders
 * Create and load data into GPU using OpenGL DSA (Direct State Access)
 */
void App::init_assets(void) {
    // Load models from JSON file
    std::ifstream models_file("resources/models.json");
    if (!models_file.is_open()) {
        throw std::runtime_error("Could not open JSON file.");
    }

    nlohmann::json json;
    models_file >> json;

    for (const auto& model_data : json["models"]) {
        std::cout << "Loading model: " << model_data["name"] << std::endl;
        try {
            Model model(model_data, shader_cache);
            if (model_data.find("texture_path") != model_data.end()) {
                model.texture_id = textureInit(model_data["texture_path"]);
                std::cout << "Texture loaded: " << model_data["texture_path"] << std::endl;
                std::cout << "Texture ID: " << model.texture_id << std::endl;
            }
            model_cache[model.name] = model;
        } catch (std::exception const& e) {
            std::cerr << "Loading model failed : " << model_data["name"] << ", " << e.what()
                      << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    std::cout << "Models loaded." << std::endl;

    /*
     */
    // load map objects to model cache
    std::ifstream map_2_models("resources/map_2_models.json");
    if (!map_2_models.is_open()) {
        throw std::runtime_error("Could not open JSON file.");
    }

    map_2_models >> json;

    for (const auto& model_data : json["map_2_models"]) {
        try {
            // copy model from cache
            Model model = model_cache[model_data["model_name"]];

            map_2_model_dict[model_data["token"]] = Model::parse_json_to_model(model_data, model, model_cache);
        } catch (std::exception const& e) {
            std::cerr << "ERROR loading textured model: " << model_data["model_name"] << ", "
                      << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::cout << "Models loaded." << std::endl;

    // print loaded collectible objects
    std::cout << "Collectible objects: ";
    for (const auto& obj : map_2_model_dict) {
        if (obj.second.collectible) {
            std::cout << obj.first << " ";
        }
    }
    std::cout << std::endl;

    // print loaded light sources
    std::cout << "Light sources: ";
    for (const auto& obj : map_2_model_dict) {
        if (obj.second.light_source) {
            std::cout << obj.first << " ";
        }
    }
    std::cout << std::endl;

    // print loaded enemies
    std::cout << "Enemies: ";
    for (const auto& obj : map_2_model_dict) {
        if (obj.second.isEnemy) {
            std::cout << obj.first << " ";
        }
    }
    std::cout << std::endl;

    // print loaded doors
    std::cout << "Doors: ";
    for (const auto& obj : map_2_model_dict) {
        if (obj.second.isDoor) {
            std::cout << obj.first << " ";
        }
    }
    std::cout << std::endl;

	// end level objects
	std::cout << "End level objects: ";
	for (const auto& obj : map_2_model_dict) {
		if (obj.second.end_level) {
			std::cout << obj.first << " ";
		}
	}
	std::cout << std::endl;

    // load level
	init_map_for_level_and_generate_scene(level);

    // load status bar
    status_bar = std::make_shared<StatusBar>(model_cache["statusbar"]);
    status_bar->loadAssets();

    std::cout << "Status bar loaded." << std::endl;
}

void App::print_opencv_info() {
    std::cout << "Capture source: "
              //<<  ": width=" << capture.get(cv::CAP_PROP_FRAME_WIDTH)
              //<<  ", height=" << capture.get(cv::CAP_PROP_FRAME_HEIGHT)
              << '\n';
}

void App::print_glfw_info(void) {
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    std::cout << "Running GLFW DLL " << major << '.' << minor << '.' << revision << '\n';
    std::cout << "Compiled against GLFW " << GLFW_VERSION_MAJOR << '.' << GLFW_VERSION_MINOR << '.'
              << GLFW_VERSION_REVISION << '\n';
}

void App::print_glm_info() {
    // GLM library
    std::cout << "GLM version: " << GLM_VERSION_MAJOR << '.' << GLM_VERSION_MINOR << '.'
              << GLM_VERSION_PATCH << "rev" << GLM_VERSION_REVISION << std::endl;
}

void App::print_gl_info() {
    // get OpenGL info
    auto vendor_s = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    std::cout << "OpenGL driver vendor: " << (vendor_s ? vendor_s : "UNKNOWN") << '\n';

    auto renderer_s = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    std::cout << "OpenGL renderer: " << (renderer_s ? renderer_s : "<UNKNOWN>") << '\n';

    GLint maxTextureUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "Max texture units supported by GPU: " << maxTextureUnits << std::endl;

    auto version_s = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    std::cout << "OpenGL version: " << (version_s ? version_s : "<UNKNOWN>") << '\n';

    auto glsl_s = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    std::cout << "Primary GLSL shading language version: " << (glsl_s ? glsl_s : "<UNKNOWN>")
              << std::endl;

    // get GL profile info
    {
        GLint profile_flags;
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_flags);
        std::cout << "Current profile: ";
        if (profile_flags & GL_CONTEXT_CORE_PROFILE_BIT)
            std::cout << "CORE";
        else
            std::cout << "COMPATIBILITY";
        std::cout << std::endl;
    }

    // get context flags
    {
        GLint context_flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
        std::cout << "Active context flags: ";
        if (context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
            std::cout << "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT ";
        if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) std::cout << "GL_CONTEXT_FLAG_DEBUG_BIT ";
        /*if (context_flags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT)
                std::cout << "GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT ";
        if (context_flags & GL_CONTEXT_FLAG_NO_ERROR_BIT)
                std::cout << "GL_CONTEXT_FLAG_NO_ERROR_BIT";*/
        std::cout << std::endl;
    }

    {  // get extension list
        GLint n = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        std::cout << "GL extensions: " << n << '\n';

        // for (GLint i = 0; i < n; i++) {
        //     const char* extension_name = (const char*)glGetStringi(GL_EXTENSIONS, i);
        //     std::cout << extension_name << '\n';
        // }
    }
}

void App::init_opencv() {
    // ...
}

/* Will copmare camera position with map and return true if there is no collision
 */
bool App::CheckHitboxes(glm::vec3 movement) {
    // Prodloužení směru kroku
    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement) * (glm::length(movement) + 0.2f);
    }
    // camera adjustment
    glm::vec3 camera_position = camera.Position;
    // get camera position in map
    int camera_x = (int)camera_position.x;
    int camera_y = (int)camera_position.z;
    // new position
    camera_x += movement.x;
    camera_y += movement.z;

    // check if camera is out of map
    if (map.outOfBounds(camera_x, camera_y)) {
        return false;
    }
    glm::vec3 new_camera_position = camera_position + movement;
    for (auto& obj : models) {
        if (obj->isSolid) {
            // Get the object's AABB
            glm::vec3 obj_min = obj->origin - obj->scale / 2.0f;
            glm::vec3 obj_max = obj->origin + obj->scale / 2.0f;

            // Get the camera's AABB
            glm::vec3 camera_min = new_camera_position - glm::vec3(player.radius);
            glm::vec3 camera_max = new_camera_position + glm::vec3(player.radius);

            // Check for intersection
            if (aabb_intersect(obj_min, obj_max, camera_min, camera_max)) {
                //std::cout << "Collision detected with object: " << obj->name << " " << obj->collectible << std::endl;
                return false;
            }
        }
    }
    return true;
}

void App::thread_code(void) {
    double now = glfwGetTime();
    // FPS related
    double fps_last_displayed = now;
    int fps_counter_frames = 0;
    double FPS = 0.0;

    // animation related
    double frame_begin_timepoint = now;
    double previous_frame_render_time{};
    double time_speed{};

    // Clear color
    glClearColor(0.2, 0.2, 0.2, 1.0);

    // get first position of mouse cursor
    glfwGetCursorPos(window, &cursorLastX, &cursorLastY);

    // first update
    glfwGetFramebufferSize(window, &width, &height);
    update_projection_matrix();
    glViewport(0, 0, width, height);

    double last_frame_time = glfwGetTime();

    glm::vec3 offset = glm::vec3(0.0);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale_change = glm::vec3(1.0f);

    char movement_local = 'n';

    while (!glfwWindowShouldClose(window)) {
        // new level?
        if (load_new_level) {
            load_new_level = false; // reset flag
            level++;
            status_bar->setLevel(level);
            try {
                init_map_for_level_and_generate_scene(level);
                std::cout << "Level " << level << " initialized." << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error initializing level " << level << ": " << e.what() << std::endl;
                glfwSetWindowShouldClose(window, GLFW_TRUE); // Exit on error
            }
        }

        // =================================================================
        //                          IMGUI & UPDATE
        // =================================================================
        if (show_imgui) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::SetNextWindowSize(ImVec2(350, 190));
            ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", camera.Position.x,
                        camera.Position.z, camera.Position.y);
            ImGui::Text("Camera direction: (%.2f, %.2f, %.2f)", camera.Yaw, camera.Pitch,
                        camera.Roll);
            ImGui::Text("V-Sync: %s", is_vsync_on ? "ON" : "OFF");
            ImGui::Text("FPS: %.1f", FPS);
            ImGui::Text("Player Health: %d, Gold: %d, Ammo: %d", player.health, player.gold,
                        player.ammo);
            ImGui::Text("(press UP/DOWN to change color)");
            ImGui::Text("(press RMB to release mouse)");
            ImGui::Text("(hit C to show/hide info)");
            ImGui::Text("(hit V to toggle V-Sync)");
            ImGui::End();
        }

        if (show_imgui) {
            time_speed = 0.0;
        } else {
            time_speed = 1.0;
        }

        double delta_t = glfwGetTime() - last_frame_time;
        last_frame_time = glfwGetTime();
        glm::vec3 movement = camera.ProcessInput(window, delta_t);
        // get global variables
        if (webcam_connected) {
			std::lock_guard<std::mutex> lock(mtx);
			if (new_coordinates) {
				new_coordinates = false;
				movement_local = movement_from_webcam;
				
				std::cout << "movement: " << movement_local << std::endl;
			}
		}
        if (movement_local != 'n') {
            switch (movement_local) {
                case 'r': 
                    camera.rotateRight(delta_t);
                    break;
                case 'l': 
                    camera.rotateLeft(delta_t);
                    break;
            }
        }

        if (CheckHitboxes(movement) || camera.freeCam) {
            camera.UpdateCameraPosition(movement);
        }

        // --- Game logic for collectibles, bullets, etc. remains the same ---
        float radius = 0.7f;
        for (auto it = models.begin(); it != models.end();) {
            if ((*it)->collectible) {
                float dist = glm::distance(camera.Position, (*it)->origin);
                if (dist < radius) {
                    std::cout << "Collected item: " << (*it)->collect_type << std::endl;
                    if ((*it)->collect_type == "gold") {
                        player.gold += (*it)->value;
                    } else if ((*it)->collect_type == "health") {
                        player.health += (*it)->value;
                        if (player.health > 100) player.health = 100;
                    } else if ((*it)->collect_type == "ammo") {
                        player.ammo += (*it)->value;
                    } else if ((*it)->collect_type == "life") {
                        player.lives += (*it)->value;
                    } else if ((*it)->collect_type == "weapon") {
                        // TODO: make this more robust
                        if ((*it)->value == 1) {
                            player.current_weapon = weapon_type::MACHINEGUN;
                        } else if ((*it)->value == 2) {
                            player.current_weapon = weapon_type::CHAINGUN;
                        } else {
                            std::cout << "Unknown weapon value: " << (*it)->value << std::endl;
                        }
                        std::cout << "Switched to weapon: " << player.current_weapon
                                    << std::endl;
                    }
                    it = models.erase(it);
                    continue;
                }
            }
            ++it;
        }
        for (auto& bullet : bullets) {
            if (bullet.active)
                bullet.position += bullet.direction * bullet.speed * float(delta_t);
        }
        for (auto& bullet : bullets) {
            if (!bullet.active) continue;
            for (auto it = models.begin(); it != models.end();) {
                if ((*it)->isEnemy) {
                    float dist = glm::distance(bullet.position, (*it)->origin);
                    if (dist < (*it)->radius) {
                        bullet.active = false;
                        (*it)->health -= bullet.damage;
                        std::cout << "Bullet hit enemy: " << (*it)->name
                                    << ", remaining health: " << (*it)->health << std::endl;
                        if ((*it)->health <= 0) {
                            Model corpse = map_2_model_dict["d"];
                            corpse.origin = (*it)->origin;
                            models.push_back(std::make_unique<Model>(corpse));
                            std::cout << "Enemy killed: " << (*it)->name << std::endl;
                            it = models.erase(it);
                            continue;
                        }
                    }
                }
                if ((*it)->isSolid) {
                    // Check for collision with solid objects
                    glm::vec3 obj_min = (*it)->origin - (*it)->scale / 2.0f;
                    glm::vec3 obj_max = (*it)->origin + (*it)->scale / 2.0f;
                    glm::vec3 bullet_min = bullet.position - glm::vec3(bullet.radius);
                    glm::vec3 bullet_max = bullet.position + glm::vec3(bullet.radius);
                    if (aabb_intersect(obj_min, obj_max, bullet_min, bullet_max)) {
                        bullet.active = false;  // Bullet hit a solid object
                        std::cout << "Bullet hit solid object: " << (*it)->name << std::endl;
                    }
                }
                ++it;
            }
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                        [](const Bullet& bullet) { return !bullet.active; }),
                        bullets.end());

        // =================================================================
        //                          RENDER
        // =================================================================
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get matrices once per frame
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        // Prepare for transparency
        std::vector<Model*> transparent;
        transparent.reserve(models.size());

        // --- OPAQUE OBJECTS RENDER PASS ---
        for (auto& model : models) {
            model->update(delta_t);
            if (!model->transparent) {
                // Get the specific shader for THIS model
                ShaderProgram& shader = model->meshes[0].shader;
                shader.activate();

                // Set matrices required by ALL shaders
                shader.setUniform("v_m", viewMatrix);
                shader.setUniform("p_m", projection_matrix);

                // If it's the lighting shader, set the lighting uniforms
                if (shader.hasUniform("light_position")) {
                    // Set the light position in view space
                    const Light& light = lights[0];
                    glm::vec3 lightPositionView = glm::vec3(viewMatrix * glm::vec4(light.position, 1.0f));
                    shader.setUniform("light_position", lightPositionView);
                    // Set the light properties
                    shader.setUniform("ambient_intensity", light.ambient);
                    shader.setUniform("diffuse_intensity", light.diffuse);
                    shader.setUniform("specular_intensity", light.specular);
                } else {
                    for (size_t i = 0; i < lights.size() && i < MAX_LIGHTS; ++i) {
                        const Light& light = lights[i];
                        bool visible = true;
                        /*
                        // raycasting to check if the light is visible
                        if (i > 1) {
                            glm::vec3 from = model->origin;
                            glm::vec3 to = light.position;
                            glm::vec3 dir = glm::normalize(to - from);
                            float distance = glm::distance(from, to);
                            float step = 0.2f; // krok v mapě
                            for (float d = step; d < distance; d += step) {
                                glm::vec3 pos = from + dir * d;
                                int x = int(pos.x);
                                int z = int(pos.z);
                                if (map.containsWall(x, z)) {
                                    visible = false;
                                    break;
                                }
                            }
                        }
                        */
                        shader.setUniform("lights[" + std::to_string(i) + "].isActive", visible);
                        shader.setUniform("lights[" + std::to_string(i) + "].position",
                                            light.position);
                        shader.setUniform("lights[" + std::to_string(i) + "].ambient_intensity",
                                            light.ambient);
                        shader.setUniform("lights[" + std::to_string(i) + "].diffuse_intensity",
                                            light.diffuse);
                        shader.setUniform("lights[" + std::to_string(i) + "].specular_intensity",
                                            light.specular);
                    }
                }

                rotation = glm::vec3(0.0f);
                if (model->isSprite) {
                    glm::vec3 sprite_position = model->origin;
                    glm::vec3 camera_position = camera.Position;
                    glm::vec3 direction = glm::normalize(camera_position - sprite_position);
                    float angle = atan2(direction.x, direction.z);
                    rotation = glm::vec3(0.0f, angle, 0.0f);
                }
                model->draw(offset, rotation, scale_change);
            } else {
                transparent.emplace_back(model.get());
            }
        }

        // Sort transparent objects
        std::sort(transparent.begin(), transparent.end(), [&](Model const* a, Model const* b) {
            return glm::distance(camera.Position, a->origin) >
                    glm::distance(camera.Position, b->origin);
        });

        // Set GL state for transparency
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        // --- TRANSPARENT OBJECTS RENDER PASS ---
        for (auto model : transparent) {
            // Get the specific shader for THIS model
            ShaderProgram& shader = model->meshes[0].shader;
            shader.activate();

            // Set matrices required by ALL shaders
            shader.setUniform("v_m", viewMatrix);
            shader.setUniform("p_m", projection_matrix);

            rotation = glm::vec3(0.0f);
            if (model->isSprite) {
                glm::vec3 sprite_position = model->origin;
                glm::vec3 camera_position = camera.Position;
                glm::vec3 direction = glm::normalize(camera_position - sprite_position);
                float angle = atan2(direction.x, direction.z);
                rotation = glm::vec3(0.0f, angle, 0.0f);
            }
            model->draw(offset, rotation, scale_change);
        }

        // Restore GL state
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);

        // --- UI & FINAL PRESENTATION ---
        status_bar->update(player);
        status_bar->draw(glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0));

        if (show_imgui) {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        now = glfwGetTime();
        previous_frame_render_time = now - frame_begin_timepoint;
        frame_begin_timepoint = now;

        fps_counter_frames++;
        if (now - fps_last_displayed >= 1) {
            FPS = fps_counter_frames / (now - fps_last_displayed);
            fps_last_displayed = now;
            fps_counter_frames = 0;
            std::cout << "\r[FPS]" << FPS << "      ";
        }
    }
	process_images = false;
}

cv::Point2f App::find_face(cv::Mat & frame) {
    cv::Point2f center(0.0f, 0.0f); // for result

	cv::Mat scene_grey;
	cv::cvtColor(frame, scene_grey, cv::COLOR_BGR2GRAY);

	std::vector<cv::Rect> faces;
	face_cascade.detectMultiScale(scene_grey, faces);
        
	if (faces.size() > 0) {
		center.x = (faces[0].x + faces[0].width / 2.0f) / frame.cols;
		center.y = (faces[0].y + faces[0].height / 2.0f) / frame.rows;
	}
    return center;      
}

void App::draw_cross_relative(cv::Mat& img, const cv::Point2f center_relative, const int size) {
	cv::Point p;
	p.x = center_relative.x * img.cols;
	p.y = center_relative.y * img.rows;

	cv::Point p1 = cv::Point(p.x - size / 2, p.y);
	cv::Point p2 = cv::Point(p.x + size / 2, p.y);
	cv::Point p3 = cv::Point(p.x, p.y - size / 2);
	cv::Point p4 = cv::Point(p.x, p.y + size / 2);

	cv::line(img, p1, p2, CV_RGB(255, 0, 0), 3);
	cv::line(img, p3, p4, CV_RGB(255, 0, 0), 3);
}

char App::webcam_to_movement(const cv::Point2f& center) {
	// convert webcam coordinates to movement
	char movement_from_webcam = 'n';
	if (center.y == 0.0f && center.x == 0.0f) {
        return 'n'; // no face detected
    } else if (center.x < 0.4f) {
		movement_from_webcam = 'r';
	} else if (center.x > 0.6f) {
		movement_from_webcam = 'l';
	} else {
		movement_from_webcam = 'n'; // no movement_from_webcam
	}
	return movement_from_webcam;
}

void App::UI_thread() {
	cv::Point2f center_local;
	cv::Mat frame_local;
	char movement_local;
	// wait for first frame
	while (!new_coordinates) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		// get global variables
		{
			std::lock_guard<std::mutex> lock(mtx);
			if (new_coordinates) {
				center_local = center;
				frame.copyTo(frame_local);
				new_coordinates = false;
				movement_local = movement_from_webcam;
				
				std::cout << "found normalized center: " << center << std::endl;
				std::cout << "movement_from_webcam: " << movement_local << std::endl;
			}
		}

		//display result
		cv::Mat scene_cross;
		frame_local.copyTo(scene_cross);
        draw_cross_relative(scene_cross, center, 30);
		cv::imshow("scene", scene_cross);
	} while (cv::pollKey() != 27 || cam_disconnected); //message loop untill ESC
	user_exit = true;
	process_images = false;
}

void App::camera_processing_thread() {
	cv::Mat frame_local;
	cv::Point2f center_local;
	do {
		capture.read(frame_local);
		if (frame_local.empty())
		{
			cam_disconnected = true;
			break;
		}

		// find face
        center_local = find_face(frame_local);

		char movement_local = webcam_to_movement(center_local);

		// update global variables
		{
			std::lock_guard<std::mutex> lock(mtx);
			//frame_local.copyTo(frame);	// TODO: use shared pointer (swap)
			center = center_local;
			movement_from_webcam = movement_local;
			new_coordinates = true;
		}
	} while (process_images);
}

int App::run(void) {
    // start camera processing thread
	std::thread camera_thread(std::bind(&App::camera_processing_thread, this));

	// Game rendering and logic thread
    try {
	    thread_code();
    } catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
	
	if (cam_disconnected) {
		std::cerr << "Camera disconnected\n";
		return EXIT_FAILURE;
	}

	if (user_exit) {
		std::cout << "User exit\n";
		return EXIT_SUCCESS;
	}

	camera_thread.join();

	return EXIT_SUCCESS;
}

void App::destroy(void) {
    // clean up ImGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // clean up OpenCV
    cv::destroyAllWindows();

    // clean-up GLFW
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

App::~App() {
    models.clear();  // Clear the vector to release the memory

    destroy();
    std::cout << "Bye...\n";

    if (capture.isOpened()) {
        capture.release();
    }
}
