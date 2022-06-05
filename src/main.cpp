#include <Windows.h>

LRESULT CALLBACK windowCallBack(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    LRESULT result = 0;
    switch (message) {
        case WM_SIZE: {

        } break;
        case WM_DESTROY: {

        } break;
        case WM_CLOSE: {

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
            for(;;) {
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
