#include <Windows.h>
#include <stdint.h>

#define Internal static
#define Local static
#define Global static

typedef  uint8_t uint8;
typedef  uint16_t uint16;
typedef  uint32_t uint32;
typedef  uint64_t uint64;

typedef  int8_t nint8;
typedef  int16_t int16;
typedef  int32_t int32;
typedef  int64_t int64;

Global bool running;
Global BITMAPINFO bitmapInfo;
Global void *bitmapMemory;
Global int bytesPerPixel = 4;

Internal void render(int xOffset, int yOffset) {
    int width = bitmapInfo.bmiHeader.biWidth;
    int height = -bitmapInfo.bmiHeader.biHeight;

    int pitch = width*bytesPerPixel;
    uint8 *row = (uint8 *) bitmapMemory;
    for(int y = 0; y < height; y++) {
        uint8 *pixel = (uint8 *)row;
        for(int x = 0; x < width; x++) {
            *pixel = (uint8) x + xOffset;
            ++pixel;

            *pixel = (uint8) y + yOffset;
            ++pixel;

            *pixel = 255;
            ++pixel;
            
            *pixel = 0;
            ++pixel;
        }
        row += pitch;
    }
}

Internal void resize(int width, int height) {

    if (bitmapMemory) {
        VirtualFree(bitmapMemory, 0, MEM_RELEASE);
    }

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    int bitmapMemorySize = (width * height) * bytesPerPixel;
    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    render(128,0);
}

Internal void updateWindow(HDC deviceContext,RECT *windowRect) {
    int width = bitmapInfo.bmiHeader.biWidth;
    int height = -bitmapInfo.bmiHeader.biHeight;
    int windowWidth = windowRect->right - windowRect->left;
    int windowHeight = windowRect->bottom - windowRect->top;

    StretchDIBits(
        deviceContext,
        0, 0, width, height,
        0, 0, windowWidth, windowHeight,
        bitmapMemory,
        &bitmapInfo,
        DIB_RGB_COLORS, SRCCOPY
    );
}

LRESULT CALLBACK windowCallBack(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    LRESULT result = 0;
    switch (message) {
        case WM_SIZE: {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int height = clientRect.bottom -  clientRect.top;
            int width = clientRect.right - clientRect.left;
            resize(width, height);
        } break;
        case WM_DESTROY: {
            running = false;
        } break;
        case WM_CLOSE: {
            running = false;
        } break;
        case WM_ACTIVATEAPP: {

        } break;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            RECT clientRect;
            GetClientRect(window, &clientRect);
            updateWindow(deviceContext, &clientRect);
            EndPaint(window, &paint);
        } break;

        default: {
            result = DefWindowProcA(window, message, wParam, lParam);
        } break;
    }

    return result;
}

int CALLBACK
WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR commandLine,
    int showCode
) {
    WNDCLASSA window = {};
    window.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    window.lpfnWndProc = windowCallBack;
    window.hInstance = instance;
    window.lpszClassName = "ssda";

    if (RegisterClassA(&window)) {
        HWND windowHandle = CreateWindowExA(
            0,
            window.lpszClassName,
            "ssda wweq",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            instance,
            0
        );
        if(windowHandle) {
            MSG message;
            running = true;
            int xOffset = 0;
            int yOffset = 0;
            while(running) {
                while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
                    if (message.message == WM_QUIT) {
                        running = false;
                    }
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                }
                render(xOffset, yOffset);
                RECT clientRect;
                GetClientRect(windowHandle, &clientRect);
                HDC deviceContext = GetDC(windowHandle);
                updateWindow(deviceContext, &clientRect);
                xOffset++;
            }
        } else {

        }

    } else {

    }

    return 0;
}
