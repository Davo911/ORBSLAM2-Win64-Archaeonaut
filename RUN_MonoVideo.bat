mkdir .\output\mondsee
.\Examples\Release\mono_video.exe -c=".\data\mondsee_4K.yaml" -o=".\data\ORBvoc.bin" -f=".\output\mondsee\mond4k_" -v="C:\Users\David\Desktop\GP012691.MP4"
move /Y KeyFrameTrajectory.txt .\output\mondsee\
move /Y Key.mkv .\output\mondsee\
