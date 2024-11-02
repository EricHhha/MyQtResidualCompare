#ifndef CUSTOMCARD_H
#define CUSTOMCARD_H

#include <QWidget>
#include <QPixmap>
#include <QColor>
#include <QTime>
#include <QTimer>
#include <QMap>
#include <opencv2/opencv.hpp>
namespace Ui {
class CustomCard;
}

class CustomCard : public QWidget
{
    Q_OBJECT
public:
//    enum COMPARETYPE{
//        Y=0,
//        RGB=1
//    };
public:
    explicit CustomCard(QWidget *parent = nullptr);
    ~CustomCard();
//公共函数
    void setSRPixMap(QString label_name, QString gt_map);
    void setResidualPixMap(QString label_name, QString gt_map);
    void setSRPixMap(QString label_name, QPixmap sr_map);
    void setResidualPixMap(QString label_name, QPixmap gt_map);
//
    void setCompareType(int type){m_compare_type=type;};
    QPixmap getResidualMap();
    QPixmap getResidualMap(QPixmap sr_map,QPixmap gt_map);
    void setMixable(bool ismix);
    void setShowRect(QRect rect){m_showRect=rect;};
    void setRgbMap(QMap<int,QColor> colmap){m_rgbMap=colmap;}
    void setMixSrAlpha(double colormix){m_colorSrAlpha=colormix;}
    void setMixResAlpha(double colormix){m_colorResAlpha=colormix;}
    void setMixPictureGamma(double colormix){m_colorGamma=colormix;}
    QString getlabelName(){return m_sr_label;}                                          //获取实验名称
    QString getResidualName(){return m_residual_label;}                                 //获取残差名称
    QPixmap getSRMapValue();                                                            //返回QPixmap
    QPixmap getResidualValue();                                                         //获取残差图
    QPixmap getMixImageValue();                                                         //返回混合后的图


public slots:
    void updateScene();
private:
    Ui::CustomCard *ui;
//    QTime m_timer;
    QTimer m_timer;
    QString m_sr_label;
    QString m_residual_label;
    QString m_sr_path;                                                      //sr 路径
    QString m_gt_path;                                                      //gt 路径
    QPixmap m_gt_map;
    QRect m_showRect;
    QPixmap m_sr_map;
    QPixmap m_residual_map;
    QPixmap m_mixadd_map;                                                   //混合图
    QMap<int,QColor> m_rgbMap;
    int m_compare_type;                      //0 是Y 1是RGB
    bool m_issetMap;
    double m_colorSrAlpha=0.5;                                                      //混合叠加值
    double m_colorResAlpha=0.5;                                                      //混合叠加值
    double m_colorGamma=0.0;                                                      //混合叠加值
    bool m_ismixable=false;

};

#endif // CUSTOMCARD_H
