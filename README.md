# ORB_SLAM2_Archaeonautic_Win64
See https://github.com/raulmur/ORB_SLAM2 for the Original


Tested using CMAKE GUI 3.11 + OpenCV 3.4.1 + Visual Studio 2017 on Windows 10 Pro x64
 
 **Required:**
 Git
 CMake
 OpenCV :  https://github.com/opencv/opencv/releases OR Thirdparty Folder 
 +contrib: https://github.com/opencv/opencv_contrib/releases
 Openh264: https://github.com/cisco/openh264/releases
1. DBoW2 --- Config/Generate
- !Release!
- DBoW2 project -> Properties -> General: Target Extension = .lib + Configuration Type = Static Library (.lib)
- C/C++ Tab -> Code Generation :  Runtime Library to Multi-threaded (/MT)
- Build ALL_BUILD.

2. g20 --- Config/Generate
- Same as DBoW2 
- C/C++ -> Preprocessor - Preprocessor Definitions,  add new line 'WINDOWS' (no underscore)
- Build ALL_BUILD.

3. Pangolin --- Config/Generate
- !Release!
- Build ALL_BUILD (1 fails)

4. orbslam --- Config/Generate
- Same as DBoW2/g2o
- Build ORB_SLAM2

5. Examples
- C/C++ Tab -> Code Generation :  Runtime Library to Multi-threaded (/MT)
build

6. Openh264
- Download "libopenh264-1.7.0-win64.dll"
- Move to System32
OR
-put it in System PATH
_________________________________________________________________________________________
-added Binary Support