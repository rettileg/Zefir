#include "viewport.h"
#include "../renderer.h"
#include <windows.h>
#include <glad/glad.h>
#include <cstdio>

struct ZefirViewport {
    HWND parent_hwnd;
    HWND child_hwnd;
    HDC child_dc;
    HGLRC child_glctx;
    
    HDC main_dc;
    HGLRC main_glctx;
    
    int width, height;
    int x, y;
    bool own_gl_context;
};

static LRESULT CALLBACK viewport_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_SIZE: {
            ZefirViewport* vp = (ZefirViewport*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (vp) {
                RECT r;
                GetClientRect(hwnd, &r);
                vp->width = r.right - r.left;
                vp->height = r.bottom - r.top;
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

ZefirViewport* zefir_viewport_create(ZefirContext* ctx, float x, float y, float w, float h) {
    if (!ctx) return nullptr;
    
    ZefirViewport* vp = new ZefirViewport();
    vp->width = (int)w;
    vp->height = (int)h;
    vp->x = (int)x;
    vp->y = (int)y;
    vp->own_gl_context = true;
    
    extern void* zefir_get_native_handle(ZefirContext* ctx);
    vp->parent_hwnd = (HWND)zefir_get_native_handle(ctx);
    
    vp->main_dc = wglGetCurrentDC();
    vp->main_glctx = wglGetCurrentContext();
    
    static bool class_registered = false;
    if (!class_registered) {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = viewport_wndproc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = "ZefirViewport";
        RegisterClassA(&wc);
        class_registered = true;
    }
    
    vp->child_hwnd = CreateWindowExA(
        WS_EX_NOPARENTNOTIFY,
        "ZefirViewport",
        "",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        (int)x, (int)y, (int)w, (int)h,
        vp->parent_hwnd,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    SetWindowLongPtr(vp->child_hwnd, GWLP_USERDATA, (LONG_PTR)vp);
    
    if (vp->own_gl_context) {
        vp->child_dc = GetDC(vp->child_hwnd);
        
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR), 1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA, 32,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 24, 8, 0,
            PFD_MAIN_PLANE, 0, 0, 0, 0
        };
        
        int pf = ChoosePixelFormat(vp->child_dc, &pfd);
        SetPixelFormat(vp->child_dc, pf, &pfd);
        
        HGLRC main_ctx = wglGetCurrentContext();
        vp->child_glctx = wglCreateContext(vp->child_dc);
        
        if (main_ctx && vp->child_glctx) {
            vp->child_glctx = wglCreateContext(vp->child_dc);
        }
    }
    
    printf("Viewport created: %dx%d\n", (int)w, (int)h);
    return vp;
}

void zefir_viewport_destroy(ZefirViewport* vp) {
    if (vp) {
        if (vp->child_glctx) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(vp->child_glctx);
        }
        if (vp->child_dc) {
            ReleaseDC(vp->child_hwnd, vp->child_dc);
        }
        if (vp->child_hwnd) {
            DestroyWindow(vp->child_hwnd);
        }
        delete vp;
    }
}

void* zefir_viewport_get_native_handle(ZefirViewport* vp) {
    return vp ? vp->child_hwnd : nullptr;
}

void zefir_viewport_begin(ZefirViewport* vp) {
    if (vp && vp->child_glctx) {
        wglMakeCurrent(vp->child_dc, vp->child_glctx);
    }
}

void zefir_viewport_end(ZefirViewport* vp) {
    if (vp && vp->child_dc) {
        SwapBuffers(vp->child_dc);
        if (vp->main_dc && vp->main_glctx) {
            wglMakeCurrent(vp->main_dc, vp->main_glctx);
        }
    }
}

void zefir_viewport_get_size(ZefirViewport* vp, int* w, int* h) {
    if (vp) { *w = vp->width; *h = vp->height; }
}

void zefir_viewport_set_rect(ZefirViewport* vp, float x, float y, float w, float h) {
    if (!vp) return;
    vp->x = (int)x; vp->y = (int)y;
    vp->width = (int)w; vp->height = (int)h;
    SetWindowPos(vp->child_hwnd, nullptr, (int)x, (int)y, (int)w, (int)h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void zefir_viewport_update(ZefirViewport* vp, float x, float y, float w, float h) {
    if (vp && (vp->x != (int)x || vp->y != (int)y || vp->width != (int)w || vp->height != (int)h)) {
        zefir_viewport_set_rect(vp, x, y, w, h);
    }
}