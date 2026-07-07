#include "../iotaWindow.h"
#include <stdexcept>
#include <cstring>
#include <X11/extensions/Xdbe.h>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_UNIX)

struct iotaWindow::PlatformData {
    Display* display = nullptr;
    ::Window window = 0;
    int screen = 0;
    XdbeBackBuffer backBuffer = 0;
    GC gc = 0;
    Atom wmDeleteMessage;
    Atom wmProtocols;
    Atom netWmState;
    Atom netWmStateFullscreen;
    Atom netWmName;
    Atom utf8String;
};

iotaWindow::iotaWindow() 
    : m_platform(std::make_unique<PlatformData>()) {
}

iotaWindow::iotaWindow(const std::string& title, int width, int height) : m_platform(std::make_unique<PlatformData>()) {
    if (!init(title, width, height)) {
        throw std::runtime_error("Failed to initialize window");
    }
}

iotaWindow::~iotaWindow() {
    shutdown();
}

iotaWindow::iotaWindow(iotaWindow&& other) noexcept 
    : m_platform(std::move(other.m_platform))
    , m_title(std::move(other.m_title))
    , m_size(other.m_size)
    , m_position(other.m_position)
    , m_frameColor(other.m_frameColor)
    , m_vsync(other.m_vsync)
    , m_fullscreen(other.m_fullscreen)
    , m_shouldClose(other.m_shouldClose)
    , m_focused(other.m_focused)
    , m_closeCallback(std::move(other.m_closeCallback))
    , m_resizeCallback(std::move(other.m_resizeCallback))
    , m_focusCallback(std::move(other.m_focusCallback)) {
    other.m_platform = std::make_unique<PlatformData>();
    other.m_shouldClose = true;
}

iotaWindow& iotaWindow::operator=(iotaWindow&& other) noexcept {
    if (this != &other) {
        shutdown();
        m_platform = std::move(other.m_platform);
        m_title = std::move(other.m_title);
        m_size = other.m_size;
        m_position = other.m_position;
        m_frameColor = other.m_frameColor;
        m_vsync = other.m_vsync;
        m_fullscreen = other.m_fullscreen;
        m_shouldClose = other.m_shouldClose;
        m_focused = other.m_focused;
        m_closeCallback = std::move(other.m_closeCallback);
        m_resizeCallback = std::move(other.m_resizeCallback);
        m_focusCallback = std::move(other.m_focusCallback);
        other.m_platform = std::make_unique<PlatformData>();
        other.m_shouldClose = true;
    }
    return *this;
}

bool iotaWindow::init(const std::string& title, int width, int height) {
    if (m_platform->window) shutdown();
    m_title = title;
    m_size.width = width;
    m_size.height = height;
    m_shouldClose = false;
    return initPlatform();
}

void iotaWindow::shutdown() {
    shutdownPlatform();
    m_shouldClose = true;
    m_title.clear();
    m_size = {0, 0};
    m_position = {0, 0};
    m_frameColor = FrameColor::DEFAULT;
    m_vsync = true;
    m_fullscreen = false;
    m_focused = false;
    m_closeCallback = nullptr;
    m_resizeCallback = nullptr;
    m_focusCallback = nullptr;
}

bool iotaWindow::initPlatform() {
    auto& p = *m_platform;
    
    p.display = XOpenDisplay(nullptr);
    if (!p.display) return false;
    
    p.screen = DefaultScreen(p.display);
    int rootWindow = RootWindow(p.display, p.screen);
    
    int major, minor;
    bool hasXdbe = XdbeQueryExtension(p.display, &major, &minor);
    
    p.wmDeleteMessage = XInternAtom(p.display, "WM_DELETE_WINDOW", False);
    p.wmProtocols = XInternAtom(p.display, "WM_PROTOCOLS", False);
    p.netWmState = XInternAtom(p.display, "_NET_WM_STATE", False);
    p.netWmStateFullscreen = XInternAtom(p.display, "_NET_WM_STATE_FULLSCREEN", False);
    p.netWmName = XInternAtom(p.display, "_NET_WM_NAME", False);
    p.utf8String = XInternAtom(p.display, "UTF8_STRING", False);
    
    XSetWindowAttributes swa;
    swa.colormap = DefaultColormap(p.display, p.screen);
    swa.event_mask = 
        ExposureMask | StructureNotifyMask |
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask | FocusChangeMask |
        VisibilityChangeMask | PropertyChangeMask;
    swa.background_pixel = BlackPixel(p.display, p.screen);
    
    p.window = XCreateWindow(
        p.display, rootWindow,
        0, 0, m_size.width, m_size.height, 0,
        DefaultDepth(p.display, p.screen),
        InputOutput,
        DefaultVisual(p.display, p.screen),
        CWColormap | CWEventMask | CWBackPixel,
        &swa
    );
    
    if (!p.window) {
        XCloseDisplay(p.display);
        p.display = nullptr;
        return false;
    }
    
    if (hasXdbe) {
        p.backBuffer = XdbeAllocateBackBufferName(p.display, p.window, XdbeCopied);
    }
    
    XGCValues gcValues;
    p.gc = XCreateGC(p.display, p.window, 0, &gcValues);
    
    XSetWMProtocols(p.display, p.window, &p.wmDeleteMessage, 1);
    XStoreName(p.display, p.window, m_title.c_str());
    XChangeProperty(p.display, p.window, p.netWmName, p.utf8String, 8, PropModeReplace, (unsigned char*)m_title.c_str(), m_title.size());
    
    XSizeHints* sizeHints = XAllocSizeHints();
    if (sizeHints) {
        sizeHints->flags = PMinSize | PSize;
        sizeHints->min_width = 320;
        sizeHints->min_height = 200;
        sizeHints->width = m_size.width;
        sizeHints->height = m_size.height;
        XSetWMNormalHints(p.display, p.window, sizeHints);
        XFree(sizeHints);
    }
    
    Screen* screenInfo = DefaultScreenOfDisplay(p.display);
    m_position.x = (screenInfo->width - m_size.width) / 2;
    m_position.y = (screenInfo->height - m_size.height) / 2;
    
    XMoveResizeWindow(p.display, p.window, m_position.x, m_position.y, m_size.width, m_size.height);
    
    XMapWindow(p.display, p.window);
    XFlush(p.display);
    
    m_focused = true;
    return true;
}

void iotaWindow::shutdownPlatform() {
    auto& p = *m_platform;
    
    if (p.backBuffer && p.display) {
        XdbeDeallocateBackBufferName(p.display, p.backBuffer);
        p.backBuffer = 0;
    }
    
    if (p.gc && p.display) {
        XFreeGC(p.display, p.gc);
        p.gc = 0;
    }
    
    if (p.window && p.display) {
        XDestroyWindow(p.display, p.window);
        p.window = 0;
    }
    
    if (p.display) {
        XCloseDisplay(p.display);
        p.display = nullptr;
    }
}

void iotaWindow::pollEvents() {
    auto& p = *m_platform;
    if (!p.display || !p.window) return;
    
    XEvent event;
    while (XPending(p.display)) {
        XNextEvent(p.display, &event);
        
        switch (event.type) {
            case ClientMessage: {
                if ((Atom)event.xclient.data.l[0] == p.wmDeleteMessage) {
                    if (m_closeCallback) m_closeCallback();
                    m_shouldClose = true;
                }
                break;
            }
            
            case ConfigureNotify: {
                XConfigureEvent& xce = event.xconfigure;
                bool resized = (xce.width != m_size.width || xce.height != m_size.height);
                m_size.width = xce.width;
                m_size.height = xce.height;
                m_position.x = xce.x;
                m_position.y = xce.y;
                if (resized && m_resizeCallback) {
                    m_resizeCallback(m_size.width, m_size.height);
                }
                break;
            }
            
            case FocusIn: {
                if (!m_focused) {
                    m_focused = true;
                    if (m_focusCallback) m_focusCallback(true);
                }
                break;
            }
            
            case FocusOut: {
                if (m_focused) {
                    m_focused = false;
                    if (m_focusCallback) m_focusCallback(false);
                }
                break;
            }
            
            case KeyPress: {
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                if (keysym == XK_F11) {
                    setFullscreen(!m_fullscreen);
                }
                break;
            }
        }
    }
}

void iotaWindow::swapBuffers() {
    auto& p = *m_platform;
    if (!p.display || !p.window) return;
    
    if (p.backBuffer) {
        XdbeSwapInfo swapInfo;
        swapInfo.swap_window = p.window;
        swapInfo.swap_action = XdbeCopied;
        XdbeSwapBuffers(p.display, &swapInfo, 1);
    }
    
    XClearWindow(p.display, p.window);
    XFlush(p.display);
}

bool iotaWindow::shouldClose() const {
    return m_shouldClose;
}

void iotaWindow::setTitle(const std::string& title) {
    m_title = title;
    auto& p = *m_platform;
    if (!p.display || !p.window) return;
    
    XStoreName(p.display, p.window, m_title.c_str());
    XChangeProperty(p.display, p.window, p.netWmName, p.utf8String, 8, PropModeReplace, (unsigned char*)m_title.c_str(), m_title.size());
    XFlush(p.display);
}

void iotaWindow::setVSync(bool enabled) {
    m_vsync = enabled;
}

void iotaWindow::setFullscreen(bool fullscreen) {
    if (m_fullscreen == fullscreen) return;
    m_fullscreen = fullscreen;
    
    auto& p = *m_platform;
    if (!p.display || !p.window) return;
    
    XEvent xev;
    std::memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = p.window;
    xev.xclient.message_type = p.netWmState;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = fullscreen ? 1 : 0;
    xev.xclient.data.l[1] = p.netWmStateFullscreen;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 1;
    
    XSendEvent(p.display, DefaultRootWindow(p.display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    
    if (!fullscreen) {
        XMapWindow(p.display, p.window);
        XResizeWindow(p.display, p.window, m_size.width, m_size.height);
    }
    
    XFlush(p.display);
}

void iotaWindow::setSize(int width, int height) {
    m_size.width = width;
    m_size.height = height;
    
    auto& p = *m_platform;
    if (!p.display || !p.window || m_fullscreen) return;
    
    XResizeWindow(p.display, p.window, width, height);
    XFlush(p.display);
}

void iotaWindow::setPosition(int x, int y) {
    m_position.x = x;
    m_position.y = y;
    
    auto& p = *m_platform;
    if (!p.display || !p.window || m_fullscreen) return;
    
    XMoveWindow(p.display, p.window, x, y);
    XFlush(p.display);
}

void iotaWindow::setFrameColor(FrameColor color) {
    m_frameColor = color;
    auto& p = *m_platform;
    if (!p.display || !p.window) return;
    
    if (color == FrameColor::DEFAULT) return;
    
    unsigned int rgba = static_cast<unsigned int>(color);
    unsigned long r = (rgba >> 24) & 0xFF;
    unsigned long g = (rgba >> 16) & 0xFF;
    unsigned long b = (rgba >> 8) & 0xFF;
    
    XColor xcolor;
    xcolor.red = r * 257;
    xcolor.green = g * 257;
    xcolor.blue = b * 257;
    xcolor.flags = DoRed | DoGreen | DoBlue;
    
    Colormap cmap = DefaultColormap(p.display, p.screen);
    XAllocColor(p.display, cmap, &xcolor);
    
    XSetWindowBackground(p.display, p.window, xcolor.pixel);
    XClearWindow(p.display, p.window);
    XFlush(p.display);
}

void iotaWindow::setFrameColor(unsigned int rgba) {
    setFrameColor(static_cast<FrameColor>(rgba));
}

void iotaWindow::resetFrameColor() {
    m_frameColor = FrameColor::DEFAULT;
    auto& p = *m_platform;
    
    if (!p.display || !p.window) return;
    
    Atom motifHints = XInternAtom(p.display, "_MOTIF_WM_HINTS", False);
    XDeleteProperty(p.display, p.window, motifHints);
    XFlush(p.display);
}

std::string iotaWindow::getTitle() const {
    return m_title;
}

iotaWindow::Size iotaWindow::getSize() const {
    auto& p = *m_platform;
    if (!p.display || !p.window) return m_size;
    
    XWindowAttributes attrs;
    if (XGetWindowAttributes(p.display, p.window, &attrs)) {
        Size size;
        size.width = attrs.width;
        size.height = attrs.height;
        return size;
    }
    return m_size;
}

iotaWindow::Position iotaWindow::getPosition() const {
    auto& p = *m_platform;
    if (!p.display || !p.window) return m_position;
    
    Window child;
    int x, y;
    if (XTranslateCoordinates(p.display, p.window, DefaultRootWindow(p.display), 0, 0, &x, &y, &child)) {
        Position pos;
        pos.x = x;
        pos.y = y;
        return pos;
    }
    return m_position;
}

iotaWindow::FrameColor iotaWindow::getFrameColor() const {
    return m_frameColor;
}

bool iotaWindow::isVSync() const {
    return m_vsync;
}

bool iotaWindow::isFullscreen() const {
    return m_fullscreen;
}

bool iotaWindow::isFocused() const {
    return m_focused;
}

void iotaWindow::onClose(EventCallback callback) {
    m_closeCallback = std::move(callback);
}

void iotaWindow::onResize(ResizeCallback callback) {
    m_resizeCallback = std::move(callback);
}

void iotaWindow::onFocusChange(FocusCallback callback) {
    m_focusCallback = std::move(callback);
}

void* iotaWindow::getNativeHandle() const {
    return reinterpret_cast<void*>(m_platform->window);
}

Display* iotaWindow::getDisplay() const {
    return m_platform->display;
}

::Window iotaWindow::getX11Window() const {
    return m_platform->window;
}

void iotaWindow::setBorderless(bool borderless) {
    if (m_borderless == borderless) return;
    m_borderless = borderless;
    
    auto& p = *m_platform;
    if (!p.display || !p.window) return;
    
    Atom motifHints = XInternAtom(p.display, "_MOTIF_WM_HINTS", False);
    
    if (borderless) {
        struct MwmHints {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
            long input_mode;
            unsigned long status;
        };
        
        MwmHints hints;
        hints.flags = 2;
        hints.decorations = 0;
        hints.functions = 0;
        hints.input_mode = 0;
        hints.status = 0;
        
        XWindowAttributes attrs;
        XGetWindowAttributes(p.display, p.window, &attrs);
        m_size.width = attrs.width;
        m_size.height = attrs.height;
        
        XChangeProperty(p.display, p.window, motifHints, motifHints, 32, PropModeReplace, (unsigned char*)&hints, sizeof(MwmHints)/sizeof(long));

        XUnmapWindow(p.display, p.window);
        XMapWindow(p.display, p.window);
        
    } else {
        struct MwmHints {
            unsigned long flags;
            unsigned long functions;
            unsigned long decorations;
            long input_mode;
            unsigned long status;
        };
        
        MwmHints hints;
        hints.flags = 2;
        hints.decorations = 1;
        hints.functions = 0;
        hints.input_mode = 0;
        hints.status = 0;
        
        XChangeProperty(p.display, p.window, motifHints, motifHints, 32, PropModeReplace, (unsigned char*)&hints, sizeof(MwmHints)/sizeof(long));
        
        XUnmapWindow(p.display, p.window);
        XMapWindow(p.display, p.window);
    }
    
    XFlush(p.display);
}

bool iotaWindow::isBorderless() const {
    return m_borderless;
}

#endif