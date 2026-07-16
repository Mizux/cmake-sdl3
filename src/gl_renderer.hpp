#ifndef GL_RENDERER_HPP
#define GL_RENDERER_HPP

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#endif

#include <glm/glm.hpp>

class glRenderer {
public:
  glRenderer();
  ~glRenderer();

  bool init();
  void use() const;
  void setMVP(const glm::mat4& mvp) const;
  void destroy();

private:
  GLuint shaderProgram = 0;
  GLint mvpLoc = -1;

  static GLuint compileShader(GLenum type, const char* source);
};

#endif // GL_RENDERER_HPP
