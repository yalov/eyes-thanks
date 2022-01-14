#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtWidgets>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

public slots:
   void Button_click(bool);

private:
   QPushButton * bp;
   QTextEdit * edit;
   QLabel * label;


};

#endif // WIDGET_H
