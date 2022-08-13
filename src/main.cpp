#include <Windows.h>
#include <stdint.h>
#include <xinput.h>

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
typedef int32 bool32;

Global bool running;

struct Diminsion {
    int width;
    int height;
};

struct ScreenBuffer {
    BITMAPINFO info;
    void *memory;
    int pitch;
    int bytesPerPixel;
};

Global ScreenBuffer globalBuffer;

Internal Diminsion getDiminsion(HWND window) {
    RECT clientRect;
    GetClientRect(window, &clientRect);
    Diminsion diminsion;
    diminsion.width = clientRect.right - clientRect.left;
    diminsion.height =  clientRect.bottom - clientRect.top;
    return diminsion;
}

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pState)
typedef X_INPUT_GET_STATE(x_inpute_get_state);
typedef X_INPUT_SET_STATE(x_inpute_set_state);

X_INPUT_GET_STATE(XInputGetSatetStub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}

X_INPUT_SET_STATE(xInputSetStateStub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}

Global x_inpute_get_state *xInputGetState_ = XInputGetSatetStub;
Global x_inpute_set_state *xInputeSetState_= xInputSetStateStub;

#define XInputGetState xInputGetState_
#define XInputSetState xInputeSetState_

Internal void loadXInput(void) {
    HMODULE xInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!xInputLibrary) {
        HMODULE xInputLibrary = LoadLibraryA("xinput1_3.dll");
    }
    if (xInputLibrary) {
        XInputGetState = (x_inpute_get_state *) GetProcAddress(xInputLibrary, "XInputGetState");
        XInputSetState = (x_inpute_set_state *) GetProcAddress(xInputLibrary, "XInputSetState");

    }
}

Internal void render(ScreenBuffer buffer, int xOffset, int yOffset) {
    int width = buffer.info.bmiHeader.biWidth;
    int height = -buffer.info.bmiHeader.biHeight;

    uint8 *row = (uint8 *) buffer.memory;
    for(int y = 0; y < height; y++) {
        uint32 *pixel = (uint32 *)row;
        for(int x = 0; x < width; x++) {
            uint8 blue = (x) % (y + 1);
            uint8 green = x + xOffset;
            uint8 red = y + yOffset;
            *pixel++ = (red << 16) | (green << 8) | blue;
        }
        row += buffer.pitch;
    }
}

Internal void resize(ScreenBuffer *buffer) {

    if (buffer->memory) {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    int width = buffer->info.bmiHeader.biWidth;
    int height= -buffer->info.bmiHeader.biHeight;
    int bytesPerPixel = buffer->bytesPerPixel;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;

    int bitmapMemorySize = (width * height) * bytesPerPixel;
    buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    buffer->pitch = width*bytesPerPixel;

    render(*buffer, 128, 0);
}

Internal void display(
    HDC deviceContext,
    Diminsion diminsion,
    ScreenBuffer buffer) {
    int width = buffer.info.bmiHeader.biWidth;
    int height = -buffer.info.bmiHeader.biHeight;
    int windowWidth = diminsion.width;
    int windowHeight = diminsion.height;

    StretchDIBits(
        deviceContext,
        0, 0, windowWidth, windowHeight,
        0, 0, width, height,
        buffer.memory,
        &buffer.info,
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
            display(deviceContext, getDiminsion(window), globalBuffer);
            EndPaint(window, &paint);
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            uint32 vkCode = wParam;
            bool wasDown = ((lParam & (1 << 30)) != 0);
            bool isDown = ((lParam & (1 << 31)) == 0);
            switch (vkCode) {
                case 'Q':
                case 'W':
                case 'E':
                case 'A':
                case 'S':
                case 'd':
                case VK_DOWN:
                case VK_UP:
                case VK_LEFT:
                case VK_RIGHT:
                case VK_SPACE:
                case VK_ESCAPE:
                case VK_F4: {
                    if (lParam & (1 << 29)) {
                        running = false;
                    }
                } break;
            }
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
    loadXInput();
    WNDCLASSA window = {};
    window.style = CS_HREDRAW|CS_VREDRAW;
    window.lpfnWndProc = windowCallBack;
    window.hInstance = instance;
    window.lpszClassName = "ssda";
    globalBuffer.bytesPerPixel = 4;
    globalBuffer.info.bmiHeader.biWidth = 1280;
    globalBuffer.info.bmiHeader.biHeight = -720;
    resize(&globalBuffer);

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

                for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
                    XINPUT_STATE controllerState;
                    if (XInputGetState(i, &controllerState) == ERROR_SUCCESS) {
                        XINPUT_GAMEPAD *pad = &controllerState.Gamepad;
                        bool up = pad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
                        bool down = pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                        bool left = pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                        bool right = pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                        bool start = pad->wButtons & XINPUT_GAMEPAD_START;
                        bool back = pad->wButtons & XINPUT_GAMEPAD_BACK;
                        bool leftShoulder = pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
                        bool rightShoulder = pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
                        bool aButton = pad->wButtons & XINPUT_GAMEPAD_A;
                        bool bButton = pad->wButtons & XINPUT_GAMEPAD_B;
                        bool xButton = pad->wButtons & XINPUT_GAMEPAD_X;
                        bool yButton = pad->wButtons & XINPUT_GAMEPAD_Y;
                        int16 stickX = pad->sThumbLX;
                        int16 stickY = pad->sThumbLY;

                    } else {
                        // Todo
                    }
                }
                render(globalBuffer, xOffset, yOffset);
                HDC deviceContext = GetDC(windowHandle);
                display(deviceContext, getDiminsion(windowHandle), globalBuffer);
                ReleaseDC(windowHandle, deviceContext);
                xOffset++;
                yOffset += 2;
                
            }
        } else {

        }

    } else {

    }

    return 0;
}
