#ifndef CUBE_HPP
#define CUBE_HPP

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#endif

#include <glm/glm.hpp>

class glRenderer;

class Cube {
public:
  Cube();
  ~Cube();

  void init();
  void update(float time);
  void draw(const glRenderer& renderer, const glm::mat4& pv) const;
  void destroy();

private:
  GLuint VAO = 0;
  GLuint VBO = 0;
  GLuint EBO = 0;
  glm::mat4 model = glm::mat4(1.0f);
};

#endif // CUBE_HPP
