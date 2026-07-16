#include "gl_renderer.hpp"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

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

glRenderer::glRenderer() : shaderProgram(0), mvpLoc(-1) {}

glRenderer::~glRenderer() {
  destroy();
}

bool glRenderer::init() {
  GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);
  
  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cerr << "SHADER PROGRAM LINKING ERROR:\n" << infoLog << std::endl;
    glDeleteShader(vs);
    glDeleteShader(fs);
    return false;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
  return true;
}

void glRenderer::use() const {
  if (shaderProgram != 0) {
    glUseProgram(shaderProgram);
  }
}

void glRenderer::setMVP(const glm::mat4& mvp) const {
  if (mvpLoc != -1) {
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
  }
}

void glRenderer::destroy() {
  if (shaderProgram != 0) {
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;
  }
}

GLuint glRenderer::compileShader(GLenum type, const char* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "SHADER COMPILATION ERROR ("
              << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "):\n"
              << infoLog << std::endl;
  }
  return shader;
}
