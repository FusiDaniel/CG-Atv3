### README

Daniel Fusimoto Pires - RA: 11201921874
Guilherme Cesario Scagnolato - RA: 11201812319

### ABCg
![https://github.com/hbatagelo/abcg/actions/workflows/linux.yml/badge.svg](https://github.com/hbatagelo/abcg/actions/workflows/linux.yml/badge.svg)
![https://github.com/hbatagelo/abcg/actions/workflows/macos.yml/badge.svg](https://github.com/hbatagelo/abcg/actions/workflows/macos.yml/badge.svg)
![https://github.com/hbatagelo/abcg/actions/workflows/windows.yml/badge.svg](https://github.com/hbatagelo/abcg/actions/workflows/windows.yml/badge.svg)
![https://github.com/hbatagelo/abcg/actions/workflows/wasm.yml/badge.svg](https://github.com/hbatagelo/abcg/actions/workflows/wasm.yml/badge.svg)
[https://img.shields.io/github/v/release/hbatagelo/abcg](https://img.shields.io/github/v/release/hbatagelo/abcg)

Development framework accompanying the course [MCTA008-17 Computer Graphics](http://professor.ufabc.edu.br/~harlen.batagelo/cg/) at [UFABC](https://www.ufabc.edu.br/).

[Documentation](https://hbatagelo.github.io/abcg/abcg/doc/html/) | [Release notes](CHANGELOG.md)

ABCg is a lightweight C++ framework that simplifies the development of 3D graphics applications based on [OpenGL](https://www.opengl.org/), [OpenGL ES](https://www.khronos.org/), [WebGL](https://www.khronos.org/webgl/), and [Vulkan](https://www.vulkan.org/). It is designed for the tutorials and assignments of the course “MCTA008-17 Computer Graphics” taught at Federal University of ABC (UFABC).

---

# Atividade 03 - ExplodingRubikCube

![Untitled](README%204992741d5a10417faf175012cad0c4f3/Untitled.png)

GitHub Pages: [ExplodingRubikCube](https://fusidaniel.github.io/CG-Atv3/public/helloworld.html)

## Descrição geral

- Foi desenvolvido um projeto com o objetivo de produzir uma aplicação 3D, e aplicar transformações geométricas a com o uso da lib ABCg.
- Portanto desenvolvemos um cubo mágico, que explode separando as peças e, e então "embaralha" cada um dos cubos e monta o cubo mágico novamente.

## Implementação

### `normal.vert / normal.frag`

- O fragmento de código abaixo foi usado para gerar a coloração das faces do cubo.

```cpp
void main() {
  mat4 MVP = projMatrix * viewMatrix * modelMatrix;
  gl_Position = MVP * vec4(inPosition, 1.0);
  vec3 N = inNormal;  // Object space

  vec3 rubikColor = vec3(0.5, 0.5, 0.5);
  if (N.y > 0.0 && N.x == 0.0 && N.z == 0.0) {
    // white top
    rubikColor = vec3(1.0, 1.0, 1.0);
  }  else if (N.y < 0.0 && N.x == 0.0 && N.z == 0.0) {
    // yellow bottom
    rubikColor = vec3(1.0, 1.0, 0.0);
  }  else if (N.x > 0.0 && N.y == 0.0 && N.z == 0.0) { 
    // green side
    rubikColor = vec3(0.0, 0.5, 0.0);
  }  else if (N.x < 0.0 && N.y == 0.0 && N.z == 0.0) {
    // blue side
    rubikColor = vec3(0.0, 0.0, 1.0);
  }  else if (N.z > 0.0 && N.x == 0.0 && N.y == 0.0) {
    // orange side
    rubikColor = vec3(1.0, 0.666, 0.0);
  }  else if (N.z < 0.0 && N.x == 0.0 && N.y == 0.0) {
    // red side
    rubikColor = vec3(1.0, 0.0, 0.0);
  }
  fragColor = vec4(rubikColor, 1.0);
}
```

```cpp
#version 300 es

precision mediump float;

in vec4 fragColor;
out vec4 outColor;

void main() { outColor = fragColor; }
```

### `window.hpp`

- Para a definição da classe Window, foram sobrescritos seis métodos da classe OpenGLWindow.

```cpp
  void onEvent(SDL_Event const &event) override; // Gerencia enventos na tela
  void onCreate() override;  // Cria a aplicação
  void onUpdate() override;  // Atualiza a tela a
  void onPaint() override;   // Renderiza as imagens
  void onPaintUI() override; // UI
  void onResize(glm::ivec2 const &size) override; // Redimensionamento de tela
  void onDestroy() override; // Destruição da tela
```

- Além disso, foram definidas as seguintes variáveis e métodos auxiliares.

```cpp
private:
  std::default_random_engine m_randomEngine;
  glm::ivec2 m_viewportSize{};
  Model m_model;
  
  struct Cube {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };
  std::array<Cube, 27> m_cubes;

  float m_angle{0.0};
  bool rot_reverse{false};
  bool rot_pause{false};
  bool m_animation{true};

  int m_trianglesToDraw{};
  float m_distance{0.14366};

  TrackBall m_trackBall;
  float m_zoom{};

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  GLuint m_program{};

  void randomizeCube(Cube &cube);
};
```

### `**window.cpp**`

- Neste arquivo foram implementados todos os métodos sobrescritos no arquivo window.hpp.
- `onEvent()`
    - Função utilizada para movimentar a camera em torno do cubo magico.
    
    ```cpp
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
    ```
    
- `onCreate()`
    - No método **OnCreate** são realizadas as operações, como inicialização de variáveis, carregamento dos cubos, rotação dos cubos.
        
        ```cpp
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
        ```
        
- `onUpdate()`
    - No método onUpdate o cubo é “explodido”, e então, após a explosão os cubos que formam o cubo magico rotacionam de forma aleatória, após a rotação eles são agrupados novamente (fragmento de codigo abaixo)
        
        ```cpp
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
        ...
        } else {
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
        ...
        ```
        
- `onPaint()`
    - No método **onPaint** são desenhados os objetos da aplicação.
- `onPaintUI()`
    - No método **onPaintUI** são desenhados os elementos de UI da tela.
- `onResize()`
    - No método **onResize** é onde a tela sofre resize.
- `onDestroy()`
    - No método **onDestroy** é onde a a tela é finalizada

## Demonstração (clique no GIF)

![Animação.gif](README%204992741d5a10417faf175012cad0c4f3/Animao.gif)

---

## License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.