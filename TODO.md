
- [ ] Shader program hpp - přidat inline k metodám setUniform
- [x] Opravit pořadí vykreslování průhledných modelů.

```cpp
// remember:
// void draw(glm::vec3 const & offset = glm::vec3(0.0), glm::vec3 const & rotation = glm::vec3(0.0f))
//#######              DRAW             ###############
// option 1: draw object, no changes in position, rotation, scale, etc.
    scene.at("model_of_something").draw();
//#####################################################
// option 2: pass new position, that is changing in every frame (saving is useless)
    scene.at("model_of_something").draw(glm::vec3(0.0f),
                glm::vec3(0.0f, glm::radians(static_cast<float>(360*glfwGetTime())), 0.0f) );
//#####################################################
// option 2: compute and save new position, simple transformation
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scene.at("model_of_something").rotation.y = glm::radians(static_cast<float>(360*glfwGetTime()));
    }
    scene.at("model_of_something").draw();
//#####################################################
// option 3: - for complex transformations: combine multiple transformations to model matrix  
// 			 - prepare model transformation locally, stack it with internal position
//           - see Model-extended.cpp for draw(glm::mat4 const & model_matrix)
    glm::mat4 m_m = glm::identity<glm::mat4>();
    m_m = glm::rotate(m_m, glm::radians(static_cast<float>(360*glfwGetTime())), glm::vec3(0.0f, 0.1f, 0.0f));
    m_m = glm::scale(m_m, glm::vec3(1.0f + glm::sin(glfwGetTime())));
    scene.at("model_of_something").draw(m_m);
//#####################################################
// option 4 (for dymamic object): derive new class - rotatingModel, modify update() method, so that
//                                it modifies member variable rotation itself
    scene.at("model_of_something").update(delta_t);
    scene.at("model_of_something").draw();
```