
echo compile started
cd "01-Window"
cl.exe VKWindow.cpp /I "%VK_SDK_PATH%/Include" "%VK_SDK_PATH%/Lib/vulkan-1.lib"
echo compile end