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
        QString newText = oldText + str;
        setText(newText);

        QTextCursor cursor = textCursor();
        //Устанавливаем курсор в конец блока текста
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
        setTextCursor(cursor);

        if(((newText.length()) % 7) == 0)
        {
            emit sendData(newText.mid(cursor.position() - 7, 7));
        }
    }
}
