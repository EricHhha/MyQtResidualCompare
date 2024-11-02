#INCLUDEPATH += $$PWD
#用于添加头文件的包含目录。和Visual Studio中的“附加包含目录”功能一致。
#LIBS += -L$$PWD/                            #此行代码同Visual Studio中的“附加库目录”功能，用于添加库文件搜索路径
#LIBS += -lmylib                             #此行代码同Visual Studio中的“附加依赖库”功能，用于添加需要链接的库文件。可以和上面的添加库文件搜索路径写在一起，比如 LIBS += -L$$PWD -lmylib

INCLUDEPATH += $$PWD \
/usr/include/opencv4/   \
/usr/include/opencv2


#附加依赖库目录
LIBS += -L/usr/lib/x86_64-linux-gnu/


#附加依赖项
LIBS += -lopencv_core \
-lopencv_highgui
-lopencv_imgproc
-lopencv_imgcodecs
#-lopencv_core
