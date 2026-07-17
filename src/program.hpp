#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <glm/glm.hpp>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#endif

class Program {
public:
  Program();
  ~Program();

  bool init(const char* vertexSource, const char* fragmentSource);
  void use() const;
  void setUniformMatrix4fv(const char* name, const glm::mat4& matrix) const;
  void destroy();

  GLuint getID() const { return programID; }

private:
  GLuint programID = 0;
  static GLuint compileShader(GLenum type, const char* source);
};

#endif // PROGRAM_HPP
