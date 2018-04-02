#include "widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);

    edit = new QTextEdit();
    layout->addWidget(edit);
    bp = new QPushButton("Button");
    layout->addWidget(bp);

    connect(bp, SIGNAL(clicked(bool)),this,SLOT(Button_click(bool)));


}

inline QString str_from(QRect r)
{
    return QString("(%1,%2)[%3x%4]").arg(r.x(), 5).arg(r.y(), 5).arg(r.width(), 4).arg(r.height(), 4);
}

QString f1()
{
    return str_from(QApplication::desktop()->geometry());
}

void Widget::Button_click(bool)
{
    edit->append(f1());

}



Widget::~Widget()
{


}
