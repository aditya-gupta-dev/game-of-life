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

struct Cell {
    int x;
    int y;
    Color active_color;
    const usize size;
    bool is_active;

    public:
    Cell(int x_pos, int y_pos, usize cell_size) : x(x_pos), y(y_pos), active_color(RAYWHITE), size(cell_size), is_active(false) {};
    void DrawCell() const {
        DrawRectangleLines(this->x, this->y, this->size, this->size, this->active_color);
    }
};

class Window {
    private:
        const Config &m_config;
        std::vector<Cell> cells;
   public:
      Window(const Config &config) : m_config(config){};

      void Init() {
        InitWindow(m_config.w_width, m_config.w_height, m_config.w_title.c_str());
        SetTargetFPS(m_config.fps);

        const usize cols = m_config.w_width / m_config.cell_size;
        const usize rows = m_config.w_height / m_config.cell_size;


        for (usize row = 0; row < rows; ++row) {
            for (usize col = 0; col < cols; ++col) {
                cells.emplace_back(
                    static_cast<int>(col * m_config.cell_size),
                    static_cast<int>(row * m_config.cell_size),
                    m_config.cell_size
                );
            }
        }
    }

      void Run() {
          int i = 0;
         while (!WindowShouldClose()) {
              BeginDrawing();
              ClearBackground(BLACK);
              if (this->m_config.is_debug){
                  DrawFPS(30, 30);
              }
              for (const Cell &cell: this->cells) {
                  cell.DrawCell();
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
    window.Init();
    window.Run();
}
