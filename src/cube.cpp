#include "cube.hpp"

Cube::Cube() : VAO(0), VBO(0), EBO(0) {}

Cube::~Cube() {
  destroy();
}

void Cube::init() {
  // Cube Vertices: Position (X,Y,Z) and Color (R,G,B)
  float vertices[] = {
      // Front face          // Colors
      -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,   // BL
      0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,    // BR
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,     // TR
      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,    // TL
                                              // Back face
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,  // BL
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,   // BR
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f,    // TR
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f    // TL
  };

  unsigned int indices[] = {
      0, 1, 2, 2, 3, 0,  // Front
      1, 5, 6, 6, 2, 1,  // Right
      7, 6, 5, 5, 4, 7,  // Back
      4, 0, 3, 3, 7, 4,  // Left
      4, 5, 1, 1, 0, 4,  // Bottom
      3, 2, 6, 6, 7, 3   // Top
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Position Attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // Color Attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Cube::draw() const {
  if (VAO != 0) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  }
}

void Cube::destroy() {
  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }
  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }
  if (EBO != 0) {
    glDeleteBuffers(1, &EBO);
    EBO = 0;
  }
}
