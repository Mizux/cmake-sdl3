#ifndef GL_RENDERER_HPP
#define GL_RENDERER_HPP

#include <SDL3/SDL_video.h>
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#endif

#include <glm/glm.hpp>

class Camera;
class Scene;
class glRenderer {
public:
  glRenderer(SDL_Window* window);
  ~glRenderer();

  bool init();
  void render(Camera* camera, Scene* scene);
  void destroy();
private:
  void beginFrame();
  void endFrame();

  SDL_Window* window;
  SDL_GLContext gl_context;
};

#endif // GL_RENDERER_HPP
