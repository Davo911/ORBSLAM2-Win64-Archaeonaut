mkdir .\output\TUM\
del /q .\tmp\*
del /q .\output\TUM\tmp\*

.\Examples\Monocular\Release\mono_tum.exe .\data\ORBvoc.bin .\Examples\Monocular\TUM1.yaml C:\Users\David\Desktop\TUM1_Seq\

ren KeyFrameTrajectory.txt TUMTrajectory.txt
move TUMTrajectory.txt .\output\TUM\
move Key.mkv .\output\TUM\
move .\tmp\* .\output\TUM\tmp\
