#ifndef Color_h
#define Color_h


#include <opencv2/opencv.hpp>


namespace ORB_SLAM2 {
    class Color {
    public:
        Color(Color const&) = delete;             // Copy construct
        Color(Color&&) = delete;                  // Move construct
        Color& operator=(Color const&) = delete;  // Copy assign
        Color& operator=(Color &&) = delete;      // Move assign
        
        static const cv::Scalar Black;
        static const cv::Scalar White;
        static const cv::Scalar Blue;
        static const cv::Scalar Green;
        static const cv::Scalar Red;
        static const cv::Scalar Cyan;
        static const cv::Scalar Yellow;
        static const cv::Scalar Magenta;
    };
}

#endif
