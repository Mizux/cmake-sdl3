#ifndef SCENE_HPP
#define SCENE_HPP

#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Cube;

class Scene {
 public:
  Scene();
  ~Scene();

  bool init();
  void update(float time) const;
  void draw(const glm::mat4& pv) const;

  private:
   std::vector<std::shared_ptr<Cube>> entities;
};

#endif // SCENE_HPP