#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "App.hpp"
#include "Door.hpp"

void App::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Exit The App
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_V:
                // Vsync on/off
                this_inst->is_vsync_on = !this_inst->is_vsync_on;
                glfwSwapInterval(this_inst->is_vsync_on);
                std::cout << "VSync: " << this_inst->is_vsync_on << "\n";
                break;
            case GLFW_KEY_C:
                // Show/Hide ImGui
                this_inst->show_imgui = !this_inst->show_imgui;
                break;
            case GLFW_KEY_UP:
                break;
            case GLFW_KEY_DOWN:
                break;
			case GLFW_KEY_F:
				this_inst->camera.freeCam = !this_inst->camera.freeCam;
				break;
			case GLFW_KEY_R:
				// interact with objects (open doors, etc.)
				for (auto& model : this_inst->models) {
					// in reach of the player
					float dist = glm::distance(this_inst->camera.Position, model->origin);
					if (dist < model->radius) {
						Door* door = dynamic_cast<Door*>(model.get());
						if (door) {
							door->interact();
						} else if (model->end_level) {
							model->interact();
							std::cout << "End of level reached: " << this_inst->level << std::endl;
							this_inst->load_new_level = true;
						} else {
							std::cout << "Object is not interactable: " << model->name << std::endl;
						}
					}
				}
				break;
            default:
                break;
        }
    }
}

void App::glfw_error_callback(int error, const char* description) {
	std::cerr << "GLFW error: " << description << std::endl;
}

void App::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));

    app->camera.ProcessMouseMovement(xpos - app->cursorLastX, (ypos - app->cursorLastY) * -1.0);
    app->cursorLastX = xpos;
    app->cursorLastY = ypos;
	// print
	std::cout << "xpos: " << xpos << ", ypos: " << ypos << std::endl;
}

void App::glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->fov += 10 * yoffset;  // yoffset is mostly +1 or -1; one degree difference in fov is not visible
    this_inst->fov = glm::clamp(this_inst->fov, 20.0f, 170.0f);  // limit FOV to reasonable values...
    this_inst->update_projection_matrix();
}

void App::glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
}

void App::fbsize_callback(GLFWwindow* window, int width, int height) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->width = width;
    this_inst->height = height;

    glViewport(0, 0, width, height);

    this_inst->update_projection_matrix(); 
};

void App::update_projection_matrix(void) {
    if (height <= 0) {
		height = 1;
	}

    float ratio = static_cast<float>(width) / height;

    projection_matrix = glm::perspective(
        glm::radians(fov),   // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        20000.0f             // Far clipping plane. Keep as little as possible.
    );
}

void App::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: {
			int mode = glfwGetInputMode(window, GLFW_CURSOR);
			if (mode == GLFW_CURSOR_NORMAL) {
				// we are aoutside of applicating, catch the cursor
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				// we are inside our game: shoot, click, etc.
				if (this_inst->player.reduceAmmo()) {
					std::cout << "Bang! Ammo left: " << this_inst->player.ammo << "\n";
					glm::vec3 pos = this_inst->camera.Position;
					glm::vec3 dir = this_inst->camera.Front;
					this_inst->bullets.emplace_back(pos, dir);
				} else {
					std::cout << "Click! (No ammo)\n";
				}
			}
			break;
		}
		case GLFW_MOUSE_BUTTON_RIGHT:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		default:
			break;
		}
	}
}

void GLAPIENTRY App::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		default: return "Unknown";
		}
		}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		default: return "Unknown";
		}
		}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		default: return "Unknown";
		}
		}();

	std::cout << "[GL CALLBACK]: " <<
		"source = " << src_str <<
		", type = " << type_str <<
		", severity = " << severity_str <<
		", ID = '" << id << '\'' <<
		", message = '" << message << '\'' << std::endl;
}
