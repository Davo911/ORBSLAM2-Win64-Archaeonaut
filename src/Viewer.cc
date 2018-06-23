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

#include "Viewer.h"
#include "Settings.h"
#include <pangolin/pangolin.h>

#include <mutex>

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include <chrono>
namespace ORB_SLAM2
{

Viewer::Viewer(System* pSystem, FrameDrawer *pFrameDrawer, MapDrawer *pMapDrawer, Tracking *pTracking):
    mpSystem(pSystem), mpFrameDrawer(pFrameDrawer),mpMapDrawer(pMapDrawer), mpTracker(pTracking),
    mbFinishRequested(false), mbFinished(true), mbStopped(true), mbStopRequested(false) {
    cv::FileStorage fSettings(Settings::path, cv::FileStorage::READ);
    float fps =        (0.f == static_cast<float>(fSettings["Camera.fps"]))             ? 30.f   : fSettings["Camera.fps"];
    mT = 1e3/fps;
    mShowImage =       (static_cast<int>(fSettings["Viewer.showImage"]) == 1);
    mImageWidth =      (0.f == static_cast<float>(fSettings["Viewer.ImageWidth"]))      ? 640.f  : fSettings["Viewer.ImageWidth"];
    mImageHeight =     (0.f == static_cast<float>(fSettings["Viewer.ImageHeight"]))     ? 480.f  : fSettings["Viewer.ImageHeight"];
    mImagePosX =       (0 == static_cast<int>(fSettings["Viewer.ImagePosX"]))           ? 0      : fSettings["Viewer.ImagePosX"];
    mImagePosY =       (0 == static_cast<int>(fSettings["Viewer.ImagePosY"]))           ? 0      : fSettings["Viewer.ImagePosY"];
    mMapViewerWidth =  (0.f == static_cast<float>(fSettings["Viewer.MapViewerWidth"]))  ? 640.f  : fSettings["Viewer.MapViewerWidth"];
    mMapViewerHeight = (0.f == static_cast<float>(fSettings["Viewer.MapViewerHeight"])) ? 480.f  : fSettings["Viewer.MapViewerHeight"];
    mViewpointX =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointX"]))      ? 0.f    : fSettings["Viewer.ViewpointX"];
    mViewpointY =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointY"]))      ? -10.f  : fSettings["Viewer.ViewpointY"];
    mViewpointZ =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointZ"]))      ? -0.1f  : fSettings["Viewer.ViewpointZ"];
    mViewpointF =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointF"]))      ? 2000.f : fSettings["Viewer.ViewpointF"];
}

Viewer::Viewer(System* pSystem, FrameDrawer *pFrameDrawer, MapDrawer *pMapDrawer, Tracking *pTracking,const string &strSettingPath): //# const string nur mit angenommen wegen Rauol wird im globalen objekt setting behandelt 
    mpSystem(pSystem), mpFrameDrawer(pFrameDrawer),mpMapDrawer(pMapDrawer), mpTracker(pTracking),
    mbFinishRequested(false), mbFinished(true), mbStopped(true), mbStopRequested(false) {
    cv::FileStorage fSettings(Settings::path, cv::FileStorage::READ);
    float fps =        (0.f == static_cast<float>(fSettings["Camera.fps"]))             ? 30.f   : fSettings["Camera.fps"];
    mT = 1e3/fps;
    mShowImage =       (static_cast<int>(fSettings["Viewer.showImage"]) == 1);
    mImageWidth =      (0.f == static_cast<float>(fSettings["Viewer.ImageWidth"]))      ? 640.f  : fSettings["Viewer.ImageWidth"];
    mImageHeight =     (0.f == static_cast<float>(fSettings["Viewer.ImageHeight"]))     ? 480.f  : fSettings["Viewer.ImageHeight"];
    mMapViewerWidth =  (0.f == static_cast<float>(fSettings["Viewer.MapViewerWidth"]))  ? 640.f  : fSettings["Viewer.MapViewerWidth"];
    mMapViewerHeight = (0.f == static_cast<float>(fSettings["Viewer.MapViewerHeight"])) ? 480.f  : fSettings["Viewer.MapViewerHeight"];
    mViewpointX =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointX"]))      ? 0.f    : fSettings["Viewer.ViewpointX"];
    mViewpointY =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointY"]))      ? -10.f  : fSettings["Viewer.ViewpointY"];
    mViewpointZ =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointZ"]))      ? -0.1f  : fSettings["Viewer.ViewpointZ"];
    mViewpointF =      (0.f == static_cast<float>(fSettings["Viewer.ViewpointF"]))      ? 2000.f : fSettings["Viewer.ViewpointF"];

    init = true;

    // OutputVideoKey =   (" " == static_cast<string>(fSettings["Output.Video.Map"]))      ? "".f : fSettings["Output.Video.Map"];
    // OutputVideoMap = ;
}


void Viewer::Run() {

    mbFinished = false;
    mbStopped = false;
    if (init)
    {
        videoKey.open("Key.mkv",CV_FOURCC('X','2','6','4'),30, cv::Size(mImageWidth,mImageHeight+20),true);
        init=false;
        cout << "Eingelesen" << endl;
    }
  


    pangolin::CreateWindowAndBind("ORB-SLAM2: Map Viewer",mMapViewerWidth,mMapViewerHeight);

    // 3D Mouse handler requires depth testing to be enabled
    glEnable(GL_DEPTH_TEST);

    // Issue specific OpenGl we might need
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


 pangolin::CreatePanel("menu").SetBounds(0.0,1.0,0.0,pangolin::Attach::Pix(175));
    pangolin::Var<bool> menuFollowCamera("menu.Follow Camera",true,true);
    pangolin::Var<bool> menuShowPoints("menu.Show Points",true,true);
    pangolin::Var<bool> menuShowKeyFrames("menu.Show KeyFrames",true,true);
    pangolin::Var<bool> menuShowGraph("menu.Show Graph",true,true);
	pangolin::Var<bool> menuShowKeypoints("menu.Show Keypoints", true, true);
    pangolin::Var<bool> menuLocalizationMode("menu.Localization Mode",false,true);
    pangolin::Var<bool> menuReset("menu.Reset",false,false);

    // Define Camera Render Object (for view / scene browsing)
    pangolin::OpenGlRenderState s_cam(
                 pangolin::ProjectionMatrix(mMapViewerWidth, mMapViewerHeight,
                                           mViewpointF, mViewpointF,
                                           mMapViewerWidth/2, mMapViewerHeight/2,
                                           0.1,1000),
                pangolin::ModelViewLookAt(mViewpointX, mViewpointY, mViewpointZ,
                                          0,0,0,0.0,-1.0, 0.0)
                );

    // Add named OpenGL viewport to window and provide 3D Handler
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -mMapViewerWidth / mMapViewerHeight)
            .SetHandler(new pangolin::Handler3D(s_cam));

    pangolin::OpenGlMatrix Twc;
    Twc.SetIdentity();

    cv::namedWindow("ORB-SLAM2: Current Frame");

    bool bFollow = true;
    bool bLocalizationMode = false;

    //Recording the MapViewer
    //pangolin::DisplayBase().RecordOnRender("ffmpeg:[fps=50,bps=243886080,unique_filename]//Map.mp4");
	        //Results in Deadlock
   

    int countpix = 0;
    auto last = std::chrono::steady_clock::now();
	string fname = "./tmp/map";
    while(1)
    {
        //save every frame of MapViewer seperate
		std::chrono::steady_clock::duration time_span = std::chrono::steady_clock::now() - last;
		double nseconds = double(time_span.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
        if(nseconds >= (1/30)){
			
            last = std::chrono::steady_clock::now();
            d_cam.SaveOnRender(fname + std::to_string(countpix));
			countpix++;
        }
   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mpMapDrawer->GetCurrentOpenGLCameraMatrix(Twc);

        if(menuFollowCamera && bFollow)
        {
            s_cam.Follow(Twc);
        }
        else if(menuFollowCamera && !bFollow)
        {
            s_cam.SetModelViewMatrix(pangolin::ModelViewLookAt(mViewpointX, mViewpointY, mViewpointZ,
                                                               0,0,0,0.0,-1.0, 0.0));
            s_cam.Follow(Twc);
            bFollow = true;
        }
        else if(!menuFollowCamera && bFollow)
        {
            bFollow = false;
        }

        if(menuLocalizationMode && !bLocalizationMode)
        {
            mpSystem->ActivateLocalizationMode();
            bLocalizationMode = true;
        }
        else if(!menuLocalizationMode && bLocalizationMode)
        {
            mpSystem->DeactivateLocalizationMode();
            bLocalizationMode = false;
        }

        d_cam.Activate(s_cam);
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        mpMapDrawer->DrawCurrentCamera(Twc);
        if(menuShowKeyFrames || menuShowGraph)
            mpMapDrawer->DrawKeyFrames(menuShowKeyFrames,menuShowGraph);
        if(menuShowPoints)
            mpMapDrawer->DrawMapPoints();
        
        pangolin::FinishFrame();

        mShowImage = 1;
        if (mShowImage) {
            cv::Mat im = mpFrameDrawer->DrawFrame(mImageWidth, mImageHeight, menuShowKeypoints);
            auto winName = "ORB-SLAM2: Current Frame";


            videoKey << im;
            cv::imshow(winName, im);
            


            // Wofür Fenster-pos sperren?
            //cv::moveWindow(winName, mImagePosX, mImagePosY);
            cv::waitKey(mT);
           //videoKey.release();

        }

        if(menuReset)
        {
            menuShowGraph = true;
            menuShowKeyFrames = true;
            menuShowPoints = true;
            menuLocalizationMode = false;
            if(bLocalizationMode)
                mpSystem->DeactivateLocalizationMode();
            bLocalizationMode = false;
            bFollow = true;
            menuFollowCamera = true;
            mpSystem->Reset();
            menuReset = false;
        }

        if(Stop())
        {
            while(isStopped())
            {
                usleep(3000);
            }
        }

        if(CheckFinish())
            {
            videoKey.release();

            break;
            }

            

    }

    SetFinish();
}

void Viewer::RequestFinish()
{
    unique_lock<mutex> lock(mMutexFinish);
    mbFinishRequested = true;
}

bool Viewer::CheckFinish()
{
    unique_lock<mutex> lock(mMutexFinish);
    return mbFinishRequested;
}

void Viewer::SetFinish()
{
    unique_lock<mutex> lock(mMutexFinish);
    mbFinished = true;
}

bool Viewer::isFinished()
{
    
    unique_lock<mutex> lock(mMutexFinish);
    return mbFinished;
}

void Viewer::RequestStop()
{
    unique_lock<mutex> lock(mMutexStop);
    if(!mbStopped)
        mbStopRequested = true;
}

bool Viewer::isStopped()
{
    unique_lock<mutex> lock(mMutexStop);
    return mbStopped;
}

bool Viewer::Stop()
{
    unique_lock<mutex> lock(mMutexStop);
    unique_lock<mutex> lock2(mMutexFinish);

    if(mbFinishRequested)
        return false;
    else if(mbStopRequested)
    {
        mbStopped = true;
        mbStopRequested = false;
        return true;
    }

    return false;

}

void Viewer::Release()
{
    unique_lock<mutex> lock(mMutexStop);

    mbStopped = false;
}

}
