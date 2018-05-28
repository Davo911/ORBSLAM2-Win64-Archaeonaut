#ifndef Datatransfer_h
#define Datatransfer_h

#include "UDPClient.h";
#include <mutex>
#include <unordered_set>

namespace ORB_SLAM2 {
    class System;
    class Map;
    class Frame;
    class KeyFrame;
    class MapPoint;
    class Tracking;
    
    class Datatransfer {
    public:
        enum TYPE {
            CURRENT_POSITION = 0,
            KEY_FRAMES = 1,
            MAP_POINTS = 2
        };
        Datatransfer(System* system, Map* map, Tracking* mpTracker);
        void Run();
        void RequestFinish();
        void RequestStop();
        bool isFinished();
        bool isStopped();
        
    private:
        bool Stop();
        bool CheckFinish();
        void SetFinish();
        void HandleCurrentPosition();
        void SendCurrentPositionToServer(const std::vector<float>& position);
        void HandleNewKeyFrames();
        void HandleNewMapPoints();
        void SendNewMapPointsToServer(const std::vector<std::vector<float>>& mapPoints);
        int FillHeader(unsigned char type, int length, char** header);
        std::vector<float> ExtractFrameInfo(Frame* frame);
        std::vector<float> ExtractKeyFrameInfo(KeyFrame* keyFrame);
        std::vector<float> ExtractMapPointInfo(MapPoint* mapPoint);
        
    private:
        bool mEnabledUDP;
        std::string mServerIP;
        std::string mServerPort;
        boost::asio::io_service ioService;
        std::mutex mUDP;
        UDPClient* udpClient;
        std::unordered_set<int> mKfs;
        std::unordered_set<int> mPts;
        System* mpSystem;
        Tracking* mpTracker;
        Map* mpMap;
        bool mbFinishRequested;
        bool mbFinished;
        std::mutex mMutexFinish;
        bool mbStopped;
        bool mbStopRequested;
        std::mutex mMutexStop;
    };
}

#endif
