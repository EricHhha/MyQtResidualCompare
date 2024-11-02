//#ifndef MyQGraphicsView_H
//#define MyQGraphicsView_H

//#include<QGraphicsView>
//#include<QLabel>
//#include<QPointF>
//#include<QMouseEvent>

//class RegistrationRectify;
//class MyQGraphicsView:public QGraphicsView
//{
//    Q_OBJECT
//public:
//    MyQGraphicsView(QWidget*parent=nullptr);
//    ~MyQGraphicsView();

//    void setDialog(RegistrationRectify*dlg){
//        m_dialog=dlg;
//    }
//protected:
//    void init();
//    void rgbValueInImage(QPoint pos);
//    //调用
//    void mouseMoveEvent(QMouseEvent* event);
//private:
//    QPointF m_scenePoint;
//    QLabel* scenePointDisplay;
//    RegistrationRectify* m_dialog;
//};


//#endif



#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QImage>
#include <QList>
#include <QPair>
#include <QTimer>
#include <QTime>
#include <QWheelEvent>
#include <QGraphicsScene>
//#include "opencv2/opencv.hpp"
class CustomCard;
class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyQGraphicsView(QWidget *parent = nullptr);

signals:


public:
    void setQImage(QImage);
//    void setMatImage(cv::Mat );
    //type=0 原始 type=1 ori type=2 rectify
    void setPixmap(QPixmap);
    void resetImage();
    QImage getQImage();
//    cv::Mat getMatImage();
    QPixmap getPixmap();
    void setScale(double scale){m_scale=scale;}
    void setRectangle(int rect_v);
    void KeyBoardUpdateGTScene(QPoint pos,int rect);
    void setGTSceneEnable(bool isgt){
        m_isGT_Scene=isgt; }
    void setCardWidgets(QList<CustomCard*> cards)
    {
        m_CardWidgets=cards;
    }
    QPixmap getGTPixmap(){return m_pixmapGT;}

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
//    void paintEvent(QPaintEvent *event);
    void zoom(QPoint factor);
    void togglePan(bool pan, const QPoint &startPos = QPoint());
    void pan(const QPoint &panTo);
    void initShow(QPixmap pixmap);
    void updateGTScene(QPoint pos);
public slots:
    void updateGt();


private:
    QPixmap m_image;
    QTimer m_timer;
    QTime m_time;
    QPixmap m_pixmapGT;
    bool m_isPan;
    double m_scale;
    QPoint m_prevPan;
    QPoint m_previmagePos;
    QGraphicsScene *scene;
    int m_rect=200;                           //rect大小
    /////////////////////////////
    double m_backgroundScale;
    QRectF m_backgroundRect;
    QRectF m_indicatorRect;
    QRectF m_restrictRect;
    bool m_isGT_Scene;
    QList<CustomCard*> m_CardWidgets;

};

#endif // IMAGEVIEWER_H


