#include "widget.h"

QString XmlReader()
{
    QString s = "";
    QFile file("c:/Users/User/Repositories/eyes-thanks/test.xml");
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(file.readAll());
        file.close();

        while (!xml.atEnd()) {
            xml.readNextStartElement();

            if (xml.name().compare(QString("characters")))
            {
                s += xml.readElementText();
                qDebug() << "text: " << s;
                qDebug() << "name: " << xml.name();
                qDebug() << "text: "<< xml.readElementText();
                qDebug() << "count: " << xml.readElementText().count();

            }
        }
    }

    return s;
}

QString GraphemeCounter(QString s)
{
    QString out = "";
    out = "String: " + s + "\n";

    out += QString("Code unit count       : %1\n").arg(s.length());

    QTextBoundaryFinder tbf(QTextBoundaryFinder::Grapheme, s);
    int count=0;
    while(tbf.toNextBoundary()!=-1) ++count;

    auto ucs4 = s.toUcs4();

    out += QString("QTextBoundaryFinder::Grapheme count: %1\n").arg(count);
    out += QString("toUcs4 count: %1\n").arg(ucs4.size());

    out += "every second character: ";
    for (int i=0 ;i< ucs4.count(); i=i+2)
    {
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        out += QChar::fromUcs4(ucs4[i]);
#else
        out += QString::fromUcs4( &ucs4[i],1);
#endif
        out += " ";
    }
    out += "\n";

    return out;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);

    edit = new QTextEdit();
    layout->addWidget(edit);
    bp = new QPushButton("Button");
    layout->addWidget(bp);
    label = new QLabel();
    layout->addWidget(label);

    connect(bp, SIGNAL(clicked(bool)),this,SLOT(Button_click(bool)));
}

void Widget::Button_click(bool)
{
    QString s = edit->toPlainText();
    label->setText(GraphemeCounter(s));

    //label->setText(label->text() + s);

    //QString a = "a" + QString::fromUtf16(u"\U0001D161") + "a";
    //GraphemeCounter(a);

    //QString  b = QString::fromUtf16(u"𝕬𝕭𝕮𝕯𝕰𝕱𝕲𝕳𝕴𝕵𝕶𝕷𝕸𝕹𝕺𝕻𝕼𝕽𝕾𝕿𝖀𝖁𝖂𝖃𝖄𝖅𝖆𝖇𝖈𝖉𝖊𝖋𝖌𝖍𝖎𝖏𝖐𝖑𝖒𝖓𝖔𝖕𝖖𝖗𝖘𝖙𝖚𝖛𝖜𝖝𝖞𝖟");
    //GraphemeCounter(b);

    //const QString c=QString::fromUtf8(u8"abc\U00010139def\U00010102g");
    //GraphemeCounter(c);

}



Widget::~Widget()
{
}
