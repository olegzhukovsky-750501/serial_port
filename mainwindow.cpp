#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Заполнение выпадающего списка всеми доступными портами в системе
    QStringList ports = availablePorts();
    ui->comboBox->insertItems(0, ports);

    //Заполнение выпадающего списка стандартными размерами байта
    QStringList stdByteSizes = {"5", "6", "7", "8"};
    ui->comboBox_3->addItems(stdByteSizes);

    //Заполнение выпадающего списка стандартными скоростями(бод/c)
    QStringList stdBauds = {"75", "110", "300", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    ui->comboBox_4->addItems(stdBauds);

    //Заполнение выпадающего списка режимами четности
    QStringList stdParities = {"NoParity", "EvenParity", "OddParity", "SpaceParity", "MarkParity"};
    ui->comboBox_2->addItems(stdParities);

    //Заполнение выпадающего списка стоп битами
    QStringList stdStopBits = {"1", "1.5", "2"};
    ui->comboBox_5->addItems(stdStopBits);

    //Установка стандартных параметров COM-порта в выпадающих списке
    ui->comboBox_2->setCurrentText("NoParity");
    ui->comboBox_3->setCurrentText("8");
    ui->comboBox_4->setCurrentText("9600");
    ui->comboBox_5->setCurrentText("1");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStringList MainWindow::availablePorts()
{
    QStringList ports;

    for(QSerialPortInfo port : QSerialPortInfo::availablePorts())
    {
        ports += port.portName();
    }

    return ports;
}

QSerialPort::StopBits MainWindow::getStopBitByComboBoxIndex()
{
    switch(ui->comboBox_5->currentIndex())
    {
    case 0 : return QSerialPort::OneStop;
    case 1 : return QSerialPort::OneAndHalfStop;
    case 2 : return QSerialPort::TwoStop;
    }
}

QSerialPort::Parity MainWindow::getParityByComboBoxIndex()
{
    switch(ui->comboBox_2->currentIndex())
    {
    case 0 : return QSerialPort::NoParity;
    case 1 : return QSerialPort::EvenParity;
    case 2 : return QSerialPort::OddParity;
    case 3 : return QSerialPort::SpaceParity;
    case 4 : return QSerialPort::MarkParity;
    }
}

//Получение названия режима четности соответствующего значению currParity в enum QSerialPort::Parity
QString MainWindow::getParityNameByValue()
{
    int currParity = serialPort->parity();

    switch(currParity)
    {
    case 0 : return "NoParity";
    case 2 : return "EvenParity";
    case 3 : return "OddParity";
    case 4 : return "SpaceParity";
    case 5 : return "MarkParity";
    }
}

//Получение значения стоп бита соответствующего значению stopBitValue в enum QSerialPort::StopBits
double MainWindow::getStopBitCountByValue()
{
    int stopBitValue = serialPort->stopBits();
    switch(stopBitValue)
    {
    case 1 : return 1;
    case 3 : return 1.5;
    case 2 : return 2;
    }
}

//Установка значений параметров последовательного порта из выбранных в выпадающих списках
void MainWindow::setSerialPortInfo()
{
    QSerialPort::DataBits dataBit = static_cast<QSerialPort::DataBits>(ui->comboBox_3->currentText().toInt());
    serialPort->setDataBits(dataBit);

    serialPort->setBaudRate(ui->comboBox_4->currentText().toInt());

    QSerialPort::Parity parity = getParityByComboBoxIndex();
    serialPort->setParity(parity);

    QSerialPort::StopBits stopBit = getStopBitByComboBoxIndex();
    serialPort->setStopBits(stopBit);

}

//Вывод информации о последовательном порте в Debug область приложения
void MainWindow::showSerialPortInfo()
{
    ui->textEdit_3->setText("Connected to: " + serialPort->portName() + " port");

    int currDataBits = serialPort->dataBits();
    ui->textEdit_3->append("Byte size: " + QString::number(currDataBits));


    int baudRate = serialPort->baudRate();
    ui->textEdit_3->append("Speed: " + QString::number(baudRate));

    QString parityName = getParityNameByValue();
    ui->textEdit_3->append("Parity: " + parityName);


    double currStopBit = getStopBitCountByValue();
    ui->textEdit_3->append("Stop bits: " + QString::number(currStopBit));
}

//Проверка валидности комбинации количества стоп битов и размера байта
bool MainWindow::isValidParams()
{
    if(ui->comboBox_3->currentText() == "5" &&
       ui->comboBox_5->currentText() == "2")
    {
        QMessageBox msgBox;
        msgBox.setText("The use of 5 data bits with 2 stop bits is an invalid combination");
        msgBox.exec();
        return false;
    }
    else if(ui->comboBox_5->currentText() == "1.5" &&
            ui->comboBox_3->currentText() != "5")
    {
        QMessageBox msgBox;
        msgBox.setText("The use of 6, 7, 8 data bits with 1.5 stop bits is an invalid combination");
        msgBox.exec();
        return false;
    }
    return true;
}

//Произведено нажатие на кнопку Open
void MainWindow::on_pushButton_2_clicked()
{
    //Если не выбран ни один порт либо сочетания параметров не корректны
    if((ui->comboBox->currentIndex() < 0) ||
       !isValidParams())
    {
        return;
    }

    if(serialPort != nullptr)
    {
        serialPort->close();
    }


    serialPort = new QSerialPort(ui->comboBox->currentText(), nullptr);

    online = serialPort->open(QIODevice::ReadWrite);

    //Если последовательный порт успешно был открыт
    if(online)
    {
        setSerialPortInfo();
        showSerialPortInfo();

        //Создание потока чтения COM-порта
        readerThread = new QThread;
        //Создание класса читателя COM-порта, реализующего функционал потока readerThread
        serialPortReader = new SerialPortReader(serialPort);
        serialPortReader->moveToThread(readerThread);

        connect(serialPortReader, SIGNAL(dataReceived(QByteArray)), this, SLOT(updateData(QByteArray)));
        connect(serialPortReader, SIGNAL(statusUpdated(QString)), this, SLOT(updateStatus(QString)));
        readerThread->start();
    }
    else
    {
        ui->textEdit_3->setText("Can't connect to " + serialPort->portName() + " port");
    }
}

//Произведено нажатие на кнопку Apply
void MainWindow::on_pushButton_clicked()
{
    if(online == NULL)
    {
        QMessageBox msgBox;
        msgBox.setText("No opened ports");
        msgBox.exec();
        return;
    }

    if(isValidParams())
    {
        setSerialPortInfo();
        showSerialPortInfo();
    }
}

void MainWindow::updateData(QByteArray data)
{
    QString strData = QString::fromLocal8Bit(data.data());

    ui->textEdit_2->setText(strData);
}

void MainWindow::updateStatus(QString info)
{
    //Добавить текст в конец
    ui->textEdit_3->append(info);
}

void MainWindow::on_textEdit_textChanged()
{
    if(online)
    {
        QByteArray writeData = ui->textEdit->toPlainText().toLocal8Bit();
        //Запись данных в COM-порт
        const qint64 bytesWritten = serialPort->write(writeData);

        if (bytesWritten == -1)
        {
            ui->textEdit_3->append(QObject::tr("Failed to write the data to port %1, error: %2")
                                .arg(serialPort->portName())
                                .arg(serialPort->errorString()));
        } else if (bytesWritten != writeData.size())
        {
            ui->textEdit_3->append(QObject::tr("Failed to write all the data to port %1, error: %2")
                                .arg(serialPort->portName())
                                .arg(serialPort->errorString()));
        }
    }
}
