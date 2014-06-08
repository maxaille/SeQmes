#include "SeQmes.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    QString locale = QLocale::system().name().section('_', 0, 0);
    translator.load("SeQmes_"+locale);
    a.installTranslator(&translator);
    MainWindow M;
    //SeQmes S;
    
    return a.exec();
}
