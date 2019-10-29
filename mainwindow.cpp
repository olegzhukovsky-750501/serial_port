#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Установка валидаторов на ввод адресов. Разрешен ввод только чисел от 0 до 255.
    ui->lineEdit->setValidator(new QIntValidator(0, 255, this));
    ui->lineEdit_2->setValidator(new QIntValidator(0, 255, this));

    //Соединие сигнала нажатия клавиши в поле Input со слотом записи данных в порт
    connect(ui->textEdit, SIGNAL(sendData(QString)), this, SLOT(keyPressed(QString)));

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

    //Установка стандартных параметров COM-порта в выпадающих списках
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

//Получение названия режима четности соответствующего значению в enum QSerialPort::Parity
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

//Получение значения стоп бита соответствующего значению в enum QSerialPort::StopBits
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
    int currDataBits = serialPort->dataBits();
    ui->textEdit_3->append("Byte size: " + QString::number(currDataBits));


    int baudRate = serialPort->baudRate();
    ui->textEdit_3->append("Speed: " + QString::number(baudRate));

    QString parityName = getParityNameByValue();
    ui->textEdit_3->append("Parity: " + parityName);


    double currStopBit = getStopBitCountByValue();
    ui->textEdit_3->append("Stop bits: " + QString::number(currStopBit) + "\n");
}

//Проверка валидности комбинации количества стоп битов и размера байта
bool MainWindow::isValidParams()
{
    if(ui->comboBox_3->currentText() == "5" &&
       ui->comboBox_5->currentText() == "2")
    {
        ui->textEdit_3->append("The use of 5 data bits with 2 stop bits is an invalid combination");
        return false;
    }
    else if(ui->comboBox_5->currentText() == "1.5" &&
            ui->comboBox_3->currentText() != "5")
    {
        ui->textEdit_3->append("The use of 6, 7, 8 data bits with 1.5 stop bits is an invalid combination");
        return false;
    }
    return true;
}

QByteArray MainWindow::bitStuffing(QByteArray frame, QString before, QString after)
{
    QByteArray frameWithoutFlag(frame);
    frameWithoutFlag.remove(0, 1);

    QString bitsStr = convertFromQByteArrayToQString(frameWithoutFlag);

    bitsStr.replace(before, after);

    QBitArray bits(bitsStr.size());

    for(int i = 0; i<bitsStr.size(); i++ )
    {
        if (bitsStr.at(i) == '0')
        {
            bits[i]=false;
        }
        else
        {
            bits[i]=true;
        }
    }

    QByteArray resultFrame = convertFromQBitArrayToQByteArray(bits);

    resultFrame.insert(0, frame.at(0));

    return resultFrame;
}



QString MainWindow::convertFromQByteArrayToQString(QByteArray byteArray)
{
    QString bitsStr;
    for(int i=0; i<byteArray.count(); ++i)
    {
        std::bitset<8> fieldBits(byteArray.at(i));
        bitsStr += QString::fromStdString(fieldBits.to_string());
    }
    return bitsStr;
}

QByteArray MainWindow::convertFromQBitArrayToQByteArray(QBitArray bitArray)
{
    QByteArray byteArray;
    for(int b=0; b<bitArray.count();++b) {
        byteArray[b/8] = (byteArray.at(b/8) | ((bitArray[b]?1:0)<<(7-(b%8))));
    }
    return byteArray;
}

bool MainWindow::fillPacketControlInfo()
{
    if(ui->lineEdit->text() == ui->lineEdit_2->text())
    {
        updateStatus("Destination address and source cannot be same");
        return false;
    }
    packet.source_adr = ui->lineEdit_2->text().toInt();
    packet.destination_adr = ui->lineEdit->text().toInt();
    return true;
}

int MainWindow::randomBetween(int low, int high)
{
    return (qrand() % ((high +1) - low) + low);
}

char MainWindow::crc8(char const message[])
{
    char remainder = 0;

    for (int byte = 0; byte < 10; ++byte)
    {
        remainder ^= message[byte];
        for(int bit = 8; bit > 0; --bit)
        {
            if(remainder & 0x80)
            {
                remainder = (remainder << 1) ^ 0x07;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

    }
    return remainder;
}

char MainWindow::detectError(const char message[])
{
    char remainder = 0;

    for (int byte = 0; byte < 11; ++byte)
    {
        remainder ^= message[byte];
        for(int bit = 8; bit > 0; --bit)
        {
            if(remainder & 0x80)
            {
                remainder = (remainder << 1) ^ 0x07;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

    }
    return remainder;
}

void MainWindow::rotateLeft(std::bitset<88> &b, unsigned m)
{
    b = b >> m | b << (88 - m);
}

void MainWindow::rotateRight(std::bitset<88> &b, unsigned m)
{
    b = b << m | b >> (88 - m);
}

void MainWindow::outputQByteArrayInHex(QByteArray b)
{
    std::stringstream res;
    for(int i=0; i< b.count(); ++i)
    {
        std::bitset<8> fieldBits(b.at(i));
        res << std::hex << std::uppercase << fieldBits.to_ulong() << " ";
    }
    updateStatus(QString::fromStdString(res.str()));
}

//Произведено нажатие на кнопку Connect/Disconnect
void MainWindow::on_pushButton_2_clicked()
{
    //Если произошло нажатие на Disconnect
    if(online)
    {
        ui->comboBox->setEnabled(1);
        ui->textEdit->setEnabled(0);
        ui->textEdit->clear();
        ui->textEdit_2->clear();

        serialPort->close();
        readerThread->exit();

        updateStatus(QObject::tr("Succesfully disconnected from port %1").arg(serialPort->portName()));
        ui->pushButton_2->setText("Connect");

        online = 0;
    }
    else
    {
        //Если не выбран ни один порт либо сочетания параметров не корректны
        if((ui->comboBox->currentIndex() < 0) ||
           !isValidParams())
        {
            return;
        }
        if(!(ui->lineEdit->text().isEmpty()) &&
           !(ui->lineEdit_2->text().isEmpty()))
        {
            fillPacketControlInfo();
            ui->textEdit->setEnabled(true);
        }

        serialPort = new QSerialPort(ui->comboBox->currentText(), nullptr);

        online = serialPort->open(QIODevice::ReadWrite);

        //Если последовательный порт был успешно открыт
        if(online)
        {
            setSerialPortInfo();

            //Создание потока чтения COM-порта
            readerThread = new QThread;
            //Создание класса читателя COM-порта, реализующего функционал потока readerThread
            serialPortReader = new SerialPortReader(serialPort);
            serialPortReader->moveToThread(readerThread);

            connect(serialPortReader, SIGNAL(dataReceived(QByteArray)), this, SLOT(updateData(QByteArray)));
            connect(serialPortReader, SIGNAL(statusUpdated(QString)), this, SLOT(updateStatus(QString)));
            readerThread->start();

            ui->pushButton_2->setText("Disconnect");
            ui->comboBox->setEnabled(0);

            updateStatus("Connected to: " + serialPort->portName() + " port");
            showSerialPortInfo();
        }
        else
        {
            updateStatus("Can't connect to " + serialPort->portName() + " port");
        }
    }
}

//Произведено нажатие на кнопку Apply
void MainWindow::on_pushButton_clicked()
{
    if(online == NULL)
    {
        updateStatus("No opened ports");
        return;
    }

    if(isValidParams())
    {
        setSerialPortInfo();
        updateStatus("Settings succesfully updated to " + serialPort->portName());
        showSerialPortInfo();
    }
}

QByteArray MainWindow::deBitStuffing(QByteArray frame, QString before, QString after)
{
    QByteArray deBitStuffedPacket = bitStuffing(frame, before, after);
    if(deBitStuffedPacket.length() >= 12)
    {
       return deBitStuffedPacket.remove(11, 1);
    }
    return deBitStuffedPacket;
}

//Вывод полученных данных в Output
void MainWindow::updateData(QByteArray receivedPacket)
{
    QByteArray resultFrame = deBitStuffing(receivedPacket, "00001111", "0000111");

    char remainder = detectError(resultFrame);

    //updateStatus(QString::number(remainder));

    if(remainder)
    {
        updateStatus("Data error(CRC): "  + QString::number((unsigned char)remainder));

        //std::bitset<8> remainderBits(remainder);
        //updateStatus(QString::fromStdString(remainderBits.to_string()));

        std::bitset<88> resultFrameBits;
//        for(int i =0;i < resultFrame.length(); ++i)
//        {
//            char curr = resultFrame.at(resultFrame.length() - 1 - i);
//            for(int bit = 0; bit < 8 && curr; ++bit)
//            {
//                if(curr & 0x1)
//                {
//                    resultFrameBits.set(8 * i + bit);
//                }
//                curr >>=1;
//            }
//        }

        for(int i =0;i < resultFrame.length(); ++i)
        {
            char curr = resultFrame.at(i);
            for(int bit = 7; bit >= 0 && curr; --bit)
            {
                if(curr & 0x1)
                {
                    resultFrameBits.set(8 * i + bit);
                }
                curr >>=1;
            }
        }

//        std::bitset<88> extendedRemainderBits;

//        for(int i = 7;  i >=0; --i)
//        {
//            extendedRemainderBits.set(87 - i, remainderBits[i]);
//        }

        QByteArray temp = convertFromBitsetToQByteArray(resultFrameBits);

        for(int i = 0; i < 88; i++)
        {
            resultFrameBits.flip(i);

            char remainder = detectError(convertFromBitsetToQByteArray(resultFrameBits));

            if(remainder == 0)
            {
                resultFrame = convertFromBitsetToQByteArray(resultFrameBits);
                break;
            }
            else
            {
                resultFrameBits.flip(i);
            }
        }


//        int shiftN = 0;
//        std::bitset<88> resultCombination;
//        while(1)
//        {
//            if(extendedRemainderBits.count() <= 1)
//            {
//                resultCombination = resultFrameBits ^ extendedRemainderBits;
//                updateStatus(QString::number(shiftN));
//                for(int i = 0; i < shiftN; i++)
//                {
//                   rotateLeft(resultCombination, 1);
//                }
//                updateStatus(QString::fromStdString(resultCombination.to_string()));
//                resultFrame = convertFromBitsetToQByteArray(resultCombination);
//                break;
//            }
//            else
//            {
//                shiftN++;
//                rotateRight(resultFrameBits, 1);
//                remainder = detectError(convertFromBitsetToQByteArray(resultFrameBits));

//                updateStatus(QString::number(remainder));

//                updateStatus(QString::fromStdString(resultFrameBits.to_string()));

//                std::bitset<8> remainderBits(remainder);

//                extendedRemainderBits.reset();
//                for(int i = 7;  i  >= 0; --i)
//                {
//                    extendedRemainderBits.set(88 - i, remainderBits[i]);
//                }

//                updateStatus(QString::fromStdString(extendedRemainderBits.to_string()));
//            }
//        }
    }

    if((resultFrame.at(10) == 1)
            || ((unsigned char)(resultFrame.at(1)) != packet.source_adr))
    {
        return;
    }

    char data[8];
    for(int i = 3; i <10; i++)
    {
        data[i-3] = resultFrame.at(i);
    }
    data[7] = '\0';

    QString strData = QString::fromLocal8Bit(data);

    ui->textEdit_2->insertPlainText(strData);
}

//Вывод информации в Debug
void MainWindow::updateStatus(QString info)
{
    QTime time;
    //Добавить текст в конец
    ui->textEdit_3->append(time.currentTime().toString() + ": " + info);
}

void MainWindow::keyPressed(QString data)
{
    if(online)
    {
        for(int i = 0; i < 7; i++)
        {
            packet.data[i] = data.at(i).toLatin1();
        }

        char rawPacket[11];
        rawPacket[0] = packet.flag;
        rawPacket[1] = packet.destination_adr;
        rawPacket[2] = packet.source_adr;
        for(int i = 3; i < 10; i++)
        {
            rawPacket[i] = packet.data[i-3];
        }

        packet.fcs = crc8(rawPacket);

        rawPacket[10] = packet.fcs;

        QByteArray frame = QByteArray::fromRawData(rawPacket, 11);

        //outputQByteArrayInHex(frame);

        if(ui->checkBox->isChecked())
        {
            qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);

            int bitNum = randomBetween(0, ((frame.length() - 1) * 8) - 1);

            int byteNum = bitNum / 8;

            char byteToReplace = frame.at(byteNum) ^ (1 << (7 - (bitNum - (byteNum * 8))));

            frame.replace(byteNum, 1, &byteToReplace, 1);
        }

        //outputQByteArrayInHex(frame);

        QByteArray resultFrame = bitStuffing(frame, "0000111", "00001111");

        outputQByteArrayInHex(resultFrame);

        //Запись данных в COM-порт

        const qint64 bytesWritten = serialPort->write(resultFrame);
        if (bytesWritten == -1)
        {
            updateStatus(QObject::tr("Failed to write the data to port %1, error: %2")
                                .arg(serialPort->portName())
                                .arg(serialPort->errorString()));
        } else if (bytesWritten != resultFrame.length())
        {
            updateStatus(QObject::tr("Failed to write all the data to port %1, error: %2")
                                .arg(serialPort->portName())
                                .arg(serialPort->errorString()));
        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if((ui->lineEdit->text().isEmpty()) || (ui->lineEdit_2->text().isEmpty()))
    {
        updateStatus("All address fields must be filled");
        return;
    }

    if(!fillPacketControlInfo())
    {
        return;
    }

    if(online)
    {
        ui->textEdit->setEnabled(1);
    }
    updateStatus("Addresses succesfully applied");
}

template<std::size_t N>
QByteArray MainWindow::convertFromBitsetToQByteArray(std::bitset<N> &bits)
{
    QByteArray byteArray;
    for(int b=0; b < N;++b) {
        byteArray[b/8] = (byteArray.at(b/8) | ((bits[b]?1:0)<<(7-(b%8))));
    }
    return byteArray;
}
