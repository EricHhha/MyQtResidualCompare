#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariantMap>
#include <QToolButton>
#include <QPushButton>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class CustomCard;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void initConnect();
protected slots:
    void opendir_slot();
    void last_slot();
    void next_slot();
    void seqSelect_slot();
    void saveResults_slot();
    void update_color();
    void update_bcolor(int);
    void compareModeChangeSlot();
    void update_mixColor(double);
    void update_rectangle(int);
    void update_mixState(bool);
private:
    QVariantMap readRootInfo();
    void updateSeqButtons(QSet<QString> seqDirs);
    void log(const QString&logtxt);
    bool isCanMove();
    void updateFrames();
    void setGtFileFrame(QString gtfile);
    void seq_last_slot();
    void seq_next_slot();
    bool isCanSeqMove();
    void updateResults(QList<QPair<QString,QString>> exp_results);
protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;
    QList<QPushButton*> m_seqButtons;
    QList<CustomCard*> m_CardWidgets;
    QMap<int,QColor> m_colMap;
    QPixmap m_gt_map;
    QString m_gt_path;
    QString m_rootDir;
    QString m_curSeq;
    QPixmap m_colorLegend;                  //色卡
    int m_rectangleValue=200;               //矩形框的大小
    int m_curIdx=0;
    int m_seqIdx=0;
    int m_comparetype=0;                    //0是Y 1是RGB 默认是Y
    double m_scale=1.0;
    QVariantMap m_rootStructMap;
};
#endif // MAINWINDOW_H
