#ifndef OUTPUTCLASS_H
#define OUTPUTCLASS_H

#include <QObject>

class OutputClass : public QObject
{
    Q_OBJECT
public:
    explicit OutputClass(QObject *parent = nullptr);
    void receiveData();
};

#endif // OUTPUTCLASS_H
