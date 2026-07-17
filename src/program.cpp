#include "program.hpp"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Program::Program() : programID(0) {}

Program::~Program() { destroy(); }

bool Program::init(const char* vertexSource, const char* fragmentSource) {
  GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSource);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

  programID = glCreateProgram();
  glAttachShader(programID, vs);
  glAttachShader(programID, fs);
  glLinkProgram(programID);

  GLint success;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(programID, 512, NULL, infoLog);
    std::cerr << "SHADER PROGRAM LINKING ERROR:\n" << infoLog << std::endl;
    glDeleteShader(vs);
    glDeleteShader(fs);
    return false;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  return true;
}

void Program::use() const {
  if (programID != 0) {
    glUseProgram(programID);
  }
}

void Program::setUniformMatrix4fv(const char* name,
                                  const glm::mat4& matrix) const {
  if (programID != 0) {
    GLint loc = glGetUniformLocation(programID, name);
    if (loc != -1) {
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
    }
  }
}

void Program::destroy() {
  if (programID != 0) {
    glDeleteProgram(programID);
    programID = 0;
  }
}

GLuint Program::compileShader(GLenum type, const char* source) {
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
