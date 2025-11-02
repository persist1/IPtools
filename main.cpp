#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置UTF-8编码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    MainWindow w;
    w.show();
    return a.exec();
}

