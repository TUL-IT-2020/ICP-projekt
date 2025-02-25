// icp.cpp
// author: JK

// our awesome headers
#include "App.hpp"

std::filesystem::path vertex_shader_path = "resources/shaders/basic.vert";
std::filesystem::path fragment_shader_path = "resources/shaders/basic.frag";

// define our application
App app;

// MAIN program function
int main() {
    app.fragment_shader_path = fragment_shader_path;
    app.vertex_shader_path = vertex_shader_path;

    if (app.init()) {
        return app.run();
    } else {
        return EXIT_FAILURE;
    }
}

/* */