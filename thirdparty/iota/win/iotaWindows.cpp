#include "../iotaWindow.h"
#include <stdexcept>
#include <cassert>

#if defined(PLATFORM_WINDOWS)

struct iotaWindow::PlatformData {
    HWND hwnd = nullptr;
    HINSTANCE hinstance = nullptr;
    std::string className;
    bool classRegistered = false;
    
    DWORD style = 0;
    DWORD exStyle = 0;

    WINDOWPLACEMENT windowPlacement = {};

    bool useCustomFrameColor = false;
    COLORREF captionColor = RGB(0, 0, 0);
    BOOL useDarkMode = FALSE;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void FrameColorToRGB(iotaWindow::FrameColor color, unsigned char& r, unsigned char& g, unsigned char& b) {
    unsigned int rgba = static_cast<unsigned int>(color);
    r = (rgba >> 24) & 0xFF;
    g = (rgba >> 16) & 0xFF;
    b = (rgba >> 8) & 0xFF;
}

iotaWindow::iotaWindow() 
    : m_platform(std::make_unique<PlatformData>()) {
}

iotaWindow::iotaWindow(const std::string& title, int width, int height) 
    : m_platform(std::make_unique<PlatformData>()) {
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
    if (m_platform->hwnd) {
        shutdown();
    }
    
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
    
    p.hinstance = GetModuleHandle(nullptr);
    p.className = "iotaWindowClass_" + std::to_string(reinterpret_cast<uintptr_t>(this));
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = p.hinstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = p.className.c_str();
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    
    ATOM atom = RegisterClassEx(&wc);
    if (!atom) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }
    p.classRegistered = true;

    p.style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    p.exStyle = 0;
    RECT rect = { 0, 0, m_size.width, m_size.height };
    AdjustWindowRectEx(&rect, p.style, FALSE, p.exStyle);
    int adjustedWidth = rect.right - rect.left;
    int adjustedHeight = rect.bottom - rect.top;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    m_position.x = (screenWidth - adjustedWidth) / 2;
    m_position.y = (screenHeight - adjustedHeight) / 2;

    p.hwnd = CreateWindowEx(
        p.exStyle,
        p.className.c_str(),
        m_title.c_str(),
        p.style,
        m_position.x, m_position.y,
        adjustedWidth, adjustedHeight,
        nullptr, nullptr,
        p.hinstance,
        this
    );
    
    if (!p.hwnd) {
        UnregisterClass(p.className.c_str(), p.hinstance);
        p.classRegistered = false;
        return false;
    }

    if (m_frameColor != FrameColor::DEFAULT) {
        setFrameColorPlatform();
    }

    ShowWindow(p.hwnd, SW_SHOW);
    UpdateWindow(p.hwnd);
    
    m_focused = (GetForegroundWindow() == p.hwnd);
    
    return true;
}

void iotaWindow::swapBuffers() {
    auto& p = *m_platform;
    if (!p.hwnd) return;
    
    HDC hdc = GetDC(p.hwnd);
    SwapBuffers(hdc);
    ReleaseDC(p.hwnd, hdc);
}

void iotaWindow::shutdownPlatform() {
    auto& p = *m_platform;
    
    if (p.hwnd) {
        SetWindowLongPtr(p.hwnd, GWLP_USERDATA, 0);
        if (m_fullscreen) {
            SetWindowLongPtr(p.hwnd, GWL_STYLE, p.style);
            SetWindowLongPtr(p.hwnd, GWL_EXSTYLE, p.exStyle);
        }
        
        DestroyWindow(p.hwnd);
        p.hwnd = nullptr;
    }
    
    if (p.classRegistered) {
        UnregisterClass(p.className.c_str(), p.hinstance);
        p.classRegistered = false;
    }
    
    p.hinstance = nullptr;
    p.style = 0;
    p.exStyle = 0;
}

void iotaWindow::pollEvents() {
    auto& p = *m_platform;
    if (!p.hwnd) return;
    
    MSG msg = {};
    while (PeekMessage(&msg, p.hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool iotaWindow::shouldClose() const {
    return m_shouldClose;
}

void iotaWindow::setTitle(const std::string& title) {
    m_title = title;
    if (m_platform->hwnd) {
        SetWindowText(m_platform->hwnd, m_title.c_str());
    }
}

void iotaWindow::setVSync(bool enabled) {
    m_vsync = enabled;
}

void iotaWindow::setFullscreen(bool fullscreen) {
    if (m_fullscreen == fullscreen) return;
    if (!m_platform || !m_platform->hwnd) return;
    
    auto& p = *m_platform;
    
    if (fullscreen) {
        m_fullscreen = true;
        
        p.windowPlacement.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(p.hwnd, &p.windowPlacement);
        
        p.style = GetWindowLongPtr(p.hwnd, GWL_STYLE);
        p.exStyle = GetWindowLongPtr(p.hwnd, GWL_EXSTYLE);
        
        HMONITOR monitor = MonitorFromWindow(p.hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo = {};
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(monitor, &monitorInfo);
        
        SetWindowLongPtr(p.hwnd, GWL_STYLE, 
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
        SetWindowLongPtr(p.hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
        
        SetWindowPos(
            p.hwnd, HWND_TOP,
            monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.top,
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW
        );

        m_size.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        m_size.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
        
    } else {
        m_fullscreen = false;

        SetWindowLongPtr(p.hwnd, GWL_STYLE, p.style);
        SetWindowLongPtr(p.hwnd, GWL_EXSTYLE, p.exStyle);
        SetWindowPlacement(p.hwnd, &p.windowPlacement);

        SetWindowPos(p.hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        
        RECT rect;
        GetClientRect(p.hwnd, &rect);
        m_size.width = rect.right - rect.left;
        m_size.height = rect.bottom - rect.top;
    }

    setSize(m_size.width, m_size.height);
}

void iotaWindow::setSize(int width, int height) {
    m_size.width = width;
    m_size.height = height;
    
    auto& p = *m_platform;
    if (!p.hwnd || m_fullscreen) return;
    
    RECT rect = { 0, 0, width, height };
    DWORD style = GetWindowLongPtr(p.hwnd, GWL_STYLE);
    DWORD exStyle = GetWindowLongPtr(p.hwnd, GWL_EXSTYLE);

    AdjustWindowRectEx(&rect, style, FALSE, exStyle);
    
    int adjustedWidth = rect.right - rect.left;
    int adjustedHeight = rect.bottom - rect.top;
    
    SetWindowPos(
        p.hwnd, nullptr,
        0, 0,
        adjustedWidth, adjustedHeight,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE
    );

    RECT clientRect;
    GetClientRect(p.hwnd, &clientRect);
    m_size.width = clientRect.right - clientRect.left;
    m_size.height = clientRect.bottom - clientRect.top;
}

void iotaWindow::setBorderless(bool borderless) {
    if (m_borderless == borderless) return;
    if (!m_platform || !m_platform->hwnd) return;
    
    auto& p = *m_platform;
    m_borderless = borderless;
    
    if (borderless) {
        p.style = GetWindowLongPtr(p.hwnd, GWL_STYLE);
        p.exStyle = GetWindowLongPtr(p.hwnd, GWL_EXSTYLE);
        
        p.windowPlacement.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(p.hwnd, &p.windowPlacement);
        
        SetWindowLongPtr(p.hwnd, GWL_STYLE, 
            WS_POPUP | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
        SetWindowLongPtr(p.hwnd, GWL_EXSTYLE, 
            WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
        
        SetWindowPos(p.hwnd, nullptr, 
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            
    } else {
        SetWindowLongPtr(p.hwnd, GWL_STYLE, p.style);
        SetWindowLongPtr(p.hwnd, GWL_EXSTYLE, p.exStyle);
        
        SetWindowPlacement(p.hwnd, &p.windowPlacement);
        SetWindowPos(p.hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

bool iotaWindow::isBorderless() const {
    return m_borderless;
}

void iotaWindow::setPosition(int x, int y) {
    m_position.x = x;
    m_position.y = y;
    
    auto& p = *m_platform;
    if (!p.hwnd || m_fullscreen) return;
    
    SetWindowPos(
        p.hwnd, nullptr,
        x, y,
        0, 0,
        SWP_NOSIZE | SWP_NOZORDER
    );
}

void iotaWindow::setFrameColor(FrameColor color) {
    m_frameColor = color;
    if (m_platform->hwnd) {
        setFrameColorPlatform();
    }
}

void iotaWindow::setFrameColor(unsigned int rgba) {
    setFrameColor(static_cast<FrameColor>(rgba));
}

void iotaWindow::resetFrameColor() {
    m_frameColor = FrameColor::DEFAULT;
    
    auto& p = *m_platform;
    p.useCustomFrameColor = false;
    
    if (!p.hwnd) return;
    
    BOOL useDarkMode = FALSE;
    DwmSetWindowAttribute(p.hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, 
                         &useDarkMode, sizeof(useDarkMode));
    
    SetWindowPos(p.hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void iotaWindow::setFrameColorPlatform() {
    auto& p = *m_platform;
    if (!p.hwnd || m_frameColor == FrameColor::DEFAULT) {
        resetFrameColor();
        return;
    }
    
    p.useCustomFrameColor = true;
    
    unsigned char r, g, b;
    FrameColorToRGB(m_frameColor, r, g, b);
    p.captionColor = RGB(r, g, b);
    
    unsigned int brightness = (static_cast<unsigned int>(r) * 299 + static_cast<unsigned int>(g) * 587 + static_cast<unsigned int>(b) * 114) / 1000;
    p.useDarkMode = (brightness < 128);
    
    DwmSetWindowAttribute(p.hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &p.useDarkMode, sizeof(p.useDarkMode));
    
    DwmSetWindowAttribute(p.hwnd, DWMWA_CAPTION_COLOR, &p.captionColor, sizeof(p.captionColor));
    
    MARGINS margins = {0, 0, 0, 1};
    DwmExtendFrameIntoClientArea(p.hwnd, &margins);
    
    SetWindowPos(p.hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

std::string iotaWindow::getTitle() const {
    return m_title;
}

iotaWindow::Size iotaWindow::getSize() const {
    auto& p = *m_platform;
    if (!p.hwnd) return m_size;
    
    RECT rect;
    if (GetClientRect(p.hwnd, &rect)) {
        Size size;
        size.width = rect.right - rect.left;
        size.height = rect.bottom - rect.top;
        return size;
    }
    
    return m_size;
}

iotaWindow::Position iotaWindow::getPosition() const {
    auto& p = *m_platform;
    if (!p.hwnd) return m_position;
    
    RECT rect;
    if (GetWindowRect(p.hwnd, &rect)) {
        Position pos;
        pos.x = rect.left;
        pos.y = rect.top;
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
    return static_cast<void*>(m_platform->hwnd);
}

HWND iotaWindow::getHwnd() const {
    return m_platform->hwnd;
}

HINSTANCE iotaWindow::getHinstance() const {
    return m_platform->hinstance;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    iotaWindow* window = nullptr;
    
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = static_cast<iotaWindow*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        
        if (window) {
            window->m_platform->hwnd = hwnd;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    window = reinterpret_cast<iotaWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    if (!window) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    
    switch (msg) {
        case WM_CLOSE: {
            if (window->m_closeCallback) {
                window->m_closeCallback();
            }
            window->m_shouldClose = true;
            return 0;
        }
        
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        
        case WM_SIZE: {
            if (wParam != SIZE_MINIMIZED) {
                RECT rect;
                GetClientRect(hwnd, &rect);
                window->m_size.width = rect.right - rect.left;
                window->m_size.height = rect.bottom - rect.top;
                
                if (window->m_resizeCallback) {
                    window->m_resizeCallback(window->m_size.width, window->m_size.height);
                }
            }
            return 0;
        }
        
        case WM_MOVE: {
            if (!window->m_fullscreen && !IsIconic(hwnd)) {
                RECT rect;
                GetWindowRect(hwnd, &rect);
                window->m_position.x = rect.left;
                window->m_position.y = rect.top;
            }
            return 0;
        }
        
        case WM_SETFOCUS: {
            if (!window->m_focused) {
                window->m_focused = true;
                if (window->m_focusCallback) {
                    window->m_focusCallback(true);
                }
            }
            return 0;
        }
        
        case WM_KILLFOCUS: {
            if (window->m_focused) {
                window->m_focused = false;
                if (window->m_focusCallback) {
                    window->m_focusCallback(false);
                }
            }
            return 0;
        }
        
        case WM_ACTIVATE: {
            WORD active = LOWORD(wParam);
            if (active == WA_INACTIVE) {
                if (window->m_focused) {
                    window->m_focused = false;
                    if (window->m_focusCallback) {
                        window->m_focusCallback(false);
                    }
                }
            } else if (active == WA_ACTIVE || active == WA_CLICKACTIVE) {
                if (!window->m_focused) {
                    window->m_focused = true;
                    if (window->m_focusCallback) {
                        window->m_focusCallback(true);
                    }
                }
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (wParam == VK_F11) {
                window->setFullscreen(!window->isFullscreen());
                return 0;
            }
            break;
        }
        
        case WM_SYSKEYDOWN: {
            if (wParam == VK_F4 && (lParam & (1 << 29))) {
                if (window->m_closeCallback) {
                    window->m_closeCallback();
                }
                window->m_shouldClose = true;
                return 0;
            }
            if (wParam == VK_RETURN && (lParam & (1 << 29))) {
                window->setFullscreen(!window->isFullscreen());
                return 0;
            }
            break;
        }
        
        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
            mmi->ptMinTrackSize.x = 320;
            mmi->ptMinTrackSize.y = 200;
            return 0;
        }
        
        case WM_NCCALCSIZE: {
        //    if (window->m_platform->useCustomFrameColor && wParam == TRUE) {
      //          return 0;
    //        }
            break;
        }
        
        case WM_NCPAINT: {
        //    if (window->m_platform->useCustomFrameColor) {
      //          return 0;
    //        }
            break;
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif // PLATFORM_WINDOWS