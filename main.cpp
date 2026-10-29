#include <iostream>
#include <raylib.h>

using usize = std::size_t;

struct Config {
    bool isDebug = true;
    usize w_width = 1280;
    usize w_height = 720;
    usize fps = 30;
    std::string w_title = "[convey] - game of life";

    Config() = default;
};

class Window {
    private:
        const Config &m_config;
   public:
      Window(const Config &config) : m_config(config){};

      void Run() {
        InitWindow(m_config.w_width, m_config.w_height, m_config.w_title.c_str());
        SetTargetFPS(m_config.fps);

          while (!WindowShouldClose()) {
              BeginDrawing();
              ClearBackground(BLACK);
              if (this->m_config.fps){
                  DrawFPS(30, 30);
              }
              EndDrawing();
          }
      }
      ~Window() {
          CloseWindow();
      }
};

int main() {
    const Config config;
    Window window(config);

    window.Run();
}
