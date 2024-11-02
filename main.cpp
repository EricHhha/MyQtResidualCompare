#include "mainwindow.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    cv::Mat image = cv::imread("/home/lenovo/Pictures/1.png", cv::IMREAD_COLOR);
//    cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);
//    cv::imshow("Ubuntu",image);
//    cv::waitKey(0);

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
