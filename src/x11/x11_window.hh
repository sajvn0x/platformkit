#include <X11/Xlib.h>

#include <cstdint>
#include <string>

#ifdef __APPLE__
#define KEY_ESCAPE 61
#else
#define KEY_ESCAPE 9
#endif

class WindowSystemX11 {
    ::Display* display = nullptr;

    // window
private:
    Window window;

    void _create_window(std::string p_title);
    void _show_window();

public:
    void process_events();
    void get_window_size(uint32_t* r_width, uint32_t* r_height);
    void set_window_title(std::string p_title);

public:
    WindowSystemX11();
    ~WindowSystemX11();
};
