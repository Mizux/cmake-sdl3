#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <numbers>

#define SDL_MAIN_USE_CALLBACKS 1  // Tell SDL to use the callback architecture
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>  // Emscripten provides WebGL2 symbols natively here
#include <emscripten.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#endif

#include "gl_renderer.hpp"
#include "cube.hpp"

// Main code
// A struct to hold your application state (replaces global variables)
struct AppState {
  SDL_Window* window = nullptr;
  SDL_GLContext gl_context = nullptr;

  std::unique_ptr<glRenderer> renderer;
  std::unique_ptr<Cube> cube;
};

// 1. Called once when the app starts. Initialize everything here.
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  // Allocate our custom state structure
  AppState* state = new AppState();
  *appstate = state;

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    SDL_Log("SDL Initialization failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

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

  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                 SDL_WINDOW_HIDDEN |
                                 SDL_WINDOW_HIGH_PIXEL_DENSITY;

  state->window =
      SDL_CreateWindow("SDL3 Cross-Platform App", (int)(1280 * main_scale),
                       (int)(800 * main_scale), window_flags);
  if (state->window == nullptr) {
    SDL_Log("Window creation failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  state->gl_context = SDL_GL_CreateContext(state->window);
  if (state->gl_context == nullptr) {
    SDL_Log("OpenGL Context creation failed: %s", SDL_GetError());
    SDL_DestroyWindow(state->window);
    return SDL_APP_FAILURE;
  }

  // Enable Depth Testing for 3D
  glEnable(GL_DEPTH_TEST);

  // SHADER SETUP START
  state->renderer = std::make_unique<glRenderer>();
  if (!state->renderer->init()) {
    SDL_Log("Renderer initialization failed");
    return SDL_APP_FAILURE;
  }

  // CUBE SETUP
  state->cube = std::make_unique<Cube>();
  state->cube->init();
  // SHADER SETUP END

  SDL_GL_MakeCurrent(state->window, state->gl_context);
  SDL_GL_SetSwapInterval(1);  // Enable vsync
  SDL_SetWindowPosition(state->window, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(state->window);

  return SDL_APP_CONTINUE;  // Tells SDL to keep running
}

// 2. Called whenever the system fires an event (input, window resize, etc.)
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  AppState* state = static_cast<AppState*>(appstate);

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  // Safely exits the application
  }
  if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
      event->window.windowID == SDL_GetWindowID(state->window)) {
    return SDL_APP_SUCCESS;  // Safely exits the application
  }
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
      return SDL_APP_SUCCESS;
    }
  }
  return SDL_APP_CONTINUE;
}

// 3. Called every single frame. This is your update and render loop.
SDL_AppResult SDL_AppIterate(void* appstate) {
  AppState* state = static_cast<AppState*>(appstate);

  if (SDL_GetWindowFlags(state->window) & SDL_WINDOW_MINIMIZED) {
    SDL_Delay(10);
    return SDL_APP_CONTINUE;  // Keep looping
  }

  // --- 1. Update Game/App Logic Here ---
  float time = SDL_GetTicks() / 1000.0f;

  // Build MVP Matrix using GLM
  glm::mat4 model =
      glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));

  glm::mat4 view =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));

  glm::mat4 projection =
      glm::perspective(45.0f * (static_cast<float>(std::numbers::pi) / 180.0f),
                       1280.0f / 800.0f, 0.1f, 100.0f);

  glm::mat4 mvp = projection * view * model;

  // Pass matrix to shader
  state->renderer->use();
  state->renderer->setMVP(mvp);

  // --- 2. Render Graphics Here ---
  glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw Cube
  state->cube->draw();

  SDL_GL_SwapWindow(state->window);

  return SDL_APP_CONTINUE;  // Keep looping
}

// 4. Called once automatically right before the program terminates
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  if (appstate) {
    AppState* state = static_cast<AppState*>(appstate);

    // Destroy cube and renderer while the OpenGL context is still valid
    state->cube.reset();
    state->renderer.reset();

    SDL_GL_DestroyContext(state->gl_context);
    SDL_DestroyWindow(state->window);
    delete state;  // Clean up our heap memory
  }
  SDL_Quit();
  SDL_Log("Application shut down clean with code: %d", result);
}
