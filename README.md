# ORB_SLAM2_Windows

 Built using CMAKE GUI 3.11 + OpenCV 3.4.1 + Visual Studio 2017 on Windows 10 x64
 
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
_________________________________________________________________________________________
-added Binary Support