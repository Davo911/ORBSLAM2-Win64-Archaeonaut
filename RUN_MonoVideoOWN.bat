mkdir .\output\OWN
.\Examples\Release\mono_video.exe -c=".\Examples\Monocular\TUM1.yaml" -o=".\data\ORBvoc.bin" -f=".\output\OWN\own_" -v="C:\Users\David\Desktop\MOV_0144_hell.MP4"
move /Y KeyFrameTrajectory.txt .\output\OWN\
move /Y Key.mkv .\output\OWN\
