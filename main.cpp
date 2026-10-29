#include <vector>
#include <iostream>
#include <raylib.h>

using usize = std::size_t;

struct Config {
    bool is_resizable = false;
    bool is_debug = false;
    usize fps = 30;
    usize w_width = 1280;
    usize w_height = 720;
    usize cell_size = 20;
    const std::string w_title = "[convey] - game of life";

    Config() = default;
};

class Window {
    private:
        const Config &m_config;
        const std::vector<int> active_cells;
   public:
      Window(const Config &config) : m_config(config){};

      void Run() {
        InitWindow(m_config.w_width, m_config.w_height, m_config.w_title.c_str());
        SetTargetFPS(m_config.fps);
         while (!WindowShouldClose()) {
              BeginDrawing();
              ClearBackground(BLACK);
              if (this->m_config.is_debug){
                  DrawFPS(30, 30);
              }
              // drawing Cells

              for (int y = 0; y < this->m_config.w_height; y+=this->m_config.cell_size) {
                  for (int x = 0; x < this->m_config.w_width; x+=this->m_config.cell_size) {
                      DrawRectangleLines(x,y, this->m_config.cell_size, this->m_config.cell_size, RAYWHITE);
                  }
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
