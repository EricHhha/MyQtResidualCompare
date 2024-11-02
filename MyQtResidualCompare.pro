QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyQtResidualCompare                                                                    #应用程序名称
TEMPLATE = app                                                                                  #生成目标类型

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MyQGraphicsView.cpp \
    customcard.cpp \
    image_tools.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    MyQGraphicsView.h \
    customcard.h \
    image_tools.h \
    mainwindow.h

FORMS += \
    customcard.ui \
    mainwindow.ui

TRANSLATIONS += \
    MyQtResidualCompare_zh_CN.ts


INCLUDEPATH += $$PWD   \
/usr/include/opencv4/   \
/usr/include/opencv2


#附加依赖库目录
LIBS += -L/usr/lib/x86_64-linux-gnu/


#附加依赖项
LIBS += -lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_imgcodecs
#-lopencv_*

#INCLUDEPATH += $$PWD
#用于添加头文件的包含目录。和Visual Studio中的“附加包含目录”功能一致。
#LIBS += -L$$PWD/                            #此行代码同Visual Studio中的“附加库目录”功能，用于添加库文件搜索路径
#LIBS += -lmylib                             #此行代码同Visual Studio中的“附加依赖库”功能，用于添加需要链接的库文件。可以和上面的添加库文件搜索路径写在一起，比如 LIBS += -L$$PWD -lmylib



#CONFIG += staticlib
#结合TEMPLATE = lib使用，指定生成静态库。

#CONFIG += dll
#结合TEMPLATE = lib使用，指定生成动态库。



#DEFINES += MY_MACRO
#用于添加全局需要使用的宏定义。和Visual Studio中的“预处理器定义”功能一致。

#message($$PWD)
#message是.pro中的内置函数，用于打印消息或变量，一般用于调试变量或步骤是否正确。
#输出的消息会在Qt Creator底部的“概要信息”中打印。

#配置判断
#CONFIG(debug, debug | release) {
#	# 这里写debug下的配置内容，最常见的用法是链接debug版本第三方的库
#} else {
#	# 这里写release下的配置内容，最常见的用法是链接release版本第三方的库
#}

#include(sub_module.pri)
#这个非常重要。.pri是子模块配置文件，编写方式和.pro基本一样。不一样的地方就在于，它毕竟是子模块配置文件，只需要写子模块的配置信息即可，不需要写诸如 TEMPLATE = app等这种应该在pro文件中编写的配置项。
#有的同学会问，.pri文件在哪创建？界面上怎么找不到？这个确实笔者也一直都没有找到，需要自己新建一个文件，将文件后缀改为.pri，最后在.pro文件中使用include包含进来。
#.pri的典型用法这里举一例：用来引入第三方库。一般第三方库需要提供头文件、库文件。想要引入到Qt项目中，编写一个.pri文件就比较简单，需要用到的项目直接include这个.pri文件即可。相对于Visual Studio需要到多个界面配置第三方库，简单了很多。一个引入库的pri子模块文件内容如下图所示：

include($$PWD/thirdparty.pri)



DESTDIR = $$PWD/release/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    thirdparty.pri

RESOURCES += \
    MyQtResidualCompare.qrc
