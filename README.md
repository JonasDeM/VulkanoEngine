
# VulkanoEngine 1.0

## Demo
I included a demo executable for 64-bit Windows. You can find it [here](https://github.com/JonasDeM/VulkanoEngine/releases)
### Instructions
* Use WASD + Hold and move Mouse to navigate in the scene.
* Have an AZERTY keyboard? Press 'P' to toggle to ZQSD controls and back.
* Press 'N'/'M' to go the the next/previous scene.
* Press 'G\' to TOGGLE Debug Visuals.
* Press \'F\' to TOGGLE the FPS counter in the Console Window.

## Project Goals
The goal of this project was to make a maintainable game engine that did 3 core things:
1. Render 3D graphics with the Vulkan API.  
	Working in a C++ (object oriented) way while using a C-library creates a challenge.
2. Simplify the memory management by using smart pointers.
	The 2 principles i used were: 
	* Unique  and shared pointers for ownership, weak and raw pointers if no ownership.
	* At creation immediatly take care of the destruction too, smart pointers do this automatically. But with these objects from used libraries i needed to specify custom deleters. These could be simple release calls, or in the case of Vulkan  lambda expressions that captured the necessary objects to destroy the vulkan object with. To simplify the creation of these smart pointers I made some template helper [functions](https://github.com/JonasDeM/VulkanoEngine/blob/master/VulkanoEngine/HandleUtilities.h).
3. Be multi-platform.
	All used code consists of multi-platform libraries and self-written C++;

## How To Build For Windows
First, clone [this repo](https://github.com/JonasDeM/VulkanoEngine).  
Any system paths specified are just recommendations.  
If you don't use the recommended paths/install directories or use another library version, be sure to change the project properties accordingly for every desired build configuration. Properties to change:
* C/C++: Additional Include Directories
* Linker: Additional Library Directories
* Build Events: PostBuild Event

### Prerequisites
* Vulkan Runtime Libraries (Is probably already installed on your pc)
* [Visual Studio](https://visualstudio.microsoft.com/downloads/)
* [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (Tested with version 1.0.49.0, directory="C:\VulkanSDK")
  * Comes with GLM library (directory="C:\VulkanSDK\1.0.49.0\Third-Party\Include\glm")
* [GLFW binaries](http://www.glfw.org/) (Tested with version 3.2.1, directory="C:\Libraries\glfw")
* [PhysX SDK](https://developer.nvidia.com/physx-sdk) (Tested with version 3.3, directory="C:\Libraries\PhysX-3.3")
  * Nvidia doesn't offer pre-built .dll's or .libs, so you will need to build them yourself. First request acces to [NVIDIAGameWorks](https://developer.nvidia.com/what-is-gameworks) then just follow the guidelines on their readme.
  * __IMPORTANT__: In the PhysX solution, change Code Generation to Multi-threaded (Debug) **DLL** (for every build configuration, for every project(multi project edit is supported in vs))
  * Tip: turn off 'Handle Warnings as Errors'
  * Tip: if you have a [problem](https://devtalk.nvidia.com/default/topic/977012/problems-when-building-physx-3-3-in-visual-studio-2015/) with the variable '\_\_pfnDliNotifyHook2', try making it const everywhere in the PhysX source code (2 occurences).
  
### Dynamic Libraries
The PostBuild Events that copy the required .dll's to the output folders makes use of this folder structure:  
VulkanoEngine  
└───AdditionalLibraries  
&emsp;&emsp;&emsp;└───PhysX  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;└───Binaries  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;├───Bin64_Debug  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;├───Bin64_Release  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;├───Bin_Debug  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;└───Bin_Release  
The PhysX binaries you built in the [Prerequisites](#prerequisites) step are located in:  
"$\PhysX-\<version nr.>\PhysXSDK\Bin\vc14win<32or64>".  
You need to copy them to their respective folder in the Additional Libraries.

### Building
If all the paths in the [above mentioned project properties](#how-to-build-for-windows) point to the correct folders, you are now ready to build the project and enjoy your self-built VulkanoEngine.
Still have problems and you can't seem to fix them? Don't hesitate to contact me.

## Acknowledgments
Hut Model by [Niels Breugelmans](https://www.artstation.com/nielsbreugelmans).
