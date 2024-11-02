#include "customcard.h"
#include "ui_customcard.h"
#include "image_tools.h"
#include <QDebug>
CustomCard::CustomCard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomCard)
{
    ui->setupUi(this);
    m_showRect=QRect(0,0,0,0);
    m_issetMap=false;
    m_compare_type=0;
//    bool isCon = QObject::connect(&m_timer,SIGNAL(timeout()),this,SLOT(updateScene()));
//    qDebug()<<QString("Timer connect:%01").arg(isCon);
//    m_timer.start(20);
}

CustomCard::~CustomCard()
{
    m_timer.stop();
    delete ui;
}

void CustomCard::setSRPixMap(QString label_name, QString gt_map_path){
    m_sr_label = label_name;
    m_sr_path = gt_map_path;
    m_sr_map = QPixmap(m_sr_path);
    m_issetMap=true;
}

void CustomCard::setResidualPixMap(QString label_name, QString gt_map_path){
    //目前传进来的是GT
    //转成Y通道
    m_residual_label =label_name;
    m_gt_path = gt_map_path;
    m_residual_map = QPixmap(m_gt_path);
    m_gt_map = m_residual_map;
    m_issetMap=true;
}

void CustomCard::setSRPixMap(QString label_name, QPixmap sr_map)
{
    m_sr_label = label_name;
//    m_sr_path = gt_map_path;
    m_sr_map = sr_map;
    m_issetMap=true;
}
void CustomCard::setResidualPixMap(QString label_name, QPixmap gt_map)
{
    //目前传进来的是GT
    //转成Y通道
    m_residual_label =label_name;
//    m_gt_path = gt_map_path;
//    m_residual_map = QPixmap(m_gt_path);
    m_gt_map = gt_map;
    m_issetMap=true;
}

void CustomCard::setMixable(bool ismix)
{
    m_ismixable=ismix;
    updateScene();
}



QPixmap CustomCard::getSRMapValue(){
    //放进来的是原图
    if(m_showRect==QRect(0,0,0,0)||m_showRect.width()>m_sr_map.width()||m_showRect.width()>m_sr_map.height()||m_showRect.width()==0||m_showRect.height()==0)
    {
        m_showRect=QRect(0,0,m_sr_map.width(),m_sr_map.height());
    }
    int x=m_showRect.x();
    int y=m_showRect.y();
    int w=m_showRect.width();
    int h=m_showRect.height();
    if(x<0)
        x=0;
    if(x>m_sr_map.width()-w)
        x=m_sr_map.width()-w;
    if(y<0)
        y=0;
    if(y>m_sr_map.height()-h)
        y=m_sr_map.height()-h;
    QRect copy_rect=QRect(x,y,w,h);

    QPixmap sr_map=m_sr_map.copy(copy_rect);

    return sr_map;
}                                           //返回QPixmap
QPixmap CustomCard::getResidualValue(){
    //放进来的是原图
    if(m_showRect==QRect(0,0,0,0)||m_showRect.width()>m_sr_map.width()||m_showRect.width()>m_sr_map.height()||m_showRect.width()==0||m_showRect.height()==0)
    {
        m_showRect=QRect(0,0,m_residual_map.width(),m_residual_map.height());
    }

    int x=m_showRect.x();
    int y=m_showRect.y();
    int w=m_showRect.width();
    int h=m_showRect.height();
    if(x<0)
        x=0;
    if(x>m_residual_map.width()-w)
        x=m_residual_map.width()-w;
    if(y<0)
        y=0;
    if(y>m_residual_map.height()-h)
        y=m_residual_map.height()-h;
    QRect copy_rect=QRect(x,y,w,h);

    QPixmap sr_map=m_residual_map.copy(copy_rect);

    return sr_map;
}                                  //获取残差图
QPixmap CustomCard::getMixImageValue(){
    //放进来的是原图
    if(m_showRect==QRect(0,0,0,0)||m_showRect.width()>m_sr_map.width()||m_showRect.width()>m_sr_map.height()||m_showRect.width()==0||m_showRect.height()==0)
    {
        m_showRect=QRect(0,0,m_mixadd_map.width(),m_mixadd_map.height());
    }

    int x=m_showRect.x();
    int y=m_showRect.y();
    int w=m_showRect.width();
    int h=m_showRect.height();
    if(x<0)
        x=0;
    if(x>m_mixadd_map.width()-w)
        x=m_mixadd_map.width()-w;
    if(y<0)
        y=0;
    if(y>m_mixadd_map.height()-h)
        y=m_mixadd_map.height()-h;
    QRect copy_rect=QRect(x,y,w,h);

    QPixmap sr_map=m_mixadd_map.copy(copy_rect);

    return sr_map;

}                                    //返回混合后的图

void CustomCard::updateScene()
{
    if(!m_issetMap)
        return;
    //放进来的是原图
    if(m_showRect==QRect(0,0,0,0)||m_showRect.width()>m_sr_map.width()||m_showRect.width()>m_sr_map.height()||m_showRect.width()==0||m_showRect.height()==0)
    {
        m_showRect=QRect(0,0,m_sr_map.width(),m_sr_map.height());
    }
    ui->label_sr->setText(QString(m_sr_label));
    ui->label_residual->setText(QString(m_residual_label));
    int x=m_showRect.x();
    int y=m_showRect.y();
    int w=m_showRect.width();
    int h=m_showRect.height();
    if(x<0)
        x=0;
    if(x>m_sr_map.width()-w)
        x=m_sr_map.width()-w;
    if(y<0)
        y=0;
    if(y>m_sr_map.height()-h)
        y=m_sr_map.height()-h;
    QRect copy_rect=QRect(x,y,w,h);

    QPixmap sr_map=m_sr_map.copy(copy_rect);
//    // 获取控件实际大小
    QRect geometry = ui->graphicsView_sr->geometry();

    // 计算缩放比例
    double scale = 1.0;
    double scaleX = static_cast<double>(geometry.width()) / (sr_map.width() +1);// 不+1会有滚动条
    double scaleY = static_cast<double>(geometry.height()) / (sr_map.height() +1);// 不+1会有滚动条

    if (scaleX > scaleY){
        scale = scaleY;
    }else{
        scale = scaleX;
    }
//    m_scale=scale;
    // 缩放
    sr_map = sr_map.scaled(sr_map.width() * scale, sr_map.height() * scale, Qt::KeepAspectRatio);
//    // 添加图片到场景，并将场景添加到视图中
//    RGBScene->addPixmap(RGBPixmap);
//    img_widget->setScene(RGBScene);
    //开始显示
    ui->graphicsView_sr->setScale(scale);
//    ui->graphicsView_gt->resetImage();
    ui->graphicsView_sr->setPixmap(sr_map);
    x=m_showRect.x();
    y=m_showRect.y();
    w=m_showRect.width();
    h=m_showRect.height();
    if(x<0)
        x=0;
    if(x>m_residual_map.width()-w)
        x=m_residual_map.width()-w;
    if(y<0)
        y=0;
    if(y>m_residual_map.height()-h)
        y=m_residual_map.height()-h;
    copy_rect=QRect(x,y,w,h);



    //残差图
    QPixmap residual_map_=getResidualMap();



    QPixmap residual_map=residual_map_.copy(copy_rect);
//    // 获取控件实际大小
    QRect geometry_res = ui->graphicsView_residual->geometry();

    // 计算缩放比例
    scale = 1.0;
    scaleX = static_cast<double>(geometry_res.width()) / (residual_map.width() +1);// 不+1会有滚动条
    scaleY = static_cast<double>(geometry_res.height()) / (residual_map.height() +1);// 不+1会有滚动条

    if (scaleX > scaleY){
        scale = scaleY;
    }else{
        scale = scaleX;
    }
//    m_scale=scale;
    // 缩放
    residual_map = residual_map.scaled(residual_map.width() * scale, residual_map.height() * scale, Qt::KeepAspectRatio);
//    // 添加图片到场景，并将场景添加到视图中
//    RGBScene->addPixmap(RGBPixmap);
//    img_widget->setScene(RGBScene);
    //开始显示
    ui->graphicsView_residual->setScale(scale);
//    ui->graphicsView_gt->resetImage();
    ui->graphicsView_residual->setPixmap(residual_map);

//    ui->graphicsView_residual->setPixmap(m_residual_map.copy(m_showRect));
}

QPixmap CustomCard::getResidualMap()
{
    //转成opencvmat
//    cv::Mat gt_map=Tools::qImageToCvMat(gt.toImage());
//    cv::Mat sr_map=Tools::qImageToCvMat(sr.toImage());
    cv::Mat gt_map=cv::imread(m_gt_path.toLocal8Bit().toStdString());
    cv::Mat sr_map=cv::imread(m_sr_path.toLocal8Bit().toStdString());
    QPixmap pixmap;
    if(m_compare_type==1)
    {
         //如果是rgb模式
        cv::Mat diff;
//        diff=cv::abs(gt_map-sr_map);
        cv::subtract(gt_map,sr_map,diff);
        pixmap=QPixmap::fromImage(Tools::cvMatToQImage(diff));

    }
    else if(m_compare_type==0)
    {
        //Y 模式
        cv::Mat gt_yuv,sr_yuv;
//        gt_yuv=gt_map;
//        sr_yuv=sr_map;
        cv::cvtColor(gt_map,gt_yuv,cv::COLOR_BGR2YUV);
        cv::cvtColor(sr_map,sr_yuv,cv::COLOR_BGR2YUV);
        std::vector<cv::Mat> gt_yuv_mats,sr_yuv_mats;
        cv::split(gt_yuv,gt_yuv_mats);
        cv::split(sr_yuv,sr_yuv_mats);
        cv::Mat gt_y,sr_y;
        gt_y=gt_yuv_mats.at(0);
        sr_y=sr_yuv_mats.at(0);
        cv::Mat diff;
        cv::subtract(gt_y,sr_y,diff);
//        diff=cv::abs(gt_map-sr_map);
        QImage diff_i= Tools::cvMatToQImage(diff);
        QImage diff_image=QImage(m_gt_map.toImage());
        for(int i=0;i<diff.rows;++i)
        {
            for(int j=0;j<diff.cols;++j)
            {
                int diffv=diff_i.pixelColor(i,j).red();
                QColor color=m_rgbMap.value(diffv);
                if(diffv==0)
                {
                    color=QColor(0,0,0);
                }
                diff_image.setPixelColor(i,j,QColor(color.red(),color.green(),color.blue()));
//                qDebug()<<QString("i:%01 j:%02 diff: r %03 g：%04 ：%05").arg(i).arg(j).arg(diff_i.pixelColor(i,j).red())
//                                    .arg(diff_i.pixelColor(i,j).green()).arg(diff_i.pixelColor(i,j).blue());
            }
        }
        //    msdtgp_imgcu_residual_show = 0.1 * msdtgp_imgcu_residual_show + msdtgp_imgcu

        pixmap=QPixmap::fromImage(diff_image);
    }
    m_residual_map=pixmap.copy();
    if(m_ismixable)
    {
        //残差 pixmap 原值 sr_map
        cv::Mat residual_mat=Tools::qImageToCvMat(pixmap.toImage());
        cv::Mat mix_mat;
        qDebug()<<QString("sr_alpha:%01  res_alpha:%02 gmma:%03").arg(m_colorSrAlpha).arg(m_colorResAlpha).arg(m_colorGamma);
        cv::addWeighted(residual_mat,m_colorResAlpha,sr_map,m_colorSrAlpha,m_colorGamma,mix_mat);
        //叠加原值
//        cv::Mat gt_map_diff = Tools::qImageToCvMat(diff_image);
////        cv::Mat gt_map_diff_show = m_colormix * gt_map_diff + sr_map;

        QImage gt_map_show=Tools::cvMatToQImage(mix_mat);
        pixmap=QPixmap::fromImage(gt_map_show);
    }
    m_mixadd_map=pixmap.copy();

//    pixmap=QPixmap::fromImage(Tools::cvMatToQImage(sr_map));
    return pixmap;
}

QPixmap CustomCard::getResidualMap(QPixmap sr_map_,QPixmap gt_map_)
{
    //转成opencvmat
    cv::Mat gt_map=Tools::qImageToCvMat(gt_map_.toImage());
    cv::Mat sr_map=Tools::qImageToCvMat(sr_map_.toImage());
//    cv::Mat gt_map=cv::imread(m_gt_path.toLocal8Bit().toStdString());
//    cv::Mat sr_map=cv::imread(m_sr_path.toLocal8Bit().toStdString());
    QPixmap pixmap;
    if(m_compare_type==1)
    {
         //如果是rgb模式
        cv::Mat diff;
//        diff=cv::abs(gt_map-sr_map);
        cv::subtract(gt_map,sr_map,diff);
        pixmap=QPixmap::fromImage(Tools::cvMatToQImage(diff));

    }
    else if(m_compare_type==0)
    {
        //Y 模式
        cv::Mat gt_yuv,sr_yuv;
//        gt_yuv=gt_map;
//        sr_yuv=sr_map;
        cv::cvtColor(gt_map,gt_yuv,cv::COLOR_BGR2YUV);
        cv::cvtColor(sr_map,sr_yuv,cv::COLOR_BGR2YUV);
        std::vector<cv::Mat> gt_yuv_mats,sr_yuv_mats;
        cv::split(gt_yuv,gt_yuv_mats);
        cv::split(sr_yuv,sr_yuv_mats);
        cv::Mat gt_y,sr_y;
        gt_y=gt_yuv_mats.at(0);
        sr_y=sr_yuv_mats.at(0);
        cv::Mat diff;
        cv::subtract(gt_y,sr_y,diff);
//        diff=cv::abs(gt_map-sr_map);
        QImage diff_i= Tools::cvMatToQImage(diff);
        QImage diff_image=QImage(m_gt_map.toImage());
        for(int i=0;i<diff.rows;++i)
        {
            for(int j=0;j<diff.cols;++j)
            {
                int diffv=diff_i.pixelColor(i,j).red();
                QColor color=m_rgbMap.value(diffv);
                if(diffv==0)
                {
                    color=QColor(0,0,0);
                }
                diff_image.setPixelColor(i,j,QColor(color.red(),color.green(),color.blue()));
//                qDebug()<<QString("i:%01 j:%02 diff: r %03 g：%04 ：%05").arg(i).arg(j).arg(diff_i.pixelColor(i,j).red())
//                                    .arg(diff_i.pixelColor(i,j).green()).arg(diff_i.pixelColor(i,j).blue());
            }
        }
        //    msdtgp_imgcu_residual_show = 0.1 * msdtgp_imgcu_residual_show + msdtgp_imgcu

        pixmap=QPixmap::fromImage(diff_image);
    }

    if(m_ismixable)
    {
        //残差 pixmap 原值 sr_map
        cv::Mat residual_mat=Tools::qImageToCvMat(pixmap.toImage());
        cv::Mat mix_mat;
        qDebug()<<QString("sr_alpha:%01  res_alpha:%02 gmma:%03").arg(m_colorSrAlpha).arg(m_colorResAlpha).arg(m_colorGamma);
        cv::addWeighted(residual_mat,m_colorResAlpha,sr_map,m_colorSrAlpha,m_colorGamma,mix_mat);
        //叠加原值
//        cv::Mat gt_map_diff = Tools::qImageToCvMat(diff_image);
////        cv::Mat gt_map_diff_show = m_colormix * gt_map_diff + sr_map;

        QImage gt_map_show=Tools::cvMatToQImage(mix_mat);
        pixmap=QPixmap::fromImage(gt_map_show);
    }


//    pixmap=QPixmap::fromImage(Tools::cvMatToQImage(sr_map));
    return pixmap;
}


