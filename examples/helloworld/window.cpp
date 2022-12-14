#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <algorithm>

void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBall.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mousePress(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mouseRelease(mousePosition);
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);
  abcg::glEnable(GL_CULL_FACE);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "normal.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "normal.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj(assetsPath + "cube.obj");
  m_model.setupVAO(m_program);

  m_trianglesToDraw = m_model.getNumTriangles();

  std::uniform_int_distribution<int> rot_axis(0, 2);
  
  // Setup cubes
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      for (int z = 0; z < 3; z++) {
        m_cubes.at(x*9+y*3+z*1).m_position = glm::vec3((x-1)*m_distance,(y-1)*m_distance,(z-1)*m_distance);
        
        glm::vec3 current_axis{0.0};
        current_axis[rot_axis(m_randomEngine)] = glm::half_pi<float>();
        m_cubes.at(x*9+y*3+z*1).m_rotationAxis = current_axis;
      }
    }
  }
}

void Window::onUpdate() {
  std::uniform_int_distribution<int> rot_axis(0, 2);
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  
  m_modelMatrix = m_trackBall.getRotation();

  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  
  if (!m_animation) {
    return;
  }

  // Cubes animation
  if (!rot_pause) {
    // In and Out translation
    if (!rot_reverse) {
      m_distance = glm::wrapAngle(m_distance + 0.3 * deltaTime);
    }
    else {
      m_distance = glm::wrapAngle(m_distance - 0.3 * deltaTime);
    }

    if (m_distance > 0.4 && !rot_reverse) {
      rot_pause = !rot_pause;
      m_angle += 0.1;
    }
    else if (m_distance < 0.14366 && rot_reverse) {
      rot_reverse = !rot_reverse;
      
    }

    for (int x = 0; x < 3; x++) {
      for (int y = 0; y < 3; y++) {
        for (int z = 0; z < 3; z++) {
          m_cubes.at(x*9+y*3+z*1).m_position = glm::vec3((x-1)*m_distance,(y-1)*m_distance,(z-1)*m_distance);
        }
      }
    }
  }

  else {
    // Per cube rotation
    m_angle = m_angle + glm::half_pi<float>() * deltaTime;
    if (m_angle > glm::half_pi<float>()*4 && m_angle < glm::half_pi<float>()*4 + 0.1) {
      m_angle = 0.0;
      rot_pause = !rot_pause;
      rot_reverse = !rot_reverse;

      for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
          for (int z = 0; z < 3; z++) {

            glm::vec3 current_axis{0.0};
            current_axis[rot_axis(m_randomEngine)] = glm::half_pi<float>();
            m_cubes.at(x*9+y*3+z*1).m_rotationAxis = current_axis;
          }
        }
      }
    } 
    else if (m_angle > glm::half_pi<float>()*3 && m_angle < glm::half_pi<float>()*3 + 0.1) {
      m_angle = glm::half_pi<float>()*3;
      rot_pause = !rot_pause;
      rot_reverse = !rot_reverse;
    }
    else if (m_angle > glm::half_pi<float>()*2 && m_angle < glm::half_pi<float>()*2 + 0.1) {
      m_angle = glm::half_pi<float>()*2;
      rot_pause = !rot_pause;
      rot_reverse = !rot_reverse;

    }
    else if (m_angle > glm::half_pi<float>() && m_angle < glm::half_pi<float>() + 0.1) {
      m_angle = glm::half_pi<float>();
      rot_pause = !rot_pause;
      rot_reverse = !rot_reverse;

    }
  }
  
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const normalMatrixLoc{abcg::glGetUniformLocation(m_program, "normalMatrix")};

 // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);

  for (auto &cube : m_cubes) {
    // Compute model matrix of the current cube
    glm::mat4 modelMatrix = m_modelMatrix;
    modelMatrix = glm::translate(modelMatrix, cube.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.125f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, cube.m_rotationAxis);

    // Set uniform variables for the current model
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
    
    auto const modelViewMatrix{glm::mat3(m_viewMatrix * modelMatrix)};
    auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
    abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

    m_model.render(m_trianglesToDraw);
  }

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  // Create a window for widgets
  {
    auto const widgetSize{ImVec2(222, 120)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    // CW/CCW combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> const comboItems{"CCW", "CW"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Front face",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      if (currentIndex == 0) {
        abcg::glFrontFace(GL_CCW);
      } else {
        abcg::glFrontFace(GL_CW);
      }
    }

    // Projection combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      if (currentIndex == 0) {
        auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                          gsl::narrow<float>(m_viewportSize.y)};
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
      } else {
        m_projMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }

    ImGui::Checkbox("Play/Pause Animation", &m_animation);
    ImGui::Text("%f cubes distante", m_distance);
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_trackBall.resizeViewport(size);
}

void Window::onDestroy() {
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}