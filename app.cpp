// C++
#include <thread>
#include <chrono>
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>

#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif

// OpenGL math
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

// ImGUI headers
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "App.hpp"
#include "assets.hpp"
#include "ShaderProgram.hpp"

App::App() {
	// default constructor
	camera = Camera();
}

void App::init_glew(void) {
	//
	// Initialize all valid generic GL extensions with GLEW.
	// Usable AFTER creating GL context! (create with glfwInit(), glfwCreateWindow(), glfwMakeContextCurrent()
	//
	{
		GLenum glew_ret;
		glew_ret = glewInit();
		if (glew_ret != GLEW_OK) {
			throw std::runtime_error(std::string("GLEW failed with error: ") + reinterpret_cast<const char*>(glewGetErrorString(glew_ret)));
		}
		else {
			std::cout << "GLEW successfully initialized to version: " << glewGetString(GLEW_VERSION) << "\n";
		}

		//// Platform specific init. (Change to GLXEW or ELGEW if necessary.)
		//GLenum wglew_ret = wglewInit();
		//if (wglew_ret != GLEW_OK) {
		//	throw std::runtime_error(std::string("WGLEW failed with error: ") + reinterpret_cast<const char*>(glewGetErrorString(wglew_ret)));
		//}
		//else {
		//	std::cout << "WGLEW successfully initialized platform specific functions.\n";
		//}
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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// open window, but hidden - it will be enabled later, after asset initialization
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1000, 800, "ICP", nullptr, nullptr);
	if (!window) {
		throw std::runtime_error("GLFW window can not be created.");
	}

	glfwSetWindowUserPointer(window, this);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// disable mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLFW callbacks registration
	//glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	glfwSetFramebufferSizeCallback(window, fbsize_callback);    // On GL framebuffer resize callback.
    glfwSetScrollCallback(window, glfw_scroll_callback);        // On mouse wheel.
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetKeyCallback(window, glfw_key_callback);

	// use Z buffer
	glEnable(GL_DEPTH_TEST);
}

bool App::init() {
	try {
		std::cout << "Current working directory: " << std::filesystem::current_path().generic_string() << '\n';

		if (!std::filesystem::exists("bin"))
			throw std::runtime_error("Directory 'bin' not found. DLLs are expected to be there.");

		if (!std::filesystem::exists("resources"))
			throw std::runtime_error("Directory 'resources' not found. Various media files are expected to be there.");

		init_opencv();
		init_glfw();
		init_glew();

		init_gl_debug();

		print_opencv_info();
		print_glfw_info();
		print_gl_info();
		print_glm_info();

		glfwSwapInterval(is_vsync_on ? 1 : 0); // vsync

		// For future implementation: init assets (models, sounds, textures, level map, ...)
		// (this may take a while, app window is hidden in the meantime)...
		if (!GLEW_ARB_direct_state_access) {
			throw std::runtime_error("No DSA :-(");
		}
		init_assets();

		// When all is loaded, show the window.

		glfwShowWindow(window);

		// Initialize ImGUI (see https://github.com/ocornut/imgui/wiki/Getting-Started)
		init_imgui();
	}
	catch (std::exception const& e) {
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
	if (GLEW_ARB_debug_output)
	{
		glDebugMessageCallback(MessageCallback, 0);
		glEnable(GL_DEBUG_OUTPUT);
		std::cout << "GL_DEBUG enabled." << std::endl;

		// filter notification noise...
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	}
}

glm::vec3 json_to_vec3(const nlohmann::json& json_array) {
    if (!json_array.is_array() || json_array.size() != 3) {
        throw std::invalid_argument("Invalid JSON format for vec3. Expected an array of size 3.");
    }
    return glm::vec3(json_array[0].get<float>(), json_array[1].get<float>(), json_array[2].get<float>());
}

/*
 * Initialize pipeline: compile, link and use shaders
 * Create and load data into GPU using OpenGL DSA (Direct State Access)
 */
void App::init_assets(void) {
    //Model model("resources/obj/teapot_tri_vnt.obj", shader);
	
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
			Model model(model_data, shader_cache); // Použití nového konstruktoru
			model_cache[model.name] = model;
		} catch (std::exception const& e) {
			std::cerr << "Loading model failed : " << model_data["name"] << ", " << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	std::cout << "Models loaded." << std::endl;
	
	// load position for objects -> models
	// place models to the scene
	std::ifstream map_file("resources/map.json");
	if (!map_file.is_open()) {
		throw std::runtime_error("Could not open JSON file.");
	}

	map_file >> json;

	for (const auto& model_data : json["scene"]) {
		std::cout << "Placing model: " << model_data["name"] << std::endl;
		try {
			std::string name = model_data["name"];
			
			// copy model from cache
			Model model = model_cache[name];

			if (model_data.find("origin") != model_data.end()) {
				model.origin = json_to_vec3(model_data["origin"]);
			}
			
			if (model_data.find("scale") != model_data.end()) {
				model.scale = json_to_vec3(model_data["scale"]);
			}
	
			models.push_back(model);

		} catch (std::exception const& e) {
			std::cerr << "ERROR placing model to scene: " << model_data["name"] << ", " << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	std::cout << "Scene generated." << std::endl;

	/*
	int n = 10000;
	for (int i = 0; i < n; i++) {
		std::string shader_key = "resources/shaders/defoult.vertresources/shaders/defoult.frag";
		Model model("resources/obj/triangle.obj", shader_cache[shader_key]);
		glm::vec3 origin{ 0.0f, 0.0f, 0.0f };
		model.origin = origin;
		model.name = "triangle";
		models.push_back(model);
	}
	*/
	
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
	std::cout << "Compiled against GLFW "
		<< GLFW_VERSION_MAJOR << '.' << GLFW_VERSION_MINOR << '.' << GLFW_VERSION_REVISION
		<< '\n';
}

void App::print_glm_info() {
	// GLM library
	std::cout << "GLM version: " << GLM_VERSION_MAJOR << '.' << GLM_VERSION_MINOR << '.' << GLM_VERSION_PATCH << "rev" << GLM_VERSION_REVISION << std::endl;
}

void App::print_gl_info() {
	// get OpenGL info
	auto vendor_s = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	std::cout << "OpenGL driver vendor: " << (vendor_s ? vendor_s : "UNKNOWN") << '\n';

	auto renderer_s = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	std::cout << "OpenGL renderer: " << (renderer_s ? renderer_s : "<UNKNOWN>") << '\n';

	auto version_s = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	std::cout << "OpenGL version: " << (version_s ? version_s : "<UNKNOWN>") << '\n';

	auto glsl_s = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	std::cout << "Primary GLSL shading language version: " << (glsl_s ? glsl_s : "<UNKNOWN>") << std::endl;

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
		if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			std::cout << "GL_CONTEXT_FLAG_DEBUG_BIT ";
		/*if (context_flags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT)
			std::cout << "GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT ";
		if (context_flags & GL_CONTEXT_FLAG_NO_ERROR_BIT)
			std::cout << "GL_CONTEXT_FLAG_NO_ERROR_BIT";*/
		std::cout << std::endl;
	}

	{ // get extension list
		GLint n = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);
		std::cout << "GL extensions: " << n << '\n';

		//for (GLint i = 0; i < n; i++) {
		//    const char* extension_name = (const char*)glGetStringi(GL_EXTENSIONS, i);
		//    std::cout << extension_name << '\n';
		//}
	}
}

void App::init_opencv()
{
	// ...
}

int App::run(void) {
	/*
	* Typical game loop:
		// INIT: Initial positions and state
		while (application_should_not_close)
		{
			// UPDATE: Update game state
			// RENDER: Render content
			// SWAP: Swap back/front buffer
			// VSYNC: Wait for vertical retrace (e.g. 1/60 of a second)
			// POLL: Poll events, dispatch
		}
	*/

	try {
		double now = glfwGetTime();
		// FPS related
		double fps_last_displayed = now;
		int fps_counter_frames = 0;
		double FPS = 0.0;

		// animation related
		double frame_begin_timepoint = now;
		double previous_frame_render_time{};
		double time_speed{};

		// Clear color saved to OpenGL state machine: no need to set repeatedly in game loop
		glClearColor(0, 0, 0, 0);


		/*
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		*/

		// disable cursor, so that it can not leave window, and we can process movement
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		// get first position of mouse cursor
		glfwGetCursorPos(window, &cursorLastX, &cursorLastY);
		
		// first update = manual (no event for update arrived yet)
		glfwGetFramebufferSize(window, &width, &height);    // Get GL framebuffer size	
		update_projection_matrix();
		glViewport(0, 0, width, height);

		camera.Position = glm::vec3(0, 0, 10);
		double last_frame_time = glfwGetTime();
		
		ShaderProgram& shader = models[0].meshes[0].shader;
		shader.activate();

		
		glm::vec3 offset = glm::vec3(0.0);
		glm::vec3 rotation = glm::vec3(1.0f);	
		glm::vec3 scale_change = glm::vec3(1.0f);

		while (!glfwWindowShouldClose(window)) {
			// ImGui prepare render (only if required)
			if (show_imgui) {
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				//ImGui::ShowDemoWindow(); // Enable mouse when using Demo!
				ImGui::SetNextWindowPos(ImVec2(10, 10));
				ImGui::SetNextWindowSize(ImVec2(350, 180));

				ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
				// X, Y, Z,
				ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
				ImGui::Text("V-Sync: %s", is_vsync_on ? "ON" : "OFF");
				ImGui::Text("FPS: %.1f", FPS);
				ImGui::Text("Triangle color: (%.2f, %.2f, %.2f)", triangle_color.r, triangle_color.g, triangle_color.b);
				ImGui::Text("(press UP/DOWN to change color)");
				ImGui::Text("(press RMB to release mouse)");
				ImGui::Text("(hit C to show/hide info)");
				ImGui::Text("(hit V to toggle V-Sync)");
				ImGui::End();
			}

			//
			// UPDATE: recompute object.position = object.position + object.speed * (previous_frame_render_time * time_speed); // s = s0 + v*delta_t
			//
			if (show_imgui) {
				// pause application
				time_speed = 0.0;
			}
			else {
				// imgui not displayed, run app at normal speed
				time_speed = 1.0;
			}

			//
			// RENDER: GL drawCalls
			// 

			// clear canvas
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// TODO:
			double delta_t = glfwGetTime() - last_frame_time;
			last_frame_time = glfwGetTime();
        	glm::vec3 movement = camera.ProcessInput(window, delta_t); // process keys etc.
			// update camera position
			if (camera.ValidMovement(movement)) {
				std::cout << "movement: " << movement.x << ", " << movement.y << ", " << movement.z << std::endl;
				camera.UpdateCameraPosition(movement);
			}

			shader.setUniform("uV_m", camera.GetViewMatrix());	
			shader.setUniform("uP_m", projection_matrix);

			// draw all models
			for (auto & model : models) {
				if (model.name == "triangle") {
					// update triangle color
					for (auto & mesh : model.meshes) {
						mesh.shader.activate();
						const glm::vec4 triangle_color_vec(triangle_color.r, triangle_color.g, triangle_color.b, 1.0f);
						mesh.shader.setUniform("uniform_Color", triangle_color_vec);
					}
				} else if (model.name == "teapot") {
					for (auto & mesh : model.meshes) {
						mesh.shader.activate();
						const glm::vec4 yellow_color(1.0f, 1.0f, 0.0f, 1.0f);
						mesh.shader.setUniform("uniform_Color", yellow_color);
					}
				} else {
					// blue color for other models
					for (auto & mesh : model.meshes) {
						mesh.shader.activate();
						const glm::vec4 blue_color(0.0f, 0.0f, 1.0f, 1.0f);
						mesh.shader.setUniform("uniform_Color", blue_color);
					}
				}
				//model.update(delta_t);
				//model.draw();
				rotation.x += 0.5f * delta_t;
				rotation.y += 0.5f * delta_t;
				rotation.z += 0.5f * delta_t;

				model.draw(offset, rotation, scale_change);
			}

			// ImGui display
			if (show_imgui) {
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}

			// SWAP + VSYNC
			glfwSwapBuffers(window);

			// POLL
			glfwPollEvents();

			// Time/FPS measurement
			now = glfwGetTime();
			previous_frame_render_time = now - frame_begin_timepoint; //compute delta_t
			frame_begin_timepoint = now; // set new start

			fps_counter_frames++;
			if (now - fps_last_displayed >= 1) {
				FPS = fps_counter_frames / (now - fps_last_displayed);
				fps_last_displayed = now;
				fps_counter_frames = 0;
				std::cout << "\r[FPS]" << FPS << "     "; // Compare: FPS with/without ImGUI
			}
		}
	}
	catch (std::exception const& e) {
		std::cerr << "App failed : " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

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
    models.clear(); // Clear the vector to release the memory

    destroy();

    std::cout << "Bye...\n";
}

/* Change the color of the triangle
 * clamps the color values to [0, 1], does not change the alpha value.
 * @param delta: the amount to change the color by
 */
void App::update_triangle_color(float delta) {
    triangle_color.r += delta;
    triangle_color.g += delta;
    triangle_color.b += delta;

    // clamp the color values to [0, 1]
    triangle_color.r = glm::clamp(triangle_color.r, 0.0f, 1.0f);
    triangle_color.g = glm::clamp(triangle_color.g, 0.0f, 1.0f);
    triangle_color.b = glm::clamp(triangle_color.b, 0.0f, 1.0f);
}
