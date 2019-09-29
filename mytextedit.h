#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class MyTextEdit : public QTextEdit
{
        Q_OBJECT
public:
    explicit MyTextEdit(QFrame *parent = nullptr);
protected:
   virtual void keyPressEvent(QKeyEvent *event);

signals:
    void sendData(QChar data);

};

#endif // MYTEXTEDIT_H
