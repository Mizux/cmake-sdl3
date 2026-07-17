#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

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
#include "camera.hpp"
#include "scene.hpp"

// Main code
// A struct to hold your application state (replaces global variables)
struct AppState {
  SDL_Window* window = nullptr;

  std::unique_ptr<glRenderer> renderer;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<Scene> scene;
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
  SDL_SetWindowPosition(state->window, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);

  state->renderer = std::make_unique<glRenderer>(state->window);
  if (!state->renderer->init()) {
    SDL_Log("Renderer initialization failed");
    return SDL_APP_FAILURE;
  }

  // CAMERA SETUP
  int w, h;
  SDL_GetWindowSize(state->window, &w, &h);
  state->camera = std::make_unique<Camera>(
      45.0f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 100.0f);

  // SCENE SETUP
  state->scene = std::make_unique<Scene>();
  if (!state->scene->init()) {
    return SDL_APP_FAILURE;
  }

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
  if (event->type == SDL_EVENT_WINDOW_RESIZED &&
      event->window.windowID == SDL_GetWindowID(state->window)) {
    if (state->camera) {
      state->camera->setAspectRatio(static_cast<float>(event->window.data1) /
                                    static_cast<float>(event->window.data2));
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

  // Update Cube
  state->scene->update(time);

  // --- 2. Render Graphics Here ---
  state->renderer->render(state->camera.get(), state->scene.get());
  return SDL_APP_CONTINUE;  // Keep looping
}

// 4. Called once automatically right before the program terminates
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  if (appstate) {
    AppState* state = static_cast<AppState*>(appstate);

    // Destroy cube, camera, program, and renderer while the OpenGL context is still valid
    state->scene.reset();
    state->camera.reset();
    state->renderer.reset();
    SDL_DestroyWindow(state->window);
    delete state;  // Clean up our heap memory
  }
  SDL_Quit();
  SDL_Log("Application shut down clean with code: %d", result);
}
