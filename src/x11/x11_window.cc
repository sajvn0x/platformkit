#include "x11_window.hh"

#include <iostream>

WindowSystemX11::WindowSystemX11() {
    display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "failed to open x11 dispaly" << std::endl;
        return;
    }

    _create_window("platform kit window");
    _show_window();
}

WindowSystemX11::~WindowSystemX11() {
    if (window) {
        XUnmapWindow(display, window);
        XDestroyWindow(display, window);
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
                break;
            }
            case KeyPress: {
                if (event.xkey.keycode == KEY_ESCAPE) loop = false;
                break;
            }
        }
    }
}

void WindowSystemX11::get_window_size(uint32_t* r_width, uint32_t* r_height) {
    XWindowAttributes attributes;
    XGetWindowAttributes(display, window, &attributes);
    *r_width = attributes.width;
    *r_height = attributes.height;
}

void WindowSystemX11::set_window_title(std::string p_title) {
    if (!p_title.empty()) {
        XStoreName(display, window, p_title.c_str());
    }
}

void WindowSystemX11::_create_window(std::string p_title) {
    int screen = DefaultScreen(display);
    XSetWindowAttributes attributes = {
        .background_pixel = BlackPixel(display, screen),
        .event_mask = ExposureMask | KeyPressMask};

    window = XCreateWindow(display, RootWindow(display, screen), 0, 0, 720, 480,
                           0, DefaultDepth(display, screen), InputOutput,
                           DefaultVisual(display, screen),
                           CWBackPixel | CWEventMask, &attributes);

    if (!p_title.empty()) {
        XStoreName(display, window, p_title.c_str());
    }
}

void WindowSystemX11::_show_window() { XMapWindow(display, window); }
