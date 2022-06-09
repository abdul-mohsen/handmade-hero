#include <Windows.h>

#define Internal static
#define Local static
#define Global static

Global bool running;
Global BITMAPINFO bitmapInfo;
Global void *bitmapMemory;
Global HBITMAP bitmapHandle;
Global HDC bitmapDeviceContext;

Internal void resize(int width, int height) {
    // TODO @ssda free momery

    if (bitmapHandle) {
        DeleteObject(bitmapHandle);
    }
    if (!bitmapDeviceContext) {
        bitmapDeviceContext = CreateCompatibleDC(0);
    }

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    bitmapHandle =  CreateDIBSection(
        bitmapDeviceContext, &bitmapInfo,
        DIB_RGB_COLORS,
        &bitmapMemory,
        0, 0
    );
}

Internal void updateWindow(HDC deviceContext, int x, int y, int width, int hieght) {

    StretchDIBits(
        deviceContext,
        x, y, width, hieght,
        x, y, width, hieght,
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
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int height = paint.rcPaint.bottom -  paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            PatBlt(deviceContext, x, y, width, height, WHITENESS);
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
            while(running) {
                BOOL messageResult = GetMessageA(&message, 0, 0, 0);
                if(messageResult > 0) {
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                } else {
                    break;
                }
            }
        } else {

        }

    } else {

    }

    return 0;
}
