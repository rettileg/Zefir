#include "popup.h"
#include "iotaWindow.h"
#include <glad/glad.h>
#include <cstdio>
#include <windows.h>

struct PopupWindow {
    iotaWindow* window;
    HDC dc;
    HGLRC glrc;
};

void* popup_create(const char* title, int width, int height) {
    PopupWindow* pw = new PopupWindow();
    
    pw->window = new iotaWindow();
    if (!pw->window->init(title, width, height)) {
        delete pw->window;
        delete pw;
        return nullptr;
    }
    
    printf("Popup created: %dx%d\n", width, height);
    return pw;
}

void popup_destroy(void* popup) {
    if (popup) {
        PopupWindow* pw = (PopupWindow*)popup;
        pw->window->shutdown();
        delete pw->window;
        delete pw;
    }
}

void popup_begin(void* popup) {
    // N
}

void popup_end(void* popup) {
    if (popup) {
        PopupWindow* pw = (PopupWindow*)popup;
        pw->window->pollEvents();
    }
}

bool popup_should_close(void* popup) {
    if (popup) {
        PopupWindow* pw = (PopupWindow*)popup;
        return pw->window->shouldClose();
    }
    return true;
}

void* popup_get_native_handle(void* popup) {
    if (popup) return ((PopupWindow*)popup)->window->getHwnd();
    return nullptr;
}