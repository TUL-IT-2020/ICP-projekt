#pragma once

#include <vector>
#include <GL/glew.h> 
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Mesh.h"

//vertex description
struct vertex {
    glm::vec3 position;
};

// color description
struct color {
    GLfloat r, g, b, a;
};
