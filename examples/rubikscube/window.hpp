#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"
#include "model.hpp"
#include "trackball.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  std::default_random_engine m_randomEngine;
  glm::ivec2 m_viewportSize{};
  Model m_model;
  
  struct Cube {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };
  std::array<Cube, 2> m_cubes;

  int m_trianglesToDraw{};

  TrackBall m_trackBall;
  float m_zoom{};

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  GLuint m_program{};

  void randomizeCube(Cube &cube);
};

#endif