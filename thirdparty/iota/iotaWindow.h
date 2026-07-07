#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__linux__)
    #if defined(__ANDROID__)
        #define PLATFORM_ANDROID
    #else
        #define PLATFORM_LINUX
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define PLATFORM_IOS
    #else
        #define PLATFORM_MACOS
    #endif
#elif defined(__unix__)
    #define PLATFORM_UNIX
#else
    #error "Unknown platform"
#endif

#if defined(PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <dwmapi.h>
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_UNIX)
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xatom.h>
#elif defined(PLATFORM_ANDROID)
    #include <android/native_window.h>
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
    #include <objc/objc.h>
    #include <objc/message.h>
#endif

#include <string>
#include <functional>
#include <memory>

class iotaWindow {
public:
    enum class FrameColor : unsigned int { // work only in windows
        DEFAULT         = 0x00000000,
        
        BLACK           = 0x000000FF,
        DARKEST        = 0x0A0A0AFF,
        DARKER         = 0x121212FF,
        DARK_GRAY      = 0x1A1A1AFF,
        CHARCOAL       = 0x1E1E1EFF,
        GUNMETAL       = 0x252525FF,
        MID_GRAY       = 0x2D2D2DFF,
        
        NAVY           = 0x0A0E27FF,
        DARK_BLUE      = 0x0F1B3DFF,
        MIDNIGHT       = 0x162447FF,
        STEEL_BLUE     = 0x1C2D54FF,
        
        DARK_GREEN     = 0x0A1A0AFF,
        FOREST         = 0x0F2411FF,
        PINE           = 0x162E18FF,

        DARK_RED       = 0x1A0A0AFF,
        MAROON         = 0x241010FF,
        BURGUNDY       = 0x2E1414FF,

        DARK_PURPLE    = 0x1A0F2EFF,
        PLUM           = 0x24183DFF,

        LIGHT_GRAY     = 0xD3D3D3FF,
        SILVER         = 0xE0E0E0FF,
        WHITE          = 0xFFFFFFFF,
        
        iota_TRANSPARENT = 0x00000000
    };

    struct Size {
        int width = 0;
        int height = 0;
    };

    struct Position {
        int x = 0;
        int y = 0;
    };

    using EventCallback = std::function<void()>;
    using ResizeCallback = std::function<void(int width, int height)>;
    using FocusCallback = std::function<void(bool focused)>;

    iotaWindow();
    explicit iotaWindow(const std::string& title, int width = 800, int height = 600);
    ~iotaWindow();

    iotaWindow(const iotaWindow&) = delete;
    iotaWindow& operator=(const iotaWindow&) = delete;
    iotaWindow(iotaWindow&& other) noexcept;
    iotaWindow& operator=(iotaWindow&& other) noexcept;

    bool init(const std::string& title, int width = 800, int height = 600);
    void shutdown();

    void pollEvents();
    void swapBuffers();
    bool shouldClose() const;

    void setTitle(const std::string& title);
    void setVSync(bool enabled);
    void setFullscreen(bool fullscreen);
    void setSize(int width, int height);
    void setPosition(int x, int y);
    void setFrameColor(FrameColor color);
    void setFrameColor(unsigned int rgba);
    void resetFrameColor();
    void setBorderless(bool borderless);
    bool isBorderless() const;

    std::string getTitle() const;
    Size getSize() const;
    Position getPosition() const;
    FrameColor getFrameColor() const;
    bool isVSync() const;
    bool isFullscreen() const;
    bool isFocused() const;

    void onClose(EventCallback callback);
    void onResize(ResizeCallback callback);
    void onFocusChange(FocusCallback callback);

    void* getNativeHandle() const;

#if defined(PLATFORM_WINDOWS)
    HWND getHwnd() const;
    HINSTANCE getHinstance() const;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_UNIX)
    Display* getDisplay() const;
    ::Window getX11Window() const;
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_IOS)
    void* getNSWindow() const;
#endif

#if defined(PLATFORM_WINDOWS)
    friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#if defined(PLATFORM_ANDROID)
public:
    void onNativeWindowCreated(void* activity, void* window);
    void onNativeWindowDestroyed(void* activity, void* window);
    void onInputQueueCreated(void* activity, void* queue);
    void onInputQueueDestroyed(void* activity, void* queue);
    void onConfigurationChanged(void* activity);
#endif

private:
    struct PlatformData;
    std::unique_ptr<PlatformData> m_platform;

    std::string m_title;
    Size m_size;
    Position m_position;
    FrameColor m_frameColor = FrameColor::DEFAULT;
    bool m_vsync = true;
    bool m_fullscreen = false;
    bool m_shouldClose = false;
    bool m_focused = false;
    bool m_borderless = false;

    EventCallback m_closeCallback;
    ResizeCallback m_resizeCallback;
    FocusCallback m_focusCallback;

    bool initPlatform();
    void shutdownPlatform();
    void setTitlePlatform();
    void setVSyncPlatform();
    void setFullscreenPlatform();
    void setSizePlatform();
    void setPositionPlatform();
    void setFrameColorPlatform();
    void swapBuffersPlatform();
    void pollEventsPlatform();

};

constexpr iotaWindow::FrameColor operator""_rgba(unsigned long long hex) {
    return static_cast<iotaWindow::FrameColor>(static_cast<unsigned int>(hex));
}
