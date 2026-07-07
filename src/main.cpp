#include <cmath>
#include <iostream>

#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h> // Emscripten provides WebGL2 symbols natively here
#include <functional>
static std::function<void()>            MainLoopForEmscriptenP;
static void MainLoopForEmscripten()     { MainLoopForEmscriptenP(); }
#define EMSCRIPTEN_MAINLOOP_BEGIN       MainLoopForEmscriptenP = [&]() { do
#define EMSCRIPTEN_MAINLOOP_END         while (0); }; emscripten_set_main_loop(MainLoopForEmscripten, 0, true);
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#define EMSCRIPTEN_MAINLOOP_BEGIN       while (running)
#define EMSCRIPTEN_MAINLOOP_END
#endif

// Simple matrix math helpers (to avoid external dependencies like GLM for this example)
void multiply_matrix(const float* a, const float* b, float* out) {
  float res[16];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      res[i * 4 + j] = 0;
      for (int k = 0; k < 4; ++k) {
        res[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
      }
    }
  }
  for (int i = 0; i < 16; ++i) out[i] = res[i];
}

void get_rotation_y(float angle, float* m) {
  float c = cosf(angle), s = sinf(angle);
  float r[16] = {
    c, 0, s, 0,
    0, 1, 0, 0,
    -s, 0, c, 0,
    0, 0, 0, 1
  };
  for(int i=0; i<16; ++i) m[i] = r[i];
}

void get_projection(float fov, float aspect, float nearZ, float farZ, float* m) {
  float f = 1.0f / tanf(fov / 2.0f);
  float r[16] = {
    f / aspect, 0, 0, 0,
    0, f, 0, 0,
    0, 0, (farZ + nearZ) / (nearZ - farZ), (2.0f * farZ * nearZ) / (nearZ - farZ),
    0, 0, -1, 0
  };
  for(int i=0; i<16; ++i) m[i] = r[i];
}

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

// Helper to compile and check shaders
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "SHADER COMPILATION ERROR ("
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << "):\n" << infoLog << std::endl;
    }
    return shader;
}

// Main code
int main(int, char**)
{
  // Setup SDL
  // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
  {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

#ifdef __EMSCRIPTEN__
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  const char* glsl_version = "#version 300 es";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
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
  SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window* window = SDL_CreateWindow("SDL3+OpenGL3 example", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
  if (window == nullptr)
  {
    std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr)
  {
    std::cerr << "OpenGL Context creation failed: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Enable Depth Testing for 3D
  glEnable(GL_DEPTH_TEST);

  // SHADER SETUP START
  // Compile and Link Shaders
  GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);

  glLinkProgram(shaderProgram);
  {
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cerr << "SHADER PROGRAM LINKING ERROR:\n" << infoLog << std::endl;
    }
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  // Cube Vertices: Position (X,Y,Z) and Color (R,G,B)
  float vertices[] = {
    // Front face          // Colors
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,
    // Back face
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 0.0f
  };

  unsigned int indices[] = {
    0, 1, 2,  2, 3, 0, // Front
    1, 5, 6,  6, 2, 1, // Right
    7, 6, 5,  5, 4, 7, // Back
    4, 0, 3,  3, 7, 4, // Left
    4, 5, 1,  1, 0, 4, // Bottom
    3, 2, 6,  6, 7, 3  // Top
  };

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Position Attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // Color Attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
  // SHADER SETUP END

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

  // Main loop
  bool running = true;
  EMSCRIPTEN_MAINLOOP_BEGIN
    {
      // Poll and handle events (inputs, window resize, etc.)
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
          running = false;
      }

      // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
      if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
      {
        SDL_Delay(10);
        continue;
      }

      // Rendering
      glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(shaderProgram);

      // Build MVP Matrix
      float time = SDL_GetTicks() / 1000.0f;

      float model[16];
      get_rotation_y(time, model); // Rotate around Y axis over time

      float view[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -2.5f, // Move back 2.5 units on Z
        0, 0, 0, 1
      };

      float projection[16];
      get_projection(45.0f * (M_PI / 180.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

      float viewProj[16];
      multiply_matrix(projection, view, viewProj);
      float mvp[16];
      multiply_matrix(viewProj, model, mvp);

      // Pass matrix to shader
      glUniformMatrix4fv(mvpLoc, 1, GL_TRUE, mvp);

      // Draw Cube
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

      SDL_GL_SwapWindow(window);
    }
  EMSCRIPTEN_MAINLOOP_END

  // Cleanup
  // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  SDL_GL_DestroyContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
