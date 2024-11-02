#include "image_tools.h"

namespace Tools {
//    cv::Mat qImageToCvMat(const QImage &image)
//    {
//        cv::Mat mat;
//        switch(image.format())
//        {
//        case QImage::Format_Grayscale8: // 灰度图，每个像素点1个字节（8位）
//            // Mat构造：行数，列数，存储结构，数据，step每行多少字节
//            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
//            break;
//        case QImage::Format_ARGB32: // uint32存储0xAARRGGBB，pc一般小端存储低位在前，所以字节顺序就成了BGRA
//        case QImage::Format_RGB32: // Alpha为FF
//        case QImage::Format_ARGB32_Premultiplied:
//            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
//            break;
//        case QImage::Format_RGB888: // RR,GG,BB字节顺序存储
//            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
//            // opencv需要转为BGR的字节顺序
//            cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
//            break;
//        case QImage::Format_RGBA64: // uint64存储，顺序和Format_ARGB32相反，RGBA
//            mat = cv::Mat(image.height(), image.width(), CV_16UC4, (void*)image.constBits(), image.bytesPerLine());
//            // opencv需要转为BGRA的字节顺序
//            cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
//            break;
//        }
//        return mat;
//    }

//    QImage cvMatToQImage(cv::Mat& mat)
//    {
//        QImage image;
//        switch(mat.type())
//        {
//        case CV_8UC1:
//            // QImage构造：数据，宽度，高度，每行多少字节，存储结构
//            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
//            break;
//        case CV_8UC3:
//            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
//            image = image.rgbSwapped(); // BRG转为RGB
//            // Qt5.14增加了Format_BGR888
//            // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
//            break;
//        case CV_8UC4:
//            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
//            break;
//        case CV_16UC4:
//            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
//            image = image.rgbSwapped(); // BRG转为RGB
//            break;
//        }
//        return image;
//    }


    QImage cvMatToQImage(cv::Mat &src)
    {
        if(src.channels() == 1) { // if grayscale image
            return QImage((uchar*)src.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_Grayscale8).copy();
        }
        if(src.channels() == 3) { // if 3 channel color image
            cv::Mat rgbMat;
            cv::cvtColor(src, rgbMat, cv::COLOR_BGR2RGB); // invert BGR to RGB
            return QImage((uchar*)rgbMat.data, src.cols, src.rows, static_cast<int>(src.step), QImage::Format_RGB888).copy();
        }
        return QImage();
    }

    //将QImage转化为Mat
    cv::Mat qImageToCvMat( const QImage &inImage)
    {
        switch (inImage.format())
        {
        case QImage::Format_RGB32:
        case QImage::Format_RGB888:
        {

            QImage   swapped = inImage;
            if ( inImage.format() == QImage::Format_RGB32 )
            {
                swapped = swapped.convertToFormat( QImage::Format_RGB888 );
            }
            cv::Mat matImg = cv::Mat(swapped.height(), swapped.width(),
                                      CV_8UC3,
                                      const_cast<uchar*>(swapped.bits()),
                                      static_cast<size_t>(swapped.bytesPerLine())
                                      );
            cv::cvtColor(matImg,matImg,cv::COLOR_RGB2BGR);
            return matImg.clone();
        }
        case QImage::Format_Indexed8:
        {
            cv::Mat  mat( inImage.height(), inImage.width(),
                          CV_8UC1,
                          const_cast<uchar*>(inImage.bits()),
                          static_cast<size_t>(inImage.bytesPerLine())
                          );
            return mat.clone();
        }
        default:
            break;
        }
        return cv::Mat();
    }

}

