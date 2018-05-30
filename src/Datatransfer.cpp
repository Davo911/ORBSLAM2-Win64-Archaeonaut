#include "Datatransfer.h"
#include "Converter.h"
#include "System.h"
#include "Tracking.h"
#include "Map.h"
#include "Log.h"
#include "Settings.h"

#include <mutex>
#include <thread>
#include <chrono>

namespace ORB_SLAM2 {
    
    using namespace std;
    
    
    Datatransfer::Datatransfer(System* system, Map* map, Tracking* tracker) : mpSystem(system), mpMap(map), mpTracker(tracker),
                                                                                mbFinishRequested(false), mbFinished(false),
                                                                                mbStopRequested(false), mbStopped(false) {
        cv::FileStorage fSettings(Settings::path, cv::FileStorage::READ);
        mEnabledUDP = (1 == static_cast<int>(fSettings["Data.shouldTransfer"]));
        mServerIP =   ("" == static_cast<string>(fSettings["Data.ServerIP"])) ? "127.0.0.1" : static_cast<string>(fSettings["Data.ServerIP"]);
        mServerPort = ("" == static_cast<string>(fSettings["Data.Port"]))     ? "7778"      : static_cast<string>(fSettings["Data.Port"]);
                                                                                    
        udpClient = new UDPClient(ioService, mServerIP, mServerPort);
    }

    
    void Datatransfer::Run() {
        while (true) {
            this_thread::sleep_for(chrono::milliseconds(1000));

            if (Stop()) { while (isStopped()) { this_thread::sleep_for(chrono::milliseconds(3)); } }
            if (CheckFinish()) { break; }
            
            if (mpSystem->GetTrackingState() != Tracking::eTrackingState::OK) { continue; }
            
            HandleCurrentPosition();
            HandleNewKeyFrames();
            HandleNewMapPoints();
            
            // after global bundle adjustment or loop closure
            if (mpSystem->MapChanged()) {
                Log::write(__FUNCTION__, "DATATRANSFER MAP UPDATE");
            }
        }
        
        SetFinish();
    }
    
    
    void Datatransfer::HandleCurrentPosition() {
        auto currentPosition = ExtractFrameInfo(&mpTracker->mCurrentFrame);
        if (mEnabledUDP) {
            Log::write(__FUNCTION__, "DATATRANSFER CURRENT POSITION:",currentPosition[0],currentPosition[1],currentPosition[2]);
            Log::write(__FUNCTION__, "DATATRANSFER CURRENT ROTATION:",currentPosition[3],currentPosition[4],currentPosition[5]);
            SendCurrentPositionToServer(currentPosition);
        }
    }
    
    
    void Datatransfer::SendCurrentPositionToServer(const vector<float>& position) {
        char* header;
        auto header_len = FillHeader(TYPE::KEY_FRAMES, 1, &header);
        char const *data_serialized = reinterpret_cast<char const *>(position.data());
        const auto data_len = position.size()*4;
        const auto packet_len = header_len + data_len;
        char packet[packet_len];
        char* packet_p = packet;
        
        // header into packet
        memcpy(packet_p, header, header_len);
        // data into packet
        packet_p += header_len;
        memcpy(packet_p, data_serialized, data_len);
        
        unique_lock<mutex> lock(mUDP);
        udpClient->send(packet, packet_len);
        
        delete header;
    }
    
    
    void Datatransfer::HandleNewKeyFrames() {
        auto keyFrames = mpMap->GetAllKeyFrames();
        vector<vector<float>> newKeyFrames;
        
        if (mKfs.empty()) {
            for (auto keyFrame : keyFrames) {
                if (keyFrame->isBad()) { continue; }
                mKfs.insert(keyFrame->mnId);
                newKeyFrames.push_back(ExtractKeyFrameInfo(keyFrame));
            }
        } else {
            for (auto keyFrame : keyFrames) {
                if (mKfs.find(keyFrame->mnId) == mKfs.end()) {
                    if (keyFrame->isBad()) { continue; }
                    mKfs.insert(keyFrame->mnId);
                    newKeyFrames.push_back(ExtractKeyFrameInfo(keyFrame));
                }
            }
        }
        
        if (newKeyFrames.size() > 0 && mEnabledUDP) {
            // send UDP datagram
            Log::write(__FUNCTION__, "DATATRANSFER NEW KEYFRAMES:", newKeyFrames.size());
        }
    }
    
    
    void Datatransfer::HandleNewMapPoints() {
        vector<vector<float>> newMapPoints;
        auto mapPoints = mpMap->GetAllMapPoints();
        
        if (mPts.empty()) {
            for (auto mapPoint : mapPoints) {
                if (mapPoint->isBad()) { continue; }
                mPts.insert(mapPoint->mnId);
                newMapPoints.push_back(ExtractMapPointInfo(mapPoint));
            }
        } else {
            for (auto mapPoint : mapPoints) {
                if (mPts.find(mapPoint->mnId) == mPts.end()) {
                    if (mapPoint->isBad()) { continue; }
                    mPts.insert(mapPoint->mnId);
                    newMapPoints.push_back(ExtractMapPointInfo(mapPoint));
                }
            }
        }
        
        if (newMapPoints.size() > 0 && mEnabledUDP) {
            Log::write(__FUNCTION__, "DATATRANSFER NEW MAPPOINTS:", newMapPoints.size());
            SendNewMapPointsToServer(newMapPoints);
        }
    }
    
    
    void Datatransfer::SendNewMapPointsToServer(const vector<vector<float>>& mapPoints) {
        auto max = min((size_t)600, mapPoints.size());
        
        char* header;
        auto header_len = FillHeader(TYPE::MAP_POINTS, max, &header);
        const auto data_len = max*3*4;
        const auto packet_len = header_len + data_len;
        char packet[packet_len];
        char* _packet = packet;
        
        // header into packet
        memcpy(_packet, header, header_len);
        _packet += header_len;
        
        // data into packet
        auto point_len = 3*4;
        for (auto i = 0; i < max; i++) {
            char const *data_serialized = reinterpret_cast<char const *>(mapPoints[i].data());
            memcpy(_packet, data_serialized, point_len);
            _packet += point_len;
        }
        
        unique_lock<mutex> lock(mUDP);
        udpClient->send(packet, packet_len);
        
        delete header;
    }
    
    
    int Datatransfer::FillHeader(unsigned char type, int length, char** header) {
        const int header_size = 5;
        char* _header = new char[header_size];
        _header[0] = type;
        _header[1] = (char) (length & 0x000000ff);
        _header[2] = (char)((length & 0x0000ff00) >> 8);
        _header[3] = (char)((length & 0x00ff0000) >> 16);
        _header[4] = (char)((length & 0xff000000) >> 24);
        *header = _header;
        return header_size;
    }
    
    
    void Datatransfer::RequestFinish() {
        unique_lock<mutex> lock(mMutexFinish);
        mbFinishRequested = true;
    }
    
    
    bool Datatransfer::CheckFinish() {
        unique_lock<mutex> lock(mMutexFinish);
        return mbFinishRequested;
    }
    
    
    void Datatransfer::SetFinish() {
        unique_lock<mutex> lock(mMutexFinish);
        mbFinished = true;
    }
    
    
    bool Datatransfer::isFinished() {
        unique_lock<mutex> lock(mMutexFinish);
        return mbFinished;
    }
    
    
    void Datatransfer::RequestStop() {
        unique_lock<mutex> lock(mMutexStop);
        if(!mbStopped)
            mbStopRequested = true;
    }
    
    
    bool Datatransfer::isStopped() {
        unique_lock<mutex> lock(mMutexStop);
        return mbStopped;
    }
    
    
    bool Datatransfer::Stop() {
        unique_lock<mutex> lock(mMutexStop);
        unique_lock<mutex> lock2(mMutexFinish);
        
        if (mbFinishRequested) {
            return false;
        } else if (mbStopRequested) {
            mbStopped = true;
            mbStopRequested = false;
            return true;
        }
        
        return false;
    }
    
    
    vector<float> Datatransfer::ExtractFrameInfo(Frame* frame) {
        auto info = Converter::toPosition(frame->GetCameraCenter());
        auto R = Converter::toQuaternion(frame->GetRotation().t());
        info.insert(std::end(info), std::begin(R), std::end(R));
        return info;
    }
    
    
    vector<float> Datatransfer::ExtractKeyFrameInfo(KeyFrame* keyFrame) {
        auto info = Converter::toPosition(keyFrame->GetCameraCenter());
        auto R = Converter::toQuaternion(keyFrame->GetRotation().t());
        info.insert(std::end(info), std::begin(R), std::end(R));
        return info;
    }
    
    
    vector<float> Datatransfer::ExtractMapPointInfo(MapPoint* mapPoint) {
        auto info = Converter::toPosition(mapPoint->GetWorldPos());
        return info;
    }
    
}
