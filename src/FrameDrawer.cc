/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/

#include "FrameDrawer.h"
#include "Tracking.h"
#include "Color.h"
#include "Log.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include<mutex>


namespace ORB_SLAM2 {

    FrameDrawer::FrameDrawer(Map* pMap):mpMap(pMap) {
        mState=Tracking::SYSTEM_NOT_READY;
        mIm = cv::Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
        fps = 0.1;
    }




    cv::Mat FrameDrawer::DrawFrame(float width, float height, bool showKeypoints) {
        cv::Mat im;
        vector<cv::KeyPoint> vIniKeys; // Initialization: KeyPoints in reference frame
        vector<int> vMatches; // Initialization: correspondeces with reference keypoints
        vector<cv::KeyPoint> vCurrentKeys; // KeyPoints in current frame
        vector<bool> vbVO, vbMap; // Tracked MapPoints in current frame
        int state; // Tracking state

        //Copy variables within scoped mutex
        {
            unique_lock<mutex> lock(mMutex);
            state=mState;
            if (mState==Tracking::SYSTEM_NOT_READY)
                mState=Tracking::NO_IMAGES_YET;

            mIm.copyTo(im);

            switch (state) {
                case Tracking::NOT_INITIALIZED:
                    vCurrentKeys = mvCurrentKeys;
                    vIniKeys = mvIniKeys;
                    vMatches = mvIniMatches;
                    break;
                case Tracking::OK:
                    vCurrentKeys = mvCurrentKeys;
                    vbVO = mvbVO;
                    vbMap = mvbMap;
                    break;
                case Tracking::LOST:
                    vCurrentKeys = mvCurrentKeys;
                    break;
            }
        } // destroy scoped mutex -> release mutex

        if(im.channels()<3) //this should be always true
            cvtColor(im,im,CV_GRAY2BGR);

        switch (state) {
            case Tracking::NOT_INITIALIZED: {
                mMatchedKpCount = 0;
                for (auto i=0; i<vMatches.size(); i++) {
                    if (vMatches[i]>=0) {
                        mMatchedKpCount++;
                        cv::line(im, vIniKeys[i].pt, vCurrentKeys[vMatches[i]].pt, Color::Red);
                    }
                }
                if (showKeypoints) {
                    for (auto keypoint : vCurrentKeys) {
                        cv::circle(im, keypoint.pt, 3, Color::Cyan, -1);
                    }
                }
                break;
            }
            case Tracking::OK: {
                mnTracked=0;
                mnTrackedVO=0;
                const float r = 5;
                const int n = vCurrentKeys.size();
                for(auto i=0;i<n;i++) {
                    // tracked keypoints
                    if(vbVO[i] || vbMap[i]) {
                        cv::Point2f pt1,pt2;
                        pt1.x=vCurrentKeys[i].pt.x-r;
                        pt1.y=vCurrentKeys[i].pt.y-r;
                        pt2.x=vCurrentKeys[i].pt.x+r;
                        pt2.y=vCurrentKeys[i].pt.y+r;

                        // This is a match to a MapPoint in the map
                        if(vbMap[i]) {
                            cv::rectangle(im,pt1,pt2,Color::Green);
                            cv::circle(im,vCurrentKeys[i].pt,2,Color::Green,-1);
                            mnTracked++;
                            // This is match to a "visual odometry" MapPoint created in the last frame
                        } else {
                            cv::rectangle(im, pt1, pt2, Color::Blue);
                            cv::circle(im,vCurrentKeys[i].pt,2,Color::Blue,-1);
                            mnTrackedVO++;
                        }
                        // untracked keypoints
                    } else if (showKeypoints) {
                        cv::circle(im,vCurrentKeys[i].pt,3,Color::Cyan,-1);
                    }
                }
                break;
            }
            case Tracking::LOST: {
                if (showKeypoints) {
                    for (auto keypoint : vCurrentKeys) {
                        cv::circle(im,keypoint.pt,3,Color::Cyan,-1);
                    }
                }
                break;
            }
        }

        // scale image to given size
        auto w = im.cols;
        auto h = im.rows;
        auto scaleFactor = ((width / w) + (height / h)) / 2.f;
        cv::resize(im, im, cv::Size(static_cast<int>(w * scaleFactor), static_cast<int>(h * scaleFactor)));

        cv::Mat imWithInfo;
        DrawTextInfo(im,state, imWithInfo);

        return imWithInfo;
    }

    void FrameDrawer::UpdateFPS(double frames_per_second){
        fps=frames_per_second;

    }
    void FrameDrawer::DrawTextInfo(cv::Mat &im, int nState, cv::Mat &imText) {
        stringstream s;
        //double fps = mpSystem->GetFps()

        switch (nState) {
            case Tracking::NO_IMAGES_YET: {
                s << " WAITING FOR IMAGES";
                break;
            }
            case Tracking::NOT_INITIALIZED: {
                s << " TRYING TO INITIALIZE |  "
                  << "Fps: " <<  fps
                  << ", KPs: " << N
                  << ", Matches: " << mMatchedKpCount;
                Log::write(__FUNCTION__, "matches=", mMatchedKpCount);
                break;
            }
            case Tracking::OK: {
                s << (mbOnlyTracking) ? "LOCALIZATION | " : "SLAM MODE |  ";
                int nKFs = mpMap->KeyFramesInMap();
                int nMPs = mpMap->MapPointsInMap();
                s << "KFs: " << nKFs
                  << ", Fps: " <<  fps
                  << ", MPs: " << nMPs
                  << ", KPs: " << N
                  << ", Matches: " << mnTracked;
                Log::write(__FUNCTION__, "matches=", mnTracked);
                if (mnTrackedVO>0)
                    s << ", + VO matches: " << mnTrackedVO;
                break;
            }
            case Tracking::LOST: {
                s << " TRACK LOST. TRYING TO RELOCALIZE |  "
                  << "Fps: " <<  fps
                  << ", KPs: " << N;
                break;
            }
            case Tracking::SYSTEM_NOT_READY: {
                s << " LOADING ORB VOCABULARY. PLEASE WAIT...";
                break;
            }
        }

        int baseline=0;
        cv::Size textSize = cv::getTextSize(s.str(),cv::FONT_HERSHEY_PLAIN,1,1,&baseline);
        imText = cv::Mat(im.rows+textSize.height+10,im.cols,im.type());
        im.copyTo(imText.rowRange(0,im.rows).colRange(0,im.cols));
        imText.rowRange(im.rows,imText.rows) = cv::Mat::zeros(textSize.height+10,im.cols,im.type());
        cv::putText(imText,s.str(),cv::Point(5,imText.rows-5),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,255),1,8);
    }


    void FrameDrawer::Update(Tracking *pTracker) {
        unique_lock<mutex> lock(mMutex);
        pTracker->mImGray.copyTo(mIm);
        mvCurrentKeys=pTracker->mCurrentFrame.mvKeys;
        N = mvCurrentKeys.size();
        mvbVO = vector<bool>(N,false);
        mvbMap = vector<bool>(N,false);
        mbOnlyTracking = pTracker->mbOnlyTracking;

        if(pTracker->mLastProcessedState==Tracking::NOT_INITIALIZED)
        {
            mvIniKeys=pTracker->mInitialFrame.mvKeys;
            mvIniMatches=pTracker->mvIniMatches;
        }
        else if(pTracker->mLastProcessedState==Tracking::OK)
        {
            for(int i=0;i<N;i++)
            {
                MapPoint* pMP = pTracker->mCurrentFrame.mvpMapPoints[i];
                if(pMP)
                {
                    if(!pTracker->mCurrentFrame.mvbOutlier[i])
                    {
                        if(pMP->Observations()>0)
                            mvbMap[i]=true;
                        else
                            mvbVO[i]=true;
                    }
                }
            }
        }
        mState=static_cast<int>(pTracker->mLastProcessedState);
    }

} //namespace ORB_SLAM
