#include "scene.hpp"

#include <SDL3/SDL_log.h>
#include "program.hpp"
#include "cube.hpp"

Scene::Scene() {}

Scene::~Scene() {}

// Shader Sources
#ifdef __EMSCRIPTEN__
const char* vertexShaderSource = R"(#version 300 es
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 ourColor;
    uniform mat4 uMVP;
    void main() {
        gl_Position = uMVP * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(#version 300 es
    precision mediump float; // <-- MANDATORY in WebGL 2 fragment shaders
    in vec3 ourColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(ourColor, 1.0);
    }
)";
#else
const char* vertexShaderSource = R"(#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 ourColor;
    uniform mat4 uMVP;
    void main() {
        gl_Position = uMVP * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(#version 330 core
    in vec3 ourColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(ourColor, 1.0);
    }
)";
#endif

bool Scene::init() {
  // CUBE SETUP
  std::shared_ptr<Program> program = std::make_shared<Program>();
  if (!program->init(vertexShaderSource, fragmentShaderSource)) {
    SDL_Log("Shader Program initialization failed");
    return false;
  }
  std::shared_ptr<Cube> cube = std::make_unique<Cube>(program);
  cube->init();

  entities.push_back(cube);
  return true;
}

void Scene::update(float time) const {
  for (auto cube: entities) {
    cube->update(time);
  }
}
  
void Scene::draw(const glm::mat4& pv) const {
  for (auto cube: entities) {
    cube->draw(pv);
  }
}