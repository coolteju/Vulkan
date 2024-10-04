#include <windows.h>
#include <vulkan/vulkan.h>
//#include <vulkan/vk_enum_string_helper.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_win32.h>


#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <glm/glm.hpp>

#include <stdexcept> 
#include <set>
#include <vector>
#include <optional>


#pragma comment(lib, "vulkan-1.lib") // lib path is {VULKAN_SDK}/Lib
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")


HDC ghdc;
HWND ghwnd;
const ULONG Win_Width= 800;
const ULONG Win_Height= 600;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily; 
    std::optional<uint32_t> presentFamily;
};

VkInstance instance{};
VkPhysicalDevice physicalDevice={};
VkDevice device;
VkQueue graphicsQueue;
VkQueue presentQueue;

VkSurfaceKHR surface; //object of surface which used to represent the rendered images

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
    void getPhysicalDevice();
    //QueueFamilyIndices findQueueFamily(VkPhysicalDevice);
    void createLogicalDevice();
    void createSurface();

    createInstance();
    createSurface();
    getPhysicalDevice();
    createLogicalDevice();
//    QueueFamilyIndices index= findQueueFamily(physicalDevice);

    return 0;
}

void getExtensionCountAndNames(uint32_t *count, std::set<std::string>&  extensionNames)
{

    // uint32_t count;
    VkExtensionProperties *properties;
    vkEnumerateInstanceExtensionProperties(NULL,count,NULL);
   // assert(count>=1);
    properties= new VkExtensionProperties[*count];

    vkEnumerateInstanceExtensionProperties(NULL, count, properties);

    for(uint32_t i=0;i<*count;i++)
    {
        extensionNames.insert(properties[i].extensionName);    
    }



}



void createInstance()
{

    VkApplicationInfo appInfo{};
    appInfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName="First Window";
    appInfo.applicationVersion=VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName="Shuriken";
    appInfo.engineVersion=VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion=VK_API_VERSION_1_3;

    const char* extensionNames[]={"VK_KHR_win32_surface","VK_KHR_surface"};
    const char* layerNames[]={"VK_LAYER_KHRONOS_validation"};
   
    VkInstanceCreateInfo instanceInfo={};
    instanceInfo.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo=&appInfo;
    instanceInfo.enabledExtensionCount=2;
    instanceInfo.ppEnabledExtensionNames=extensionNames;
    instanceInfo.enabledLayerCount=1;
    instanceInfo.ppEnabledLayerNames=layerNames;
    
    
    VkResult result=vkCreateInstance(&instanceInfo,nullptr, &instance);
    if(result != VK_SUCCESS)
    {
        fprintf(gpFile,"\nVKCreateInstance failed with error code : %d", result);
        fclose(gpFile);
        exit(0);
       // throw std::runtime_error("failed to create Instace");

    }
 }

void createSurface()
{
    VkWin32SurfaceCreateInfoKHR createInfo;
    createInfo.sType=VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd=ghwnd;
    createInfo.hinstance=GetModuleHandle(nullptr);

  VkResult error=  vkCreateWin32SurfaceKHR(instance,&createInfo,nullptr,&surface);
    if(error!=VK_SUCCESS)
    {
        fprintf(gpFile,"\vkCreateWin32SurfaceKHR failed with error code : %d", error);
        fclose(gpFile);
        exit(0);
    }
}

void getPhysicalDevice()
{
    bool isDeviceSuitable(VkPhysicalDevice device);
    uint32_t deviceCount=0;
    vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);

    if(deviceCount==0)
    {
        fprintf(gpFile,"Not able to find the vulkan support GPUs");
        exit(-1);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance,&deviceCount,devices.data());

    for(const auto&device: devices)
    {
        if(isDeviceSuitable(device))
        {
            physicalDevice=device;
        }
    }

    if(physicalDevice==VK_NULL_HANDLE)
    {
        fprintf(gpFile,"Failed to find the suitable GPU");
        exit(-1
        );
    }

}

 bool isDeviceSuitable(VkPhysicalDevice device)
 {
    QueueFamilyIndices findQueueFamily(VkPhysicalDevice);
    // VkPhysicalDeviceProperties deviceProperties;
    // vkGetPhysicalDeviceProperties(device,&deviceProperties);
    // VkPhysicalDeviceFeatures deviceFeatures;
    // vkGetPhysicalDeviceFeatures(device,&deviceFeatures);

    // return (deviceProperties.deviceType==VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
    //         deviceProperties.deviceType==VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)&&
    //         deviceFeatures.geometryShader;

    QueueFamilyIndices indices=findQueueFamily(device);

    return indices.graphicsFamily.has_value();
 }

QueueFamilyIndices findQueueFamily(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount=0;
    VkBool32 presentSupport=false;

    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,nullptr);

    std::vector<VkQueueFamilyProperties>queueFamilies(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,queueFamilies.data());

    int i =0;


    for(i=0;i<queueFamilies.size();i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface, &presentSupport);

        if(queueFamilies[i].queueFlags& VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily=i;
        }

        if(presentSupport)
        {
            indices.presentFamily=i;
        }
    }


    return indices;
 
    

}

void createLogicalDevice()
{
    QueueFamilyIndices findQueueFamily(VkPhysicalDevice);
      
    QueueFamilyIndices index=findQueueFamily(physicalDevice);

    //describe number devices we want for single queue family
    VkDeviceQueueCreateInfo queueCreateInfo[2]{};
    queueCreateInfo[0].sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].queueFamilyIndex=index.graphicsFamily.value();
    queueCreateInfo[0].queueCount=1;

    float queuePriority=1.0;
    queueCreateInfo[0].pQueuePriorities=&queuePriority;

    queueCreateInfo[1].sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[1].queueFamilyIndex=index.presentFamily.value();
    queueCreateInfo[1].queueCount=1;

    queueCreateInfo[1].pQueuePriorities=&queuePriority;

    //set of device features we want for the program
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount=2;
    deviceCreateInfo.pQueueCreateInfos=queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures=&deviceFeatures;

    //device specific extension is VK_KHR_swapchain 
    deviceCreateInfo.enabledExtensionCount=0;

    deviceCreateInfo.enabledLayerCount=0;
    //createInfo.ppEnabledLAyerNames=.char* string --> this is ignored by the current up to date implmentations  


    //code for validation layers is ramaining for previous vulkan implementation
    // if you have enabled validation layers


    VkResult result=vkCreateDevice(physicalDevice,&deviceCreateInfo,nullptr,&device);
    if(result!=VK_SUCCESS)
    {
        fprintf(gpFile,"\nError in logical device creation");
        exit(0);
    }


    vkGetDeviceQueue(device,index.graphicsFamily.value(),0,&graphicsQueue);
    if(graphicsQueue==NULL)
    {
        fprintf(gpFile,"\nError in vkGetDeviceQueue");
        exit(0);
    }
    
    vkGetDeviceQueue(device,index.presentFamily.value(),0,&presentQueue);
    if(presentQueue==NULL)
    {
        fprintf(gpFile,"\nError in vkGetDeviceQueue presentQueue");
        exit(0);
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

    if(surface!=NULL)
    {
        vkDestroySurfaceKHR(instance,surface,nullptr);
        surface=NULL;
    }
    if (instance!=NULL)
    {
        vkDestroyInstance(instance, nullptr);
    }
    
    if(device!=NULL)
    {
        vkDestroyDevice(device,nullptr);
    }
    
    


}
