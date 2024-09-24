#include <windows.h>
#include <vulkan/vulkan.hpp>
//#include <vulkan/vk_enum_string_helper.h>

#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <glm/glm.hpp>

#pragma comment(lib, "vulkan-1.lib") // lib path is {VULKAN_SDK}/Lib
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")


HDC ghdc;
HWND ghwnd;
const ULONG Win_Width= 800;
const ULONG Win_Height= 600;


VkInstance instance{};

WINDOWPLACEMENT wpPrev= {sizeof(WINDOWPLACEMENT)};
DWORD dwStyle;
FILE* gpFile;

bool gbIsFullScreen=false;
bool gbIsWindowActive=false;


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance,LPSTR lpstr, int iCmdShow)
{
    int initialize();
    void display();
    void update();


    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR lpszClassName[]=TEXT("Vulkan Window");
    int iRet=0;
    bool bDone=false;

    fopen_s(&gpFile,"logger.txt","w+");
    assert(gpFile!=nullptr);

    wndclass.cbSize=sizeof(WNDCLASSEX);
    wndclass.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    wndclass.cbWndExtra= 0;
    wndclass.cbClsExtra=0;
    wndclass.lpszClassName=lpszClassName;
    wndclass.lpszMenuName=nullptr;
    wndclass.lpfnWndProc=WndProc;
    wndclass.hInstance=hInstance;
    wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    wndclass.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
    wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);

    RegisterClassEx(&wndclass);

    hwnd=CreateWindowEx(WS_EX_APPWINDOW,lpszClassName,TEXT("Vulkan Window"),WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,100,100,Win_Width,Win_Height,NULL,NULL,hInstance,NULL);

    ghwnd=hwnd;

    iRet=initialize();
    if(iRet==-1)
    {
        fprintf(gpFile,"\nError in initialization");
        exit(0);
    }
    else
    {
        fprintf(gpFile,"\n Initializatin of Vulkan is successfull");
    }

    ShowWindow(hwnd,iCmdShow);
    SetFocus(hwnd);
    SetForegroundWindow(hwnd);

    while(bDone==false)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            if(msg.message==WM_QUIT)
            {
                bDone=true;
            }
            else{
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if(gbIsWindowActive)
            {
                //update
            }
            display();
        }
    }

    return(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam, LPARAM lParam)
{
    void ToggleFullscreen();

    void resize(int , int);

    void uninitialize();

    switch(iMsg)
    {
        case WM_SETFOCUS:
             gbIsWindowActive=true;
             break;
        case WM_KILLFOCUS:
            gbIsWindowActive=false;
            break;
        case WM_SIZE:
            resize(LOWORD(lParam),HIWORD(lParam));
            break;
        case WM_KEYDOWN:
            switch(wParam)
            {
                case VK_ESCAPE:
                    DestroyWindow(hwnd);
                    break;
                case 0x46:
                    ToggleFullscreen();
                    break;
            }
        break;
       case WM_DESTROY:
            uninitialize();
            PostQuitMessage(0);
            break;
    }

    return(DefWindowProc(hwnd,iMsg,wParam,lParam));


}

void ToggleFullscreen()
{
    MONITORINFO mi;
    if(gbIsFullScreen==false)
    {
        dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
        if(dwStyle & WS_OVERLAPPEDWINDOW)
        {
            mi={sizeof(MONITORINFO)};
            if(GetWindowPlacement(ghwnd,&wpPrev)&& GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY),&mi))
            {
                SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right-mi.rcMonitor.left,
                                                                mi.rcMonitor.bottom-mi.rcMonitor.top,SWP_FRAMECHANGED|SWP_NOZORDER);
            }
        }
        ShowCursor(FALSE);
        gbIsFullScreen=true;

    }
    else
    {
        SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghwnd,&wpPrev);
        SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_FRAMECHANGED|SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
        ShowCursor(TRUE);
        gbIsFullScreen=false;
    }

}

void resize(int width,int height)
{
    if(height==-1)
        height=1;
//viewport
//projetions

}

int initialize()
{
    void createInstance();

    createInstance();
    return 0;
}

void createInstance()
{
    // VkApplicationInfo appInfo{};
    // appInfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // appInfo.pApplicationName="First Window";
    // appInfo.applicationVersion=VK_MAKE_VERSION(1,0,0);
    // appInfo.pEngineName="Shuriken";
    // appInfo.engineVersion=VK_MAKE_VERSION(1,0,0);
    // appInfo.apiVersion=VK_API_VERSION_1_3;

    VkApplicationInfo appInfo={};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    uint32_t count;
    VkExtensionProperties *properties;
    vkEnumerateInstanceExtensionProperties(NULL,&count,NULL);
   // assert(count>=1);
    properties= new VkExtensionProperties[count];

    vkEnumerateInstanceExtensionProperties(NULL, &count, properties);

    char** propertyNames;
    propertyNames= new char*[count];
    for(int i=0;i<count;i++)
    {
        propertyNames[i]=new char[strlen(properties[i].extensionName)];
       // fprintf( gpFile,"\n %d :  Property: %s ",i,properties[i].extensionName);
        memcpy(propertyNames[i],properties[i].extensionName, sizeof(properties[i].extensionName));
        fprintf( gpFile,"\n %d, %zd : Property: %s ",i ,strlen(properties[i].extensionName) , propertyNames[i]);
    }

    const char* extensionNames[]={"VK_KHR_win32_surface","VK_KHR_surface"};
    const char* layerNames[]={"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo instanceInfo={};
    instanceInfo.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo=&appInfo;
    instanceInfo.enabledExtensionCount=0;
    instanceInfo.ppEnabledExtensionNames=nullptr;
    instanceInfo.enabledLayerCount=0;
    instanceInfo.ppEnabledLayerNames=nullptr;
    
    
    VkResult result=vkCreateInstance(&instanceInfo,nullptr, &instance);
    if(result != VK_SUCCESS)
    {
        fprintf(gpFile,"\nVKCreateInstance failed with error code : %d", result);
        fclose(gpFile);
        exit(0);
       // throw std::runtime_error("failed to create Instace");

    }
 }

void display()
{

}

void update()
{

}

void uninitialize()
{
    if (gbIsFullScreen)
        {
            SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
            SetWindowPlacement(ghwnd, &wpPrev);
            SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE);

            ShowCursor(TRUE);
            gbIsFullScreen = false;
        }

}

