#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QScrollBar>
#include <QSettings>
#include <opencv2/opencv.hpp>
#include "image_tools.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化连接信号
    initConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnect()
{
    ui->graphicsView_gt->setGTSceneEnable(true);                        //设置场景为GT

    m_CardWidgets.push_back(ui->widget_SR1);
    m_CardWidgets.push_back(ui->widget_sr2);
    m_CardWidgets.push_back(ui->widget_sr3);
    m_CardWidgets.push_back(ui->widget_sr4);
    m_CardWidgets.push_back(ui->widget_sr5);
    m_CardWidgets.push_back(ui->widget_sr6);
    m_CardWidgets.push_back(ui->widget_sr7);
    m_CardWidgets.push_back(ui->widget_sr8);

    ui->graphicsView_gt->setCardWidgets(m_CardWidgets);
    double mixValuesr=ui->doubleSpinBox_alpha_sr->value();
    double mixValueres=ui->doubleSpinBox_alpha_res->value();
    double mixValuegmma=ui->doubleSpinBox_gamma->value();
    for(auto iter:m_CardWidgets)
    {
        iter->setCompareType(m_comparetype);
        iter->setMixSrAlpha(mixValuesr);
        iter->setMixResAlpha(mixValueres);
        iter->setMixPictureGamma(mixValuegmma);
        iter->updateScene();
    }
    //初始化
    update_mixState(ui->groupBox_mixcolor->isChecked());
    update_color();
    compareModeChangeSlot();
    update_bcolor(ui->comboBox_color->currentIndex());
    update_mixColor(ui->doubleSpinBox_alpha_sr->value());
    update_rectangle(ui->spinBox_rectangle->value());


    //
    ui->toolButton_next->setEnabled(false);
    ui->toolButton_last->setEnabled(false);
    QObject::connect(ui->radioButton_y,&QRadioButton::clicked,this,&MainWindow::compareModeChangeSlot);
    QObject::connect(ui->radioButton_rgb,&QRadioButton::clicked,this,&MainWindow::compareModeChangeSlot);
    QObject::connect(ui->toolButton_opendir,&QToolButton::clicked,this,&MainWindow::opendir_slot);
    QObject::connect(ui->toolButton_last,&QToolButton::clicked,this,&MainWindow::last_slot);
    QObject::connect(ui->toolButton_next,&QToolButton::clicked,this,&MainWindow::next_slot);
    QObject::connect(ui->toolButton_save,&QToolButton::clicked,this,&MainWindow::saveResults_slot);
    QObject::connect(ui->scroll_color,&QScrollBar::sliderReleased,this,&MainWindow::update_color);
    QObject::connect(ui->score_color_max,&QScrollBar::sliderReleased,this,&MainWindow::update_color);
    bool isConnect= QObject::connect(ui->comboBox_color,SIGNAL(currentIndexChanged(int)),this,SLOT(update_bcolor(int)));
    isConnect=QObject::connect(ui->doubleSpinBox_alpha_sr,SIGNAL(valueChanged(double)),this,SLOT(update_mixColor(double)));
    isConnect=QObject::connect(ui->doubleSpinBox_alpha_res,SIGNAL(valueChanged(double)),this,SLOT(update_mixColor(double)));
    isConnect=QObject::connect(ui->doubleSpinBox_gamma,SIGNAL(valueChanged(double)),this,SLOT(update_mixColor(double)));
    isConnect=QObject::connect(ui->spinBox_rectangle,SIGNAL(valueChanged(int)),this,SLOT(update_rectangle(int)));
    isConnect=QObject::connect(ui->groupBox_mixcolor,SIGNAL(clicked(bool)),this,SLOT(update_mixState(bool)));
    qDebug()<<QString("combox connect is %01").arg(isConnect);
    update_color();
    //toolButton_save
}

void MainWindow::opendir_slot()
{
    QSettings set(this);
    set.beginGroup("path");
    QString selPath=set.value("seldir").toString();
    if(selPath.isEmpty())
        selPath=qApp->applicationDirPath();
    if(!m_rootDir.isEmpty())
    {
        selPath=QFileInfo(selPath).absolutePath();
    }
    QString root_dir=QFileDialog::getExistingDirectory(this,QString("选择根目录"),selPath);
    if(root_dir.isEmpty())
        return;
    set.setValue("seldir",QVariant(root_dir));
    set.endGroup();

    m_rootDir=root_dir;
    m_rootStructMap=readRootInfo();
}

bool MainWindow::isCanMove()
{
    //总共多少个
    int totalCount=0;
    if(!m_rootStructMap.isEmpty())
    {
        for(auto iter=m_rootStructMap.begin();iter!=m_rootStructMap.end();++iter)
        {
            QString expDir=iter.key();
            //序列 和序列中的list
            QVariantMap expValue=iter.value().toMap();
            QList<QVariant> seqfiles = expValue.value(m_curSeq).toList();
            totalCount=seqfiles.count();
        }
    }

    if(m_curIdx<=0)
    {
        //如果当前小于0
        ui->toolButton_last->setEnabled(false);
        ui->toolButton_next->setEnabled(true);
        m_curIdx=0;
    }
    else if(m_curIdx>0 && m_curIdx<totalCount)
    {
        ui->toolButton_last->setEnabled(true);
        ui->toolButton_next->setEnabled(true);
    }
    else{
        ui->toolButton_last->setEnabled(true);
        ui->toolButton_next->setEnabled(false);
        m_curIdx=totalCount-1;
    }
}

bool MainWindow::isCanSeqMove()
{
    //总共多少个
    int totalCount=m_seqButtons.count();

    if(m_seqIdx<=0)
    {
        //如果当前小于0
       m_seqIdx=totalCount-1;
    }
    else if(m_seqIdx>0 && m_seqIdx<totalCount)
    {
        m_seqIdx=m_seqIdx;
    }
    else{
        m_seqIdx=0;
    }
    QPushButton* button = m_seqButtons.at(m_seqIdx);
    if(button)
    {
        QString seq = button->text();
        log(QString("当前序列：%01").arg(seq));
        m_curSeq=seq;
        button->setEnabled(false);
        ui->toolButton_last->setEnabled(true);
        ui->toolButton_next->setEnabled(true);
    }
    for(auto buttoniter:m_seqButtons)
    {
        if(buttoniter==button)
            continue;
        buttoniter->setEnabled(true);
    }
    m_curIdx=0;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
//    qDebug()<<"kkkkkkk";
    if(event->key() == Qt::Key_O) {

    }
    else if (event->key() == Qt::Key_Left||event->key() == Qt::Key_A) {
        last_slot();
    }
    else if(event->key() == Qt::Key_Right||event->key() == Qt::Key_D)
    {
        next_slot();
    }
    else if (event->key() == Qt::Key_Up||event->key() == Qt::Key_W) {
        seq_last_slot();
    }
    else if(event->key() == Qt::Key_Down||event->key() == Qt::Key_S)
    {
        seq_next_slot();
    }
    event->accept();
}

void MainWindow::setGtFileFrame(QString gtfile)
{
    //更新GTFile
    //创建显示容器
//    QGraphicsScene *RGBScene = new QGraphicsScene;
//    QImage image = QImage(img.data, img.cols, img.rows, static_cast<int>(img.step), QImage::Format_RGB888).rgbSwapped();
   m_gt_path=gtfile;
    m_gt_map = QPixmap(gtfile);
    QPixmap RGBPixmap=m_gt_map.copy();
//    // 获取控件实际大小
    QRect geometry = ui->graphicsView_gt->geometry();

    // 计算缩放比例
    double scale = 1.0;
    double scaleX = static_cast<double>(geometry.width()) / (RGBPixmap.width() +1);// 不+1会有滚动条
    double scaleY = static_cast<double>(geometry.height()) / (RGBPixmap.height() +1);// 不+1会有滚动条
    qDebug()<<"gtfile: "<<gtfile << "scale size:" << scaleX << scaleY;
    if (scaleX > scaleY){
        scale = scaleY;
    }else{
        scale = scaleX;
    }
    m_scale=scale;
    // 缩放
    RGBPixmap = RGBPixmap.scaled(RGBPixmap.width() * scale, RGBPixmap.height() * scale, Qt::KeepAspectRatio);
//    // 添加图片到场景，并将场景添加到视图中
//    RGBScene->addPixmap(RGBPixmap);
//    img_widget->setScene(RGBScene);
    //开始显示
    ui->graphicsView_gt->setScale(scale);
//    ui->graphicsView_gt->resetImage();
    ui->graphicsView_gt->setPixmap(RGBPixmap);
    ui->graphicsView_gt->updateGt();
//    m_gt_map=RGBPixmap.copy();
//    ui->graphicsView_gt->show();

}

void MainWindow::updateResults(QList<QPair<QString,QString>> exp_results)
{

    for(int i=0; i<exp_results.count();++i)
    {
        auto iter=exp_results.at(i);
        QString expName=iter.first;
        QString pixmapPath=iter.second;
//        QPixmap RGBPixmap = QPixmap(pixmapPath);
        // 计算缩放比例
        double scale = m_scale;

        // 缩放
//        RGBPixmap = RGBPixmap.scaled(RGBPixmap.width() * scale, RGBPixmap.height() * scale, Qt::KeepAspectRatio);
        CustomCard* card=m_CardWidgets.at(i);
        QString label_name=expName+"_residual";
        card->setSRPixMap(expName,pixmapPath);
        card->setResidualPixMap(label_name,m_gt_path);
        card->updateScene();
    }

}

//更新页面
void MainWindow::updateFrames()
{
    //更新页面图像
    //GT_file 最后一个是GT
    QString gt_file="";
    QList<QPair<QString,QString>> exp_results;
    for(auto iterexp=m_rootStructMap.constBegin();iterexp!=m_rootStructMap.constEnd();)
    {
        //实验方法名称
        QString expName=iterexp.key();
        QVariantMap expMap=iterexp.value().toMap();
        QList<QVariant> seqfiles=expMap.value(m_curSeq).toList();
        QString filename=seqfiles.at(m_curIdx).toString();                  //当前的文件名
        exp_results.push_back(qMakePair(expName,filename));
        ++iterexp;
        if(iterexp==m_rootStructMap.constEnd())
        {
            gt_file=filename;
        }
    }
    if(gt_file.isEmpty())
        return;
    //设置GT_file
    log(QString("cur_gtfile:%01").arg(gt_file));
    setGtFileFrame(gt_file);
    updateResults(exp_results);
}

void MainWindow::seq_last_slot(){
    m_seqIdx=m_seqIdx-1;
    isCanSeqMove();
    updateFrames();
    log(QString("curseq:%01  curidx:%02").arg(m_curSeq).arg(QString::number(m_curIdx)));
}

//下一个idx
void MainWindow::seq_next_slot(){
    m_seqIdx=m_seqIdx+1;
    isCanSeqMove();
    updateFrames();
    log(QString("curseq:%01  curidx:%02").arg(m_curSeq).arg(QString::number(m_curIdx)));
}

//上一个idx
void MainWindow::last_slot(){
    m_seqIdx=m_seqIdx-1;
    isCanMove();
    updateFrames();
    log(QString("curseq:%01  curidx:%02").arg(m_curSeq).arg(QString::number(m_curIdx)));
}

//下一个idx
void MainWindow::next_slot(){
    m_curIdx=m_curIdx+1;
    isCanMove();
    updateFrames();
    log(QString("curseq:%01  curidx:%02").arg(m_curSeq).arg(QString::number(m_curIdx)));
}

//选择序列
void MainWindow::seqSelect_slot()
{
    QPushButton* button = (QPushButton*)sender();
    if(button)
    {
        QString seq = button->text();
        log(QString("当前序列：%01").arg(seq));
        m_curSeq=seq;
        button->setEnabled(false);
        ui->toolButton_last->setEnabled(true);
        ui->toolButton_next->setEnabled(true);
    }
    for(auto buttoniter:m_seqButtons)
    {
        if(buttoniter==button)
            continue;
        buttoniter->setEnabled(true);
    }
    m_curIdx=0;
    updateFrames();
    log(QString("curseq:%01  curidx:%02").arg(m_curSeq).arg(QString::number(m_curIdx)));
}

void MainWindow::saveResults_slot()
{
    //
    qDebug()<<QString("save slot is ok!");
    //保存事件
    //step 1. 选择保存文件目录
    QSettings set(this);
    set.beginGroup("path");
    QString selPath=set.value("seldir").toString();
    if(selPath.isEmpty())
        selPath=qApp->applicationDirPath();
    if(!m_rootDir.isEmpty())
    {
        selPath=QFileInfo(selPath).absolutePath();
    }
    QString root_dir=QFileDialog::getExistingDirectory(this,QString("选择根目录"),selPath);
    if(root_dir.isEmpty())
        return;
    set.setValue("seldir",QVariant(root_dir));
    set.endGroup();
    QDir dir(root_dir);
    if(!dir.exists())
        dir.mkpath(root_dir);                       //此方法能创建多层目录
    //step2. 保存颜色色卡图像
    QString color_lengend_path=root_dir+"/color_legend.png";
    bool isSave = m_colorLegend.save(color_lengend_path);
    if(isSave)
    {
        log(QString("color legend save success! %01").arg(color_lengend_path));
    }
    //step3 保存 GT 图像
    QPixmap GT_map=ui->graphicsView_gt->getGTPixmap();
    QString color_gt=root_dir+"/gt_rectangle.png";
    isSave = GT_map.save(color_gt);
    if(isSave)
    {
        log(QString("GT map with rectangle save success! %01").arg(color_gt));
    }
    for(auto iter:m_CardWidgets)
    {
        QString expname=iter->getlabelName();               //获取方法名称
        QString expDir=root_dir+"/"+expname;
        QDir direxp(expDir);
        if(!direxp.exists())
        {
            direxp.mkpath(expDir);
        }
        QString sr_map_path=expDir+"/"+expname+"_sr.png";
        QString residual_map_path=expDir+"/"+expname+"_residual.png";
        QString mix_map_path=expDir+"/"+expname+"_mixImage.png";

        QString residualName=iter->getResidualName();       //获取残差名称

        QPixmap srMap=iter->getSRMapValue();                //获取重建图
        srMap.save(sr_map_path);
        QPixmap residualMap=iter->getResidualValue();       //获取残差图
        residualMap.save(residual_map_path);
        if(ui->groupBox_mixcolor->isChecked())
        {
            QPixmap mixMap=iter->getMixImageValue();        //获取混合后的图
            mixMap.save(mix_map_path);
        }
    }
    log(QString("save rootdir:%01").arg(root_dir));

}


void MainWindow::compareModeChangeSlot()
{
    if(ui->radioButton_y->isChecked())
        m_comparetype=0;
    else if(ui->radioButton_rgb->isChecked())
        m_comparetype=1;
    else
        int ccc=0;

    for(auto iter:m_CardWidgets)
    {
        iter->setCompareType(m_comparetype);
        iter->updateScene();
    }
}

void MainWindow::update_rectangle(int value)
{
    m_rectangleValue=value;
    //对应更新 rectangle 大小
    int rectValue=ui->spinBox_rectangle->value();
//    for(auto iter:m_CardWidgets)
//    {
//        iter->set(mixValue);
//    }
    ui->graphicsView_gt->setRectangle(rectValue);
}

//更新混合状态
void MainWindow::update_mixState(bool)
{
    //混合状态
    bool mixenable=ui->groupBox_mixcolor->isChecked();
    for(auto iter:m_CardWidgets)
    {
        iter->setMixable(mixenable);
    }
}


void MainWindow::update_color()
{
    int colorType=ui->comboBox_color->currentIndex();
    //改变颜色
    int minvalue=ui->scroll_color->value();
    int maxvalue=ui->score_color_max->value();

    //画出来一个QPixMap
    int width=765;
    int height=50;
    QPixmap colorMap=QPixmap(width,height);
    colorMap.fill(Qt::white);

    QPainter painter(&colorMap);

    int colorBarLength=255;//设置颜色条的长度                                                                   //得到255个颜色
//    int color_len=width/colorBarLength;
    int color_len=3;
    QMap<int,QPoint> cordmap;

    for(int i=0;i<colorBarLength;i++)// gray
    {
       //color.setRgbF(i/colorBarLength,i/colorBarLength,i/colorBarLength);//也可以使用这种方法
//        qDebug()<<QString("i:%01 min:%02 max:%03 color:%04").arg(i).arg(minvalue).arg(maxvalue).arg((i*1.0+1)/colorBarLength*(maxvalue-minvalue)+minvalue);
        QColor color;
        QRect section;
        color.setHsv(0,0,(i*1.0)/(colorBarLength-1)*(maxvalue-minvalue)+minvalue);
        section.setRect(i*color_len,0,color_len,height);
        cordmap.insert(i,section.center());                                     //把色块的中心点坐标记录下来
        painter.fillRect(section,color);
    }
    cv::Mat graymap=Tools::qImageToCvMat(colorMap.toImage());
    cv::Mat distMap;
    cv::applyColorMap(graymap,distMap,colorType);
    QPixmap colorMapColor=QPixmap::fromImage(Tools::cvMatToQImage(distMap));
    ui->label_color->setPixmap(colorMapColor);
    m_colorLegend = colorMapColor;
    QImage colrMapImage=colorMapColor.toImage();
    for(int i=0;i<colorBarLength;++i)
    {
        QPoint pt=cordmap.value(i);
        QColor rgb=colrMapImage.pixelColor(pt);
//        qDebug()<<QString("i:%01 b:%02 g:%03 r:%04").arg(i).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
        m_colMap.insert(i,rgb);
    }
    for(auto itercard:m_CardWidgets)
    {
        itercard->setRgbMap(m_colMap);
//        itercard->updateScene();
    }

}

void MainWindow::update_bcolor(int)
{
    int colorType=ui->comboBox_color->currentIndex();
    //改变颜色
    int minvalue=ui->scroll_color->value();
    int maxvalue=ui->score_color_max->value();

    //画出来一个QPixMap
    int width=765;
    int height=50;
    QPixmap colorMap=QPixmap(width,height);
    colorMap.fill(Qt::white);

    QPainter painter(&colorMap);

    int colorBarLength=255;//设置颜色条的长度                                                                   //得到255个颜色
//    int color_len=width/colorBarLength;
    int color_len=3;
    QMap<int,QPoint> cordmap;

    for(int i=0;i<colorBarLength;i++)// gray
    {
       //color.setRgbF(i/colorBarLength,i/colorBarLength,i/colorBarLength);//也可以使用这种方法
//        qDebug()<<QString("i:%01 min:%02 max:%03 color:%04").arg(i).arg(minvalue).arg(maxvalue).arg((i*1.0+1)/colorBarLength*(maxvalue-minvalue)+minvalue);
        QColor color;
        QRect section;
        color.setHsv(0,0,(i*1.0)/(colorBarLength-1)*(maxvalue-minvalue)+minvalue);
        section.setRect(i*color_len,0,color_len,height);
        cordmap.insert(i,section.center());                                     //把色块的中心点坐标记录下来
        painter.fillRect(section,color);
    }
    cv::Mat graymap=Tools::qImageToCvMat(colorMap.toImage());
    cv::Mat distMap;
    cv::applyColorMap(graymap,distMap,colorType);
    QPixmap colorMapColor=QPixmap::fromImage(Tools::cvMatToQImage(distMap));
    ui->label_color->setPixmap(colorMapColor);
    QImage colrMapImage=colorMapColor.toImage();
    for(int i=0;i<colorBarLength;++i)
    {
        QPoint pt=cordmap.value(i);
        QColor rgb=colrMapImage.pixelColor(pt);
//        qDebug()<<QString("i:%01 b:%02 g:%03 r:%04").arg(i).arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
        m_colMap.insert(i,rgb);
    }
    for(auto itercard:m_CardWidgets)
    {
        itercard->setRgbMap(m_colMap);
//        itercard->updateScene();
    }

}

void MainWindow::update_mixColor(double)
{
    double mixValuesr=ui->doubleSpinBox_alpha_sr->value();
    double mixValueres=ui->doubleSpinBox_alpha_res->value();
    double mixValuegmma=ui->doubleSpinBox_gamma->value();
    for(auto iter:m_CardWidgets)
    {
        iter->setCompareType(m_comparetype);
        iter->setMixSrAlpha(mixValuesr);
        iter->setMixResAlpha(mixValueres);
        iter->setMixPictureGamma(mixValuegmma);
    }
}

//打印日志
void MainWindow::log(const QString&logtxt)
{
    ui->textEdit->append(logtxt);
    ui->textEdit->horizontalScrollBar()->setValue(ui->textEdit->horizontalScrollBar()->maximum());
    qApp->processEvents();
}

//更新序列按钮
void MainWindow::updateSeqButtons(QSet<QString> seqDirs){
    //释放之前new的按钮
    for(auto button:m_seqButtons)
    {
        delete button;
        button=nullptr;
    }
    QStringList seqdirs=seqDirs.toList();
    seqdirs.sort();
    QGridLayout* layout=ui->gridLayout_seqbutton;
    for(auto seq:seqdirs)
    {
        QPushButton* toolButton=new QPushButton(seq,this);
        m_seqButtons.push_back(toolButton);
        layout->addWidget(toolButton);
        QObject::connect(toolButton,&QPushButton::clicked,this,&MainWindow::seqSelect_slot);
    }

}

//读取文件夹结构
QVariantMap MainWindow::readRootInfo()
{
    //第一步，先获取所有文件夹并进行排序
    QDir dir(m_rootDir);
    dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);             //过滤掉非有效目录
    dir.setSorting(QDir::DirsFirst);                            //优先排序目录
    QStringList dirnames=dir.entryList();
    dirnames.sort(Qt::CaseInsensitive);                         //对大小写不敏感
    //第二步 获取公共序列
    QSet<QString> seqDirs;
    for(auto dir:dirnames)
    {
        //获取seq目录
        QString seqDir = m_rootDir + QString("/%01").arg(dir);
        QDir dirseq(seqDir);
        dirseq.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);             //过滤掉非有效目录
        dirseq.setSorting(QDir::DirsFirst);                            //优先排序目录
        QStringList dirseqnames=dirseq.entryList();
        dirseqnames.sort(Qt::CaseInsensitive);                         //对大小写不敏感
        if(seqDirs.isEmpty())
        {
            seqDirs=dirseqnames.toSet();
        }
        else{
            //如果不是空的 就取交集
            seqDirs=seqDirs.intersect(dirseqnames.toSet());
        }
//        qDebug()<<QString("seq:%01  seqs:%02\n   seqdirs:%03").arg(seqDir).arg(dirseqnames.join(" ")).arg(seqDirs.toList().join(" "));
    }
    updateSeqButtons(seqDirs);
    //处理序列 假定序列中的数据数目一致 序号对应一致
    QVariantMap dirDict;
    for(auto iterdir:dirnames)                                                              //
    {
        QVariantMap seqDict;
        for(auto iterseq:seqDirs)
        {
            //序列路径
            QString seqDir = QString("%01/%02/%03").arg(m_rootDir).arg(iterdir).arg(iterseq);
            QDir dirseq(seqDir);
            dirseq.setFilter(QDir::Files);             //过滤掉非有效目录
            QStringList dirseqnames=dirseq.entryList();
            dirseqnames.sort();
            QStringList filenames;
            for(auto iterf:dirseqnames)
            {
                filenames.push_back(QString("%01/%02").arg(seqDir).arg(iterf));
            }
            seqDict.insert(iterseq,QVariant(filenames));
//            log(QString("seqDir:%01 %02").arg(seqDir).arg(dirseqnames.join(" ")));
        }
        dirDict.insert(iterdir,QVariant(seqDict));
    }
    m_rootStructMap=dirDict;
    return m_rootStructMap;
}
