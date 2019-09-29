#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMessageBox>
#include <QSerialPort>
#include <QTime>
#include <QSerialPortInfo>
#include <QThread>
#include <QTextCursor>
#include "serialportreader.h"
#include "mytextedit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QStringList availablePorts();
    QSerialPort::StopBits getStopBitByComboBoxIndex();
    QSerialPort::Parity getParityByComboBoxIndex();
    QString getParityNameByValue();
    double getStopBitCountByValue();
    void setSerialPortInfo();
    void showSerialPortInfo();
    bool isValidParams();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void updateData(QByteArray data);

    void updateStatus(QString info);

    void keyPressed(QChar data);
private:
    Ui::MainWindow *ui;
    QThread *readerThread = nullptr;
    SerialPortReader *serialPortReader;
    QSerialPort *serialPort = nullptr;
    int online = 0;
};
#endif // MAINWINDOW_H
