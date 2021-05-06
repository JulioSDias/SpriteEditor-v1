#include "Engine_Utils.c"
#include "Engine_Engine.c"
#include "Engine_SpriteEditor.c"

//TODO:
//create big bitmap
//add coordinates
//add lines

I32 __stdcall WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, I32 show_code);
LRESULT __stdcall WindowMessageHandle(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
void InputMessageHandling(MSG message, HWND window, POINT mouse_coord);
void DrawableSurface(HWND window);
void CalcFPS();
F32 GetTime();
void DebugInfo(HDC device_context);
void FullScreen(HWND window, MONITORINFO monitor_info, WINDOWSIZE monitor_size);

I32 __stdcall WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, I32 show_code){
    MSG message;
    
    WINDOWSIZE window_size = {RES_WIDTH, RES_HEIGHT};
    WINDOWSIZE monitor_size = {0};
    MONITORINFO monitor_info = {sizeof(MONITORINFO)};
    POINT mouse_coord = {0};
    
    fps.target = 60;
    fps.delay = 1000/fps.target;
    
    //Window Class
    
    WNDCLASS window_class = {0};
    
    window_class.style = CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = WindowMessageHandle;
    window_class.hInstance = instance;
    window_class.lpszClassName = "Engine";
    
    RegisterClass(&window_class);
    
    //Create Window
    
    HWND window = CreateWindow(window_class.lpszClassName, window_class.lpszClassName,  WS_VISIBLE|WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, window_size.width + 16, window_size.height + 39, 0, 0, instance, 0);
    
    GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
    
    monitor_size.width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
    monitor_size.height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
    
    printf("monitor: %d  %d\n", monitor_size.width, monitor_size.height);
    
    SetUp();
    QueryPerformanceFrequency(&fps.LINT_frequency);
    while(running == 1){
        QueryPerformanceCounter(&fps.LINT_start_time);
        FullScreen(window, monitor_info, monitor_size);
        //Input
        InputMessageHandling(message, window, mouse_coord);
        if(pressed(ESCAPE)) running = 0;
        if(pressed(F3)) debug = !debug;
        if(pressed(F1)) fullscreen = !fullscreen;
        
        //Editor
        SelectColor();
        SelectMapPixel();
        Draw();
        if(pressed(F5)) ExportBMP(drawboard, 8, 8, "mybmp.bmp");
        if(pressed(F6)) ImportBMP();
        if(pressed(F7));
        
        
        //Render
        HDC device_context = GetDC(window);
        StretchDIBits(device_context, 0, 0, back_buffer.width, back_buffer.height, 0, 0, back_buffer.width, back_buffer.height, back_buffer.memory, &back_buffer.bitmapinfo, DIB_RGB_COLORS, SRCCOPY);
        
        if(debug == 1) DebugInfo(device_context);
        ReleaseDC(window, device_context);
        CalcFPS();
    }
    
}

void CalcFPS(){
    fps.count++;
    fps.accum_raw += 1000.0f/GetTime();
    if(fps.delay > GetTime()){
        timeBeginPeriod(1);
        Sleep(fps.delay - GetTime());
        timeEndPeriod(1);
    }
    fps.accum_cooked += 1000.0f/GetTime();
    
    if(fps.count == 75){
        
        fps.raw = fps.accum_raw/fps.count;
        fps.cooked = fps.accum_cooked/fps.count;
        fps.count = 0;
        fps.accum_cooked = 0;
        fps.accum_raw = 0;
    }
}

F32 GetTime(){
    
    F32 time;
    QueryPerformanceCounter(&fps.LINT_end_time);
    time = (F32)(fps.LINT_end_time.QuadPart - fps.LINT_start_time.QuadPart) / (F32)fps.LINT_frequency.QuadPart;
    time *= 1000;
    return time;
}

void DebugInfo(HDC device_context){
    U8 debug_string[64] = {0};
    sprintf(debug_string, "FPS RAW: %.01f", fps.raw);
    TextOut(device_context, 0, 0, debug_string, (I32)strlen(debug_string));
    sprintf(debug_string, "FPS COOKED: %.01f", fps.cooked);
    TextOut(device_context, 0, 16, debug_string, (I32)strlen(debug_string));
    sprintf(debug_string, "RES: %.01f", resolution);
    TextOut(device_context, 0, 32, debug_string, (I32)strlen(debug_string));
    sprintf(debug_string, "MOUSE RAW X: %d, Y: %d", input.mouse.x, input.mouse.y);
    TextOut(device_context, 0, 32, debug_string, (I32)strlen(debug_string));
    sprintf(debug_string, "MOUSE COOKED X: %d, Y: %d", display_mouse.x, display_mouse.y);
    TextOut(device_context, 0, 48, debug_string, (I32)strlen(debug_string));
    sprintf(debug_string, "TILE: %d", tile_num);
    TextOut(device_context, 0, 64, debug_string, (I32)strlen(debug_string));
}

LRESULT __stdcall WindowMessageHandle(HWND window, UINT message, WPARAM w_param, LPARAM l_param){
    
    LRESULT result = 0; 
    
    switch(message){
        case WM_SIZE:{
            //Buffer Size
            RECT window_rect;
            GetClientRect(window, &window_rect);
            back_buffer.width = window_rect.right - window_rect.left;
            back_buffer.height = window_rect.bottom - window_rect.top;
            
            printf("%d   %d\n", back_buffer.width, back_buffer.height);
            
            back_buffer.size = back_buffer.width * back_buffer.height * (back_buffer.bit_count/8);
            
            
            //Allocate Buffer
            if(back_buffer.memory != 0){
                //Free
                VirtualFree(back_buffer.memory, 0, MEM_RELEASE);
            }
            back_buffer.memory = VirtualAlloc(0, back_buffer.size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            
            
            back_buffer.bitmapinfo.bmiHeader.biSize = sizeof(back_buffer.bitmapinfo.bmiHeader);
            back_buffer.bitmapinfo.bmiHeader.biWidth = back_buffer.width;
            back_buffer.bitmapinfo.bmiHeader.biHeight = -back_buffer.height;
            back_buffer.bitmapinfo.bmiHeader.biPlanes = 1;
            back_buffer.bitmapinfo.bmiHeader.biBitCount = back_buffer.bit_count;
            back_buffer.bitmapinfo.bmiHeader.biCompression = BI_RGB;
            
            DrawableSurface(window);
            
            resolution = (F32)(draw_surface.width)/(F32)RES_WIDTH;
            
        }break;
        
        case WM_CLOSE:{
            running = 0;
        }break;
        
        case WM_DESTROY:{
            running = 0;
        }break;
        
        default:{
            result = DefWindowProc(window, message, w_param, l_param);
        }break;
    }
    
    return result;
}

void InputMessageHandling(MSG message, HWND window, POINT mouse_coord){
    
    for(U32 i = 0; i < BUTTON_COUNT; i++) input.key[i].changed = 0;
    
    
#define ProcessButtonPress(code, b) if(VKCODE == code) {if(input.key[b].is_down == 1) input.key[b].changed = 0; if(input.key[b].is_down == 0) input.key[b].changed = 1; input.key[b].is_down = 1;}
    
#define ProcessButtonRelease(code, b) if(VKCODE == code) {if(input.key[b].is_down == 1) input.key[b].changed = 1; if(input.key[b].is_down == 0) input.key[b].changed = 0; input.key[b].is_down = 0;}
    
    while(PeekMessage(&message, window, 0, 0, PM_REMOVE)){
        switch(message.message){
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:{
                U32 VKCODE = (U32)message.wParam;
                
                ProcessButtonPress(VK_LBUTTON, LBUTTON);
                ProcessButtonRelease(VK_LBUTTON, LBUTTON);
            }break;
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:{
                U32 VKCODE = (U32)message.wParam;
                
                ProcessButtonPress(VK_UP, UP);
                ProcessButtonPress(VK_F1, F1);
                ProcessButtonPress(VK_F3, F3);
                ProcessButtonPress(VK_F5, F5);
                ProcessButtonPress(VK_F6, F6);
                ProcessButtonPress(VK_F7, F7);
                ProcessButtonPress(VK_ESCAPE, ESCAPE);
            }break;
            case WM_SYSKEYUP:
            case WM_KEYUP:{
                U32 VKCODE = (U32)message.wParam;
                
                ProcessButtonRelease(VK_UP, UP);
                ProcessButtonRelease(VK_F1, F1);
                ProcessButtonRelease(VK_F3, F3);
                ProcessButtonRelease(VK_F5, F5);
                ProcessButtonRelease(VK_F6, F6);
                ProcessButtonRelease(VK_F7, F7);
                ProcessButtonRelease(VK_ESCAPE, ESCAPE);
            }break;
            default:{
                TranslateMessage(&message);
                DispatchMessage(&message);
            }break;
        }
        GetCursorPos(&mouse_coord);
        ScreenToClient(window, &mouse_coord);
        input.mouse.x = (U32)mouse_coord.x;
        input.mouse.y = (U32)mouse_coord.y;
        
    }
    
}

void DrawableSurface(HWND window){
    
    U32 client_width;
    U32 client_height;
    RECT window_rect;
    GetClientRect(window, &window_rect);
    client_width = window_rect.right - window_rect.left;
    client_height = window_rect.bottom - window_rect.top;
    
    //Crop Top|Bottom
    if((F32)client_width/(F32)client_height <= RATIO){
        draw_surface.height = (client_width * 3)/4;
        draw_surface.width = client_width;
        black_bar = (client_height - draw_surface.height)/2;
        bar_placement = 1;
    }
    
    //Crop Left|Right
    if((F32)client_width/(F32)client_height >= RATIO){
        draw_surface.width = (client_height * 4)/3;
        draw_surface.height = client_height;
        black_bar = (client_width - draw_surface.width)/2;
        bar_placement = 2;
    }
    
    printf("draw: %d  %d %d\n", draw_surface.width, draw_surface.height, black_bar);
}

void FullScreen(HWND window, MONITORINFO monitor_info, WINDOWSIZE monitor_size){
    if(fullscreen == 1){
        SetWindowLong(window, GWL_STYLE, WS_VISIBLE);
        SetWindowPos(window, HWND_TOPMOST, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top, monitor_size.width, monitor_size.height,
                     SWP_NOZORDER|SWP_DRAWFRAME);
    }
    if(fullscreen == 0){
        SetWindowLong(window, GWL_STYLE, WS_VISIBLE|WS_OVERLAPPEDWINDOW);
        SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOSIZE);
    }
}