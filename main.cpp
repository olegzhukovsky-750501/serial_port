#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Если это второй экземпляр программы
    if(a.arguments().contains("--app2"))
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
    //Запуск независимой копии процесса
    QProcess::startDetached(QCoreApplication::applicationFilePath(), QStringList{"--app2"});
    MainWindow w;
    w.show();
    return a.exec();
}
