
echo compile started
cd "01-Window"
rm VKWindow.exe
cl.exe VKWindow.cpp /I "%VK_SDK_PATH%/Include" "%VK_SDK_PATH%/Lib/vulkan-1.lib" /std:c++17
VKWindow.exe
cd ..
echo compile end