#include "serialportreader.h"

SerialPortReader::SerialPortReader(QSerialPort *serialPort, QObject *parent)
    : QObject(parent), serialPort(serialPort)
{
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortReader::handleReadyRead);
    connect(serialPort, &QSerialPort::errorOccurred, this, &SerialPortReader::handleError);

    timer.start(5000);
}

//Обработчик сигнала появления новых данных в канале QSerialPort::readyRead
void SerialPortReader::handleReadyRead()
{
    readData = serialPort->readAll();

    //Посылаем сигнал вместе с прочитанными данными в MainWindow
    emit dataReceived(readData);

    //Если таймер не идёт
    if (!timer.isActive())
    {
        timer.start(5000);
    }
}

//Обработчик сигнала QSerialPort::errorOccured
void SerialPortReader::handleError(QSerialPort::SerialPortError serialPortError)
{
    QString status;
    switch(serialPortError)
    {
    case QSerialPort::ParityError :
    {
        status = QObject::tr("Parity error occurred while reading "
                                        "the data from port %1, error: %2")
                            .arg(serialPort->portName())
                            .arg(serialPort->errorString());
        break;
    }
    case QSerialPort::ReadError :
    {
        status = QObject::tr("An I/O error occurred while reading "
                                        "the data from port %1, error: %2")
                            .arg(serialPort->portName())
                            .arg(serialPort->errorString());
        break;
    }
    case QSerialPort::FramingError :
    {
        status = QObject::tr("Framing error occurred while reading "
                             "the data from port %1, error: %2")
                 .arg(serialPort->portName())
                 .arg(serialPort->errorString());
        break;
    }
    }


    emit statusUpdated(status);
}
