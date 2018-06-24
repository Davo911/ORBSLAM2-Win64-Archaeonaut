mkdir .\output\OWN\
mkdir .\tmp\
mkdir .\output\OWN\MapSeq\
del /q .\tmp\*
del /q .\output\OWN\MapSeq\*

.\Examples\Release\mono_video.exe -c=".\Examples\Monocular\TUM1.yaml" -o=".\data\ORBvoc.bin" -f=".\output\OWN\own_" -v="C:\Users\David\Desktop\MOV_0144_hell.MP4"

ren KeyFrameTrajectory.txt OWNTrajectory.txt
move OWNTrajectory.txt .\output\OWN\
move Key.mkv .\output\OWN\
move .\tmp\* .\output\OWN\MapSeq\

