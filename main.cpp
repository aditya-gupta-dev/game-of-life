#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <raylib.h>

using usize = std::size_t;

struct Config {
    bool is_resizable = false;
    bool is_debug = true;
    usize fps = 60;
    usize w_width = 1280;
    usize w_height = 720;
    usize cell_size = 20;
    double update_interval = 0.1; // Seconds per simulation step (10 updates/sec)
    const std::string w_title = "[Conway] - Game of Life";
    const std::string active_cells_file_name = "active_cells";

    Config() = default;
};

struct Cell {
    int x;
    int y;
    usize size;
    Color active_color;
    Color grid_color;
    bool is_active;

    Cell(int x_pos, int y_pos, usize cell_size, bool active = false)
        : x(x_pos),
          y(y_pos),
          size(cell_size),
          active_color(RAYWHITE),
          grid_color(DARKGRAY),
          is_active(active) {}

    void DrawCell() const {
        if (is_active) {
            DrawRectangle(x, y, static_cast<int>(size), static_cast<int>(size), active_color);
        } else {
            DrawRectangleLines(x, y, static_cast<int>(size), static_cast<int>(size), grid_color);
        }
    }
};

class Window {
private:
    const Config &m_config;
    usize m_cols = 0;
    usize m_rows = 0;
    std::vector<Cell> m_cells;

    bool m_is_paused = true;
    double m_last_update_time = 0.0;

    std::vector<usize> GetActiveCellsFromFile() {
        std::ifstream input_stream(m_config.active_cells_file_name);
        std::vector<usize> active_cells;
        if (!input_stream.is_open()) {
            return active_cells;
        }

        std::string line;
        while (std::getline(input_stream, line)) {
            if (line.empty()) continue;
            try {
                active_cells.push_back(static_cast<usize>(std::stoul(line)));
            } catch (...) {
                // Skip invalid lines
            }
        }
        return active_cells;
    }

    // Counts live neighbors with toroidal (wrap-around) boundaries
    int CountNeighbors(int col, int row) const {
        int live_neighbors = 0;
        int i_cols = static_cast<int>(m_cols);
        int i_rows = static_cast<int>(m_rows);

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;

                int neighbor_col = (col + dx + i_cols) % i_cols;
                int neighbor_row = (row + dy + i_rows) % i_rows;
                usize index = static_cast<usize>(neighbor_row * i_cols + neighbor_col);

                if (m_cells[index].is_active) {
                    live_neighbors++;
                }
            }
        }
        return live_neighbors;
    }

    void UpdateSimulation() {
        std::vector<bool> next_states(m_cells.size(), false);

        for (usize row = 0; row < m_rows; ++row) {
            for (usize col = 0; col < m_cols; ++col) {
                usize index = row * m_cols + col;
                int neighbors = CountNeighbors(static_cast<int>(col), static_cast<int>(row));

                if (m_cells[index].is_active) {
                    // Rule 1 & 2: Any live cell with 2 or 3 live neighbors survives
                    next_states[index] = (neighbors == 2 || neighbors == 3);
                } else {
                    // Rule 4: Any dead cell with exactly 3 live neighbors becomes active
                    next_states[index] = (neighbors == 3);
                }
            }
        }

        // Apply new generation
        for (usize i = 0; i < m_cells.size(); ++i) {
            m_cells[i].is_active = next_states[i];
        }
    }

    void HandleInput() {
        // Spacebar: Pause/Resume
        if (IsKeyPressed(KEY_SPACE)) {
            m_is_paused = !m_is_paused;
        }

        // 'N' or Right Arrow: Step one frame while paused
        if (m_is_paused && (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_RIGHT))) {
            UpdateSimulation();
        }

        // 'C': Clear grid
        if (IsKeyPressed(KEY_C)) {
            for (auto &cell : m_cells) {
                cell.is_active = false;
            }
        }

        // 'R': Randomize grid
        if (IsKeyPressed(KEY_R)) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::bernoulli_distribution dist(0.20); // 20% alive probability
            for (auto &cell : m_cells) {
                cell.is_active = dist(gen);
            }
        }

        // Mouse Painting (Left Click: Draw, Right Click: Erase)
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_pos = GetMousePosition();
            int col = static_cast<int>(mouse_pos.x) / static_cast<int>(m_config.cell_size);
            int row = static_cast<int>(mouse_pos.y) / static_cast<int>(m_config.cell_size);

            if (col >= 0 && col < static_cast<int>(m_cols) &&
                row >= 0 && row < static_cast<int>(m_rows)) {
                usize index = static_cast<usize>(row * static_cast<int>(m_cols) + col);
                m_cells[index].is_active = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
            }
        }
    }

public:
    Window(const Config &config) : m_config(config) {}

    void Init() {
        if (m_config.is_resizable) {
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        }

        InitWindow(
            static_cast<int>(m_config.w_width),
            static_cast<int>(m_config.w_height),
            m_config.w_title.c_str()
        );
        SetTargetFPS(static_cast<int>(m_config.fps));

        m_cols = m_config.w_width / m_config.cell_size;
        m_rows = m_config.w_height / m_config.cell_size;
        m_cells.reserve(m_cols * m_rows);

        for (usize row = 0; row < m_rows; ++row) {
            for (usize col = 0; col < m_cols; ++col) {
                m_cells.emplace_back(
                    static_cast<int>(col * m_config.cell_size),
                    static_cast<int>(row * m_config.cell_size),
                    m_config.cell_size
                );
            }
        }

        // Load predefined active cells
        for (const usize active_cell : GetActiveCellsFromFile()) {
            if (active_cell < m_cells.size()) {
                m_cells[active_cell].is_active = true;
            }
        }
    }

    void Run() {
        while (!WindowShouldClose()) {
            HandleInput();

            // Simulation step based on elapsed time (independent of render FPS)
            if (!m_is_paused) {
                double current_time = GetTime();
                if (current_time - m_last_update_time >= m_config.update_interval) {
                    UpdateSimulation();
                    m_last_update_time = current_time;
                }
            }

            BeginDrawing();
            ClearBackground(BLACK);

            for (const Cell &cell : m_cells) {
                cell.DrawCell();
            }

            if (m_config.is_debug) {
                DrawFPS(20, 20);
                DrawText(
                    m_is_paused ? "PAUSED (Press SPACE to Run)" : "RUNNING (Press SPACE to Pause)",
                    20, 45, 18, m_is_paused ? RED : GREEN
                );
                DrawText("Left Click: Draw | Right Click: Erase | C: Clear | R: Randomize | N: Step",
                         20, 70, 14, GRAY);
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
    return 0;
}
