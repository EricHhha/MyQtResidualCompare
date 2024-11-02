#include <QToolTip>
#include <QMessageBox>
#include "MyQGraphicsView.h"
#include <QScrollBar>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QGraphicsView>
#include <QDebug>
#include "customcard.h"
//#include "opencv2/opencv.hpp"
MyQGraphicsView::MyQGraphicsView(QWidget *parent)
    : QGraphicsView{parent},
    m_isPan(false),m_prevPan(0,0),scene(nullptr)
{
    scene = new QGraphicsScene(this);
    this->setScene(scene);
    setDragMode(QGraphicsView::DragMode::NoDrag);
    setInteractive(false);
    setEnabled(false);
    setMouseTracking(true);
    m_isGT_Scene=false;
//    m_time.start();
//    bool isCon = QObject::connect(&m_timer,SIGNAL(timeout()),this,SLOT(updateGt()));
//    qDebug()<<QString("MyGraphicviewQt Timer connect:%01").arg(isCon);
//    m_timer.start(20);
}


void MyQGraphicsView::setQImage(QImage image)
{   if(image.isNull())
        return;
    m_image = QPixmap::fromImage(image);
    initShow(m_image.copy());
}


void MyQGraphicsView::updateGt()
{
    updateGTScene(m_previmagePos);
}

void MyQGraphicsView::setPixmap(QPixmap pixmap)
{
    m_image = pixmap.copy();
    QPixmap pixmap_show=pixmap.copy();
    initShow(pixmap_show);
}

void MyQGraphicsView::resetImage()
{
    if(m_image.isNull())
        return;
    scene->clear();
    setEnabled(false);
}

QImage  MyQGraphicsView::getQImage()
{
    return m_image.toImage();
}


QPixmap MyQGraphicsView::getPixmap()
{
    return m_image;
}


void MyQGraphicsView::setRectangle(int rect_v)
{
    m_rect=rect_v;
    updateGt();
}

void MyQGraphicsView::initShow(QPixmap pixmap)
{
    setEnabled(true);
    setMouseTracking(true);
    scene->clear();
    scene->addPixmap(pixmap);
    scene->update();
    this->resetTransform();
    this->setSceneRect(pixmap.rect());
    this->fitInView(QRect(0, 0, pixmap.width(), pixmap.height()), Qt::KeepAspectRatio);
}

void MyQGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && event->modifiers() &Qt::ControlModifier ){
        if(m_isGT_Scene)
        {
//            //判断执行的时间 50ms内只执行一次
//            if(m_time.elapsed()<200)
//            {
//                event->accept();
//                qDebug()<<"time <200";
//                return;
//            }
            QPoint pos=event->pos();
            QPoint imagePos = mapToScene(pos).toPoint();
            QPoint imageGpos= mapToGlobal(pos);
            QPoint img_pos=QPoint(imagePos.x()/m_scale,imagePos.y()/m_scale);
            m_previmagePos=imagePos;
            updateGTScene(imagePos);


            QToolTip::showText(imageGpos,QString("img_pix: %01,%02 global_idx:%03").arg(img_pos.x()).arg(img_pos.y()));
            //ctrl+鼠标左键
        //        togglePan(true, event->pos());

//            m_time.restart();
            event->accept();
        }
    }
    if(event->button() == Qt::LeftButton){
        togglePan(true, event->pos());
        event->accept();
        return;
    }
    event->ignore();
}

void MyQGraphicsView::updateGTScene(QPoint pos)
{
//    qDebug()<<"update scene\n";
    QPixmap pix_show=m_image.copy();
    if(pix_show.isNull())
        return;
    QRect rect_indicate;
    if(m_rect>pix_show.width()||m_rect>pix_show.height())
    {
        rect_indicate=QRect(0,0,pix_show.width(),pix_show.height());
    }
    else{
        int x=pos.x()-m_rect/2;
        int y=pos.y()-m_rect/2;
        int w=m_rect;
        int h=m_rect;
        if(x<0)
            x=0;
        if(x>pix_show.width()-w)
            x=pix_show.width()-w;
        if(y<0)
            y=0;
        if(y>pix_show.height()-h)
            y=pix_show.height()-h;

        rect_indicate=QRect(x,y,w,h);
    }

    QPainter painter(&pix_show);
//    painter.setPen(Qt::red);
    painter.setPen(QPen(Qt::green,3));
    painter.drawRect(rect_indicate);

    m_pixmapGT=pix_show;

    scene->clear();
    scene->addPixmap(pix_show);
    scene->update();
    for(CustomCard* card:m_CardWidgets)
    {
        int x=pos.x()/m_scale-m_rect/2;
        int y=pos.y()/m_scale-m_rect/2;
        int w=m_rect;
        int h=m_rect;
        QRect rect_indicate_set=QRect(x,y,w,h);

//        qDebug()<<QString("img_px:%01,%02 rect:%03 %04 %05 %06\n").arg(pos.x()/m_scale).arg(pos.y()/m_scale)
//                  .arg(x).arg(y).arg(w).arg(h);

        card->setShowRect(rect_indicate_set);
        card->updateScene();
    }


}

void MyQGraphicsView::KeyBoardUpdateGTScene(QPoint pos,int rect)
{
    //    qDebug()<<"update scene\n";
        QPixmap pix_show=m_image.copy();
        if(pix_show.isNull())
            return;
        QRect rect_indicate;
        rect_indicate=QRect(0,0,pix_show.width(),pix_show.height());


        QPainter painter(&pix_show);
    //    painter.setPen(Qt::red);
        painter.setPen(QPen(Qt::green,3));
        painter.drawRect(rect_indicate);

        m_pixmapGT=pix_show;

        scene->clear();
        scene->addPixmap(pix_show);
        scene->update();
        for(CustomCard* card:m_CardWidgets)
        {
            int x=0;
            int y=0;
            int w=pix_show.width()/m_scale;
            int h=pix_show.height()/m_scale;
            QRect rect_indicate_set=QRect(x,y,w,h);

    //        qDebug()<<QString("img_px:%01,%02 rect:%03 %04 %05 %06\n").arg(pos.x()/m_scale).arg(pos.y()/m_scale)
    //                  .arg(x).arg(y).arg(w).arg(h);

            card->setShowRect(rect_indicate_set);
            card->updateScene();
        }
}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isPan) {
        pan(event->pos());
        event->accept();
        return;
    }
    event->ignore();
}

void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        togglePan(false);
        event->accept();
        return;
    }
    event->ignore();
}

void MyQGraphicsView::zoom(QPoint factor)
{
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left(), FOV.top(), FOV.width(), FOV.height());
    float scaleX = static_cast<float>(m_image.width()) / FOVImage.width();
    float scaleY = static_cast<float>(m_image.height()) / FOVImage.height();
    float minScale = scaleX > scaleY ? scaleY : scaleX;
    float maxScale = scaleX > scaleY ? scaleX : scaleY;
    if ((factor.y() > 0 && minScale > 100) || (factor.y() < 0 && maxScale < 1 )) {
      return;
    }
    if(factor.y()>0)
        scale(1.2, 1.2);
    else
        scale(0.8, 0.8);
}

void MyQGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_O) {
        this->resetTransform();
        this->setSceneRect(m_image.rect());
        this->fitInView(QRect(0, 0, m_image.width(), m_image.height()), Qt::KeepAspectRatio);
        KeyBoardUpdateGTScene(m_previmagePos,0);
    }
}

void MyQGraphicsView::pan(const QPoint &panTo)
{
    auto hBar = horizontalScrollBar();
    auto vBar = verticalScrollBar();
    auto delta = panTo - m_prevPan;
    m_prevPan = panTo;
    hBar->setValue(hBar->value() - delta.x());
    vBar->setValue(vBar->value() - delta.y());
}

void MyQGraphicsView::wheelEvent(QWheelEvent *event)
{
    if(m_image.isNull())
        return;
    QPoint numDegrees = event->angleDelta() / 8;
    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        zoom(numSteps);
    }
    event->accept();
}

void MyQGraphicsView::togglePan(bool pan, const QPoint &startPos)
{
    if(pan){
        if(m_isPan) {
            return;
        }
        m_isPan = true;
        m_prevPan = startPos;
        setCursor(Qt::ClosedHandCursor);
    } else {
        if(!m_isPan) {
            return;
        }
        m_isPan = false;
        m_prevPan = QPoint(0,0);
        setCursor(Qt::ArrowCursor);
    }
}


//void MyQGraphicsView::paintEvent(QPaintEvent *event)
//{
////    QGraphicsScene *scene = scene;
//    // 设置图像大小与场景图元边界大小一致
//    QRect itemsBoundingRect = QRect(100,100,50,50);


//    QPainter painter;
//    painter.begin(viewport());
//    painter.setRenderHints(/*QPainter::Antialiasing | */QPainter::/*HighQualityAntialiasing*/SmoothPixmapTransform);
//    scene->render(&painter, m_backgroundRect, itemsBoundingRect);
//    painter.setPen(Qt::red);
//    painter.drawRect(itemsBoundingRect);

//    QGraphicsView::paintEvent(event);
//}


