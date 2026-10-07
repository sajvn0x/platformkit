#include <X11/Xlib.h>

#include <cstdint>
#include <map>
#include <string>

typedef int WindowId;

enum { INVALID_WINDOW_ID = -1 };

class WindowSystemX11 {
    ::Display* display = nullptr;

    // window
private:
    WindowId window_id_counter = 0;

    struct WindowData {
        Window window;

        // size and position
        int width;
        int height;
        int x_pos;
        int y_pos;

        bool app_focused = true;
        Window embed_parent = 0;
        bool fullscreen = false;
        bool resize_disabled = false;
    };
    std::map<int, WindowData> windows;

    bool xkb_ext_ok = false;
    bool xinerama_ext_ok = false;
    bool xrandr_ext_ok = false;

    WindowId _create_window(std::string p_title, int p_width, int p_height,
                            int p_x_pos, int p_y_pos, Window p_parent_window);
    void _show_window(WindowId p_wid);

public:
    void process_events();
    void get_window_size(WindowId wid, uint32_t* r_width, uint32_t* r_height);
    void set_window_title(WindowId wid, std::string p_title);

    int get_screen_count();

public:
    WindowSystemX11();
    ~WindowSystemX11();
};
