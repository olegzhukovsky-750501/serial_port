#include "mytextedit.h"

MyTextEdit::MyTextEdit(QFrame *frame)
    :QTextEdit(frame)
{

}

void MyTextEdit::keyPressEvent(QKeyEvent *event)
{
    QString oldText = toPlainText();
    QString str = event->text();

    int curPos = oldText.length();

    //Разрешаем ввод символов только от пробела до тильды
    if ((str >= " ") && (str <= "~"))
    {
        setText(oldText+str);

        QTextCursor cursor = textCursor();
        //Устанавливаем курсор в конец блока текста
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
        setTextCursor(cursor);

        emit sendData(str.at(0));
    }
}
