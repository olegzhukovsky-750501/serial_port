#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iomanip>
#include <QProcess>
#include <QMessageBox>
#include <QSerialPort>
#include <QTime>
#include <QBitArray>
#include <QSerialPortInfo>
#include <QThread>
#include <QTextCursor>
#include <bitset>
#include <iostream>
#include <sstream>
#include "serialportreader.h"
#include "mytextedit.h"
#include "packet.h"

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
    QByteArray bitStuffing(QByteArray frame, QString before, QString after);
    QByteArray deBitStuffing(QByteArray frame, QString before, QString after);
    QString convertFromQByteArrayToQString(QByteArray byteArray);
    QByteArray convertFromQBitArrayToQByteArray(QBitArray bitArray);
    template<std::size_t N>
    QByteArray convertFromBitsetToQByteArray(std::bitset<N> &b);
    bool fillPacketControlInfo();
    int randomBetween(int low, int high);
    char crc8(char const message[]);
    char detectError(char const message[]);
    void rotateLeft(std::bitset<88>& b, unsigned m);
    void rotateRight(std::bitset<88>& b, unsigned m);
    void outputQByteArrayInHex(QByteArray b);


private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void updateData(QByteArray packet);

    void updateStatus(QString info);

    void keyPressed(QString data);
    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QThread *readerThread = nullptr;
    SerialPortReader *serialPortReader;
    QSerialPort *serialPort = nullptr;
    int online = 0;
    Packet packet;

};
#endif // MAINWINDOW_H
