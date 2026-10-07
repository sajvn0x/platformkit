#include "x11_window.hh"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>

#include <iostream>

WindowSystemX11::WindowSystemX11() {
    display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "failed to open x11 dispaly" << std::endl;
        return;
    }

    // xkb extension
    {
        int opcode;
        int event;
        int error;
        int minor_version;
        int major_version;

        Bool supported = XkbQueryExtension(display, &opcode, &event, &error,
                                           &major_version, &minor_version);
        xkb_ext_ok = supported;
    }

    // xinerama extension
    {
        int event_base;
        int error_base;

        Bool supported =
            XineramaQueryExtension(display, &event_base, &error_base);
        xinerama_ext_ok = supported;
    }

    // xrandr extension
    {
        int event_base;
        int error_base;

        Bool supported = XRRQueryExtension(display, &event_base, &error_base);
        xrandr_ext_ok = supported;
    }

    WindowId wid = _create_window("platform kit window", 1920, 1080, 0, 0,
                                  DefaultRootWindow(display));

    _show_window(wid);
}

WindowSystemX11::~WindowSystemX11() {
    for (int i = 0; i < window_id_counter; ++i) {
        if (windows[i].window) {
            XUnmapWindow(display, windows[i].window);
            XDestroyWindow(display, windows[i].window);
        }
    }
    XCloseDisplay(display);
}

void WindowSystemX11::process_events() {
    XEvent event;
    bool loop = true;

    while (loop) {
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose: {
                for (int i = 0; i < window_id_counter; ++i) {
                    if (windows[i].window == event.xkey.window) {
                        windows[i].app_focused = true;
                    } else {
                        windows[i].app_focused = false;
                    }
                }
            } break;

            case ResizeRequest: {
                WindowData* w_data = nullptr;
                for (int i = 0; i < window_id_counter; ++i) {
                    if (windows[i].window == event.xkey.window) {
                        w_data = &windows[i];
                        break;
                    }
                }

                if (w_data) {
                    if (w_data->resize_disabled) {
                        XResizeWindow(display, w_data->window, 100, 100);
                    }
                }

                break;
            }

            case KeyPress: {
                KeySym keysym =
                    XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
                if (keysym == XK_Escape) loop = false;
                break;
            }
        }
    }
}

void WindowSystemX11::get_window_size(WindowId wid, uint32_t* r_width,
                                      uint32_t* r_height) {
    XWindowAttributes attributes;
    XGetWindowAttributes(display, windows[wid].window, &attributes);
    *r_width = attributes.width;
    *r_height = attributes.height;
}

void WindowSystemX11::set_window_title(WindowId wid, std::string p_title) {
    if (!p_title.empty()) {
        XStoreName(display, windows[wid].window, p_title.c_str());
    }
}

int WindowSystemX11::get_screen_count() {
    int monitor_count = 0;
    if (xinerama_ext_ok) {
        XineramaQueryScreens(display, &monitor_count);
    }

    if (monitor_count == 0) {
        monitor_count = XScreenCount(display);
    }

    return monitor_count;
}

WindowId WindowSystemX11::_create_window(std::string p_title, int p_width,
                                         int p_height, int p_x_pos, int p_y_pos,
                                         Window p_parent_window) {
    int screen = DefaultScreen(display);
    XSetWindowAttributes attributes = {
        .background_pixel = BlackPixel(display, screen),
        .event_mask = ExposureMask | KeyPressMask};

    Window window = XCreateWindow(
        display, p_parent_window, p_x_pos, p_y_pos, p_width, p_height, 0,
        DefaultDepth(display, screen), InputOutput,
        DefaultVisual(display, screen), CWBackPixel | CWEventMask, &attributes);

    if (!p_title.empty()) {
        XStoreName(display, window, p_title.c_str());
    }

    WindowId wid = window_id_counter++;
    windows[wid].window = window;
    windows[wid].embed_parent = p_parent_window;
    windows[wid].width = p_width;
    windows[wid].height = p_height;
    windows[wid].x_pos = p_x_pos;
    windows[wid].y_pos = p_y_pos;

    return wid;
}

void WindowSystemX11::_show_window(WindowId p_wid) {
    XMapWindow(display, windows[p_wid].window);
}
