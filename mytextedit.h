#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>
#include <QDebug>
#include <QKeyEvent>

class MyTextEdit : public QTextEdit
{
        Q_OBJECT
public:
    explicit MyTextEdit(QFrame *parent = nullptr);
protected:
   virtual void keyPressEvent(QKeyEvent *event);

signals:
    void sendData(QString data);

};

#endif // MYTEXTEDIT_H
