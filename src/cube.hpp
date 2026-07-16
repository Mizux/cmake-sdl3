#ifndef CUBE_HPP
#define CUBE_HPP

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#endif

class Cube {
public:
  Cube();
  ~Cube();

  void init();
  void draw() const;
  void destroy();

private:
  GLuint VAO = 0;
  GLuint VBO = 0;
  GLuint EBO = 0;
};

#endif // CUBE_HPP
