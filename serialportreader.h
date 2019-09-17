#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QDebug>

class SerialPortReader : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortReader(QSerialPort *serialPort, QObject *parent = nullptr);
private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);
signals:
    void dataReceived(QByteArray data);
    void statusUpdated(QString status);
private:
    QSerialPort *serialPort = nullptr;
    QByteArray readData;
    QTimer timer;
};

#endif // SERIALPORTREADER_H
