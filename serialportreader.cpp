#include "serialportreader.h"

SerialPortReader::SerialPortReader(QSerialPort *serialPort, QObject *parent)
    : QObject(parent), serialPort(serialPort)
{
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortReader::handleReadyRead);
    connect(serialPort, &QSerialPort::errorOccurred, this, &SerialPortReader::handleError);
    connect(&timer, &QTimer::timeout, this, &SerialPortReader::handleTimeout);

    //Каждые 5000 мс будет посылаться сигнал QTimer::timeout и выполняться его обработчик handleTimeout
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

//Обработчик сигнала QTimer::timeout
void SerialPortReader::handleTimeout()
{
    if (readData.isEmpty())
    {
        QString status = QObject::tr("No data was currently available "
                                        "for reading from port %1")
                            .arg(serialPort->portName());
        emit statusUpdated(status);
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
