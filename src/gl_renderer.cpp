#include "gl_renderer.hpp"

#include "camera.hpp"
#include "scene.hpp"
#include <SDL3/SDL_log.h>

glRenderer::glRenderer(SDL_Window* window) :
 window(window) {
#ifdef __EMSCRIPTEN__
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
  // GL 3.0 + GLSL 130
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

glRenderer::~glRenderer() {
  destroy();
}

bool glRenderer::init() {
  gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    SDL_Log("OpenGL Context creation failed: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    return false;
  }
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1);  // Enable vsync

  // Enable Depth Testing for 3D
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  return true;
}
  
void glRenderer::render(Camera* camera, Scene* scene) {
  beginFrame();

  // Get ViewProjection Matrix from Camera
  glm::mat4 pv = camera->getViewProjectionMatrix();

  // Draw Cube
  scene->draw(pv);

  endFrame();
}
  
void glRenderer::beginFrame() {
  glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void glRenderer::endFrame() { SDL_GL_SwapWindow(window); }

void glRenderer::destroy() {
    SDL_GL_DestroyContext(gl_context);
}
