# VulkanoEngine 1.0
## Demo
I included a demo executable for 64-bit Windows. You only need the folder Executables, easiest way is to just clone this repo.
Then run the VulkanoEngine.exe inside the Executables folder and watch the console window for instructions.
### Instructions
* Use WASD + Hold and move Mouse to navigate in the scene.
* Have an AZERTY keyboard? Press 'P' to toggle to ZQSD controls and back.
* Press 'N'/'M' to go the the next/previous scene.
* Press 'G\' to TOGGLE Debug Visuals.
* Press \'F\' to TOGGLE the FPS counter in the Console Window.

## Project Goals
TODO

## How To Build For Windows
Any system paths specified are just recommendations
If you don't use the recommended paths/install directories or use another library version, be sure to change the project properties accordingly for every desired build configuration. Properties to change:
* C/C++: Additional Include Directories
* Linker: Additional Library Directories
* Build Events: PostBuild Event

### Prerequisuites
* Vulkan Runtime Libraries (Is probably already installed on your pc)
* Visual Studio
* Vulkan SDK (Tested with version 1.0.49.0, directory="C:\VulkanSDK")
  * Comes with GLM library (directory="C:\VulkanSDK\1.0.49.0\Third-Party\Include\glm")
* GLFW binaries (Tested with version 3.2.1, directory="C:\Libraries\glfw")
* PhysX SDK (Tested with version 3.3, directory="C:\Libraries\PhysX-3.3")
  * Nvidia doesn't offer pre-built .dll's or .libs, so you will need to build them yourself. First request acces to [NVIDIAGameWorks](https://developer.nvidia.com/what-is-gameworks) then just follow the guidelines on their readme.
  * IMPORTANT: In the PhysX solution, change Code Generation to Multi-threaded (Debug) **DLL** (for every build configuration, for every project(multi project edit is supported in vs))
  * [Problems and Solutions](TODO-TODO-TODO-TODO) I had.
  
### Dynamic Libraries
The PhysX dll's we need next to the executable get copied to the output folder in a 'PostBuild Event'.
For this to work you need to make [this folder structure](TODO-TODO-TODO-TODO). The files inside are binaries you built in the [Prerequisuites](Prerequisuites) step, located in "$\PhysX-**<version-nr>**\PhysXSDK\Bin\vc14win**<32or64>**".

### Building
If all the paths in the above mentioned properties point to the correct folders, you are now ready to press F7 and enjoy your self built VulkanoEngine.
