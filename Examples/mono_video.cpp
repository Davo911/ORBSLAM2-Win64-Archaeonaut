#include "System.h"
#include "Tracking.h"
#include "CSV.h"
#include "Log.h"
#include "Settings.h"
#include "Color.h"
#include <iostream>
#include <future>
#include <thread>
#include <chrono>
//#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>


using namespace std;
using namespace cv;
using namespace ORB_SLAM2;
int sockfd;

template <typename T>
int processSLAM(T input, System *slamPtr, int start, int end, int skipInit, int skipLoc, int skipTrack, string exportPath, bool webcam);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    CommandLineParser parser(argc, argv,
                             "{o  | <none> | -o=<path to ORB vocabulary>]}"
                                     "{c  | <none> | -c=<path to config yaml>]}"
                                     "{b  | 0      | -b=<begin frame>]}"
                                     "{e  | 0      | -e=<end frame>]}"
                                     "{si | 0      | -si=<skip frame at initialization>]}"
                                     "{st | 0      | -st=<skip frame at tracking>]}"
                                     "{sl | 0      | -sl=<skip frame at localization>]}"
                                     "{v  |        | -v=<path to video>]}"
                                     "{f  | <none> | -f=<path for export files>]}"
                                     "{w  | -1     | -w=<webcam>]}"
                                     "{na | -1     | -na=Network Adresse vom Server}"
                                     "{np | -1     | -np=Network Port vom Server}");
    //bcv::FileStorage fSettings(Settings::path, cv::FileStorage::READ);
    //auto vocabulary =      (0 == static_cast<string>(fSettings["OrbDirectory.Path"])) ? "../data/ORBvoc.bin"   : fSettings["OrbDirectory.Path"];
    //filteringFactor =      (0.0 == static_cast<float>(fSettings["Keypoint.Offset.Factor"])) ? 0.0              : fSettings["Keypoint.Offset.Factor"];

    auto config = parser.get<string>("c");
    auto vocabulary = parser.get<string>("o");
    auto video = parser.get<string>("v");
    auto exportPath = parser.get<string>("f");
    auto webcam = parser.get<int>("w");
    auto startFrame = parser.get<int>("b");
    auto endFrame = parser.get<int>("e");
    auto skipInit = parser.get<int>("si");
    auto skipLoc = parser.get<int>("sl");
    auto skipTrack = parser.get<int>("st");
    auto NetworkAdress = parser.get<string>("na");
    auto NetworkPort = parser.get<int>("np");
    if (!parser.check()) {
        parser.printErrors();
        return -1;
    }

    // write stdout into file
    //freopen(string(exportPath + "ORB_SLAM2_log.txt").c_str(), "w", stdout);

    System SLAM(vocabulary, config, System::MONOCULAR, true);
    future<int> resultFuture;

/*    if (webcam == -1) {
        resultFuture = async(launch::async, processSLAM<string>, video, &SLAM, startFrame, endFrame, skipInit, skipLoc, skipTrack, exportPath,false,NetworkAdress,NetworkPort);
    } else {
        resultFuture = async(launch::async, processSLAM<int>, webcam, &SLAM, 0, 0, skipInit, skipLoc, skipTrack, exportPath,true,NetworkAdress,NetworkPort);
    }*/
    if (webcam == -1) {
        resultFuture = async(launch::async, processSLAM<string>, video, &SLAM, startFrame, endFrame, skipInit, skipLoc, skipTrack, exportPath,false);
    } else {
        resultFuture = async(launch::async, processSLAM<int>, webcam, &SLAM, 0, 0, skipInit, skipLoc, skipTrack, exportPath,true);
    }

    //#SLAM.RunViewer(); 
    return resultFuture.get();
}


/* Bommel hat versucht einen TCP Client zu inplementieren

int Client(string NetworkAdress, int NetworkPort)
{
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    portno = NetworkPort;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(NetworkAdress);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
}

String ReadSocket() {

        n = read(sockfd,buffer,2096);
        String a = new String(buffer);
        if (n < 0)
            error("ERROR reading from socket");
        //Eventueller Rückkanal
    }
}

int WriteSocket(string message) {

        char sendbuffer[2096];
        strcpy(sendbuffer, message.c_str());
        n = write(sockfd, sendbuffer, strlen(sendbuffer));
        if (n < 0)
            error("ERROR writing to socket");

}
*/

template <typename T>
//int processSLAM(T input, System *slamPtr, int start, int end, int skipInit, int skipLoc, int skipTrack, string exportPath,bool webcam, string NetworkAdress, int NetworkPort) {
int processSLAM(T input, System *slamPtr, int start, int end, int skipInit, int skipLoc, int skipTrack, string exportPath,bool webcam) {


    System& SLAM = *slamPtr;
    //CSV& csv = CSV::getInstance(exportPath + "stats.csv");
    const std::string url = "udp://@:8554";
    /*VideoCapture cap(url); Hier wird versucht den Videostream von der Gopro direkt zu cappen
    
    if (webcam)
    {
		VideoCapture cap(url);
    }
    else{
		VideoCapture cap(input);
	}*/

    VideoCapture cap(input);
    if (!cap.isOpened()) {
        cerr << "Failed opening video file: " << input << endl;
        SLAM.Shutdown();
        return -1;
    }

    auto frameCount = (end == 0) ? cap.get(CAP_PROP_FRAME_COUNT) : end;
    auto tframe = 0.0;
    Mat frame;

    if (start > 0 && start < frameCount) {
        cap.set(CAP_PROP_POS_FRAMES, start);
    }
    double TimeDur;
    auto TimeStart = chrono::steady_clock::now();
    bool firststart = true;
    for(auto i=start; i<frameCount ;i++) {

        if (firststart)
        {
            auto TimeStart = chrono::steady_clock::now();
            firststart = false;
        }
        auto TimeAct = chrono::steady_clock::now();
        TimeDur = (duration_cast<chrono::milliseconds>(TimeAct - TimeStart).count()/1000);

        cap >> frame;

        cout << endl << endl;
        Log::write(__FUNCTION__, "###############################################");
        Log::write(__FUNCTION__, "frame=", i);



        if (SLAM.GetTrackingState() == Tracking::eTrackingState::NOT_INITIALIZED
            && !SLAM.LocalizationOnly()
            && skipInit > 1
            && i%skipInit != 0) {
            // for initialization
            Log::write(__FUNCTION__, "init_skip=", i);
            continue;
        } else if (SLAM.GetTrackingState() == Tracking::eTrackingState::OK
                   && SLAM.LocalizationOnly()
                   && skipLoc > 1
                   && i%skipLoc != 0) {
            // for localization
            Log::write(__FUNCTION__, "loc_skip=", i);
            continue;
        } else if (SLAM.GetTrackingState() == Tracking::eTrackingState::OK
                   && !SLAM.LocalizationOnly()
                   && skipTrack > 1
                   && i%skipTrack == 0) {
            // for tracking
            Log::write(__FUNCTION__, "tracking_skip=", i);
            continue;
        }
        double fps = (((int)(((i/TimeDur)*10+0.5)))/10.0);
        if (!frame.empty()) {
            //csv.write(true, true, i, tframe);
            auto t1 = chrono::steady_clock::now();
            SLAM.TrackMonocular(frame, tframe,fps);
            auto t2 = chrono::steady_clock::now();
            auto t = duration_cast<chrono::milliseconds>(t2 - t1).count();
            Log::write(__FUNCTION__, "tracking_duration=", t);
        }


        Log::write(__FUNCTION__, "Sekunden=", TimeDur);
        Log::write(__FUNCTION__, "FPS=", fps);
        //Log::write(__FUNCTION__, "FPM=", fpm);

    }

    SLAM.Shutdown();
	Log::write(__FUNCTION__, "[SLAM System Shutted Down]");

    //Save resulting Map and track data
    auto resultMap = async(launch::async, &System::SaveMapOBJ, &SLAM, exportPath + "map.obj");
    auto resultTrack = async(launch::async, &System::SaveKeyFrameTrajectoryOBJ, &SLAM, exportPath + "track.obj");
    auto resultTrack2 = async(launch::async, &System::SaveKeyFrameTrajectoryCSV, &SLAM, exportPath + "track.csv");

    resultMap.get();
    resultTrack.get();
    resultTrack2.get();

    return 0;
}
