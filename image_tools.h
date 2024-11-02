#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H
#include <opencv2/opencv.hpp>
#include <QImage>
namespace Tools {
    cv::Mat qImageToCvMat( const QImage &inImage);
    QImage cvMatToQImage(cv::Mat& src);
}



#endif // IMAGE_TOOLS_H
