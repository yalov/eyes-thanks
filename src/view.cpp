//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "view.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QOpenGLWidget>
#include "testitem.h"
#include <QTextFormat>
#include <QTextCursor>

View::View(QWidget *parent): QGraphicsView(parent),
    clockItem(nullptr), textItem(nullptr), Item(nullptr), ElapsedTimerDot(nullptr), Method(-1), IsBackgroundUpdate(false), RunnedFirstTime(false),
    testitem(nullptr)
{
#ifdef DEPLOY
    setWindowFlags(Qt::WindowStaysOnTopHint);
#endif

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    setAttribute(Qt::WA_DeleteOnClose);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QGraphicsView { border-style: none; }");

    myscene = new QGraphicsScene();
    setScene(myscene);


    ProgressBarText = new GraphicsTextItemFixBound();
    QFont font;
    font.setPointSize(16);
    ProgressBarText->setFont(font);
    ProgressBarText->setDefaultTextColor(Qt::black);
    ProgressBarText->setZValue(3);
    ProgressBarText->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    myscene->addItem(ProgressBarText);

    ProgressBar = new QGraphicsRectItem();
    ProgressBar->setPen(Qt::NoPen);
    ProgressBar->setBrush(Qt::white);
    ProgressBar->setOpacity(0.8);
    ProgressBar->setZValue(2);
    ProgressBar->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    myscene->addItem(ProgressBar);

    ProgressBarBackground = new QGraphicsRectItem();
    ProgressBarBackground->setPen(Qt::NoPen);
    ProgressBarBackground->setBrush(Qt::white);
    ProgressBarBackground->setOpacity(0.4);
    ProgressBarBackground->setZValue(1);
    myscene->addItem(ProgressBarBackground);

    ProgressBarBound = new QGraphicsRectItem();
    ProgressBarBound->setPen(QPen(Qt::black));
    ProgressBarBound->setBrush(Qt::NoBrush);
    ProgressBarBound->setZValue(2.5);
    myscene->addItem(ProgressBarBound);

    ButtonText = new GraphicsTextItemFixBound();
    font.setPointSize(14);
    ButtonText->setFont(font);
    ButtonText->setDefaultTextColor(Qt::black);
    ButtonText->setZValue(3);
    ButtonText->setPlainText(tr("Close"));
    myscene->addItem(ButtonText);

    ButtonRectItem = new QGraphicsRectItem();
    ButtonRectItem->setPen(QPen(Qt::black));
    ButtonRectItem->setBrush(Qt::white);
    ButtonRectItem->setOpacity(0.4);
    ButtonRectItem->setZValue(2);
    myscene->addItem(ButtonRectItem);
}



View::~View()
{

}


inline QString Tostr(QRect r)
{
    return QString("(%1,%2)[%3x%4]").arg(r.x(), 5).arg(r.y(), 5).arg(r.width(), 4).arg(r.height(), 4);
}

inline QString Tostr(QSize s)
{
    return QString("             [%3x%4]").arg(s.width(), 4).arg(s.height(), 4);
}

void View::ShowRefreshment(const QList<QString> &pics_path, const QString &clock, const QString &text, const  Setting &_setting)
{
    //qDebug() << QTime::currentTime().toString("ss.zzz") << "ShowRefreshment start" ;
    setting = _setting;

    bool isLogging = setting.isLogging;
    bool isPrettyFont = setting.isPrettyFont;
    ImageAspectMode AspectMode = setting.imageAspectMode;

    QRect desktop = QApplication::desktop()->geometry();
    QRect default_screen = QApplication::desktop()->screenGeometry(-1);
    double ratio_desk = double(desktop.width()) / desktop.height();
    double ratio_default_screen = double(default_screen.width()) / default_screen.height();

    {
        // ProgressBar and ButtonRect
        int label_height = this->ProgressBarText->boundingRect().height();
        int label_y_pos = default_screen.height() - 25 - label_height;
        int label_x_pos = default_screen.left() + 2.5 * default_screen.width() / 5 - this->ProgressBarText->boundingRect().width() / 2 + 10;

        ProgressBarText->setPos(label_x_pos, label_y_pos);
        ProgressBarText->setPlainText("0:30");
        ProgressBarRect = QRect(default_screen.left() + default_screen.width() / 5, label_y_pos, 3 * default_screen.width() / 5, label_height);
        ProgressBarBackground->setRect(ProgressBarRect);
        ProgressBar->setRect(ProgressBarRect.adjusted(0, 0, -ProgressBarRect.width() / 2, 0));
        ProgressBarBound->setRect(ProgressBarRect.adjusted(-1, -1, 0, 0));

        ButtonRectItem->setRect(QRect(ProgressBarBackground->boundingRect().right() + 25, label_y_pos, 100, label_height));

        QPointF p = ButtonRectItem->rect().center() -
                    QPoint(ButtonText->boundingRect().width(), ButtonText->boundingRect().height()) / 2;
        ButtonText->setPos(p.x(), p.y());
    }

    // ClockItem and TextItem
    if (!clock.isEmpty() || !text.isEmpty()) {
        int font_size = 30;
        QFont font(isPrettyFont ? "UKIJ Diwani Yantu" : "PT Serif", font_size, QFont::Bold);
        QColor fill_color(Qt::white);
        QColor outline_color(Qt::black);
        qreal opacity = 0.5;

        if (!clock.isEmpty()) {
            clockItem = new QGraphicsSimpleTextItem();
            clockItem->setText(clock);
            clockItem->setFont(font);
            clockItem->setPos(default_screen.topRight() + QPoint(-50 - clockItem->boundingRect().width(), 25));
            clockItem->setZValue(3);
            clockItem->setBrush(fill_color);
            clockItem->setPen(outline_color);
            clockItem->setOpacity(opacity);
            myscene->addItem(clockItem);
        }

        if (!text.isEmpty()) {
            textItem = new QGraphicsSimpleTextItem();
            textItem->setText(text);
            textItem->setFont(font); //PT Serif
            textItem->setPos(default_screen.topLeft() + QPoint(50, 25));
            textItem->setZValue(3);
            textItem->setBrush(fill_color);
            textItem->setPen(outline_color);
            textItem->setOpacity(opacity);
            myscene->addItem(textItem);

//            QGraphicsRectItem *textItemRect = new QGraphicsRectItem();
//            textItemRect->setRect(textItem->boundingRect());
//            textItemRect->setPos(textItem->pos());
//            textItemRect->setZValue(2);
//            textItemRect->setPen(Qt::NoPen);
//            textItemRect->setBrush(Qt::black);
//            textItemRect->setOpacity(0.5);

//            QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
//            blur->setBlurRadius(20);
//            textItemRect->setGraphicsEffect(blur);

            //myscene->addItem(textItemRect);

        }
    }

    // Picture and Background
    QList<double> ratios_pic;
    QList<QPixmap> pics;
    QList<QString> pics_path_confirmed;

    for (auto pic_path : pics_path) {
        QPixmap pic = QPixmap(pic_path);
        if (!pic.isNull()) {
            pics.append(pic);
            ratios_pic.append((double)pic.width() / pic.height());
            pics_path_confirmed.append(pic_path);
        }
    }

    QPixmap pic;

    myscene->setSceneRect(desktop);
    myscene->setBackgroundBrush(Qt::black);
    QString ratio_case = "No image";
    double ratio_pic = 0.0;


    if (pics.size() > 0) {
        // find out which one of pic apropriate, if "inside" - comparison to def screen, if outside or auto - comparison to desk
        {
            double ratio = (AspectMode == ImageAspectMode::Inside) ? ratio_default_screen : ratio_desk;
            double min_value = std::numeric_limits<double>::max();
            double min_index = 0;

            for (int i = 0; i < pics.size(); i++) {
                if (abs(ratios_pic[i] - ratio) < min_value) {
                    min_value = abs(ratios_pic[i] - ratio);
                    min_index = i;
                }
            }

            picture_path = pics_path_confirmed[min_index];
            pic = pics[min_index];
            ratio_pic = ratios_pic[min_index];
        }

        if (AspectMode == ImageAspectMode::Inside)      ratio_case = "Default_screen Inside";
        else if (AspectMode == ImageAspectMode::Outside)     ratio_case = "Full_desktop Outside";
        else if (AspectMode == ImageAspectMode::Auto) {
            if (std::abs(ratio_desk - ratio_pic) <= std::abs(ratio_default_screen - ratio_pic)) { // Full_desktop
                if (ratio_pic / ratio_desk < 0.7 || ratio_pic / ratio_desk > 1 / 0.7)                     ratio_case = "Full_desktop Inside";
                else                                                                                      ratio_case = "Full_desktop Outside";
            }
            else {   //Default_screen
                if (ratio_pic / ratio_default_screen < 0.7 || ratio_pic / ratio_default_screen > 1 / 0.7)  ratio_case = "Default_screen Inside";
                else                                                                                       ratio_case = "Default_screen Outside";
            }
        }

        QGraphicsPixmapItem *pic_item;
        if (ratio_case == "Full_desktop Outside") {
            pic = pic.scaled(desktop.size(), Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(desktop.topLeft() + QPoint(desktop.width() - pic.width(), desktop.height() - pic.height()) / 2);
        }
        else if (ratio_case == "Default_screen Outside") {
            pic = pic.scaled(default_screen.size(), Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(default_screen.topLeft() + QPoint(default_screen.width() - pic.width(), default_screen.height() - pic.height()) / 2);

        }
        else if (ratio_case == "Full_desktop Inside") {
            pic = pic.scaled(desktop.size(), Qt::KeepAspectRatio);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(desktop.topLeft() + QPoint(desktop.width() - pic.width(), desktop.height() - pic.height()) / 2);
        }
        else {   //if (ratio_var == "Default_screen Inside")
            pic = pic.scaled(default_screen.size(), Qt::KeepAspectRatio);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(default_screen.topLeft() + QPoint(default_screen.width() - pic.width(), default_screen.height() - pic.height()) / 2);
        }

        pic_item->setZValue(-2);
        myscene->addItem(pic_item);

    }
    else {   // pics.size()==0
        Hue_start = qrand() % 360 / 360.0;

        SetBackground(Hue_start);
    }

    // Logging
    if (isLogging) {
        QString Logging_str = "";

        for (int i = 0; i < pics.size(); i++) {
            if (picture_path == pics_path_confirmed[i])
                Logging_str += QString("image showing      %1  %2 %3\n").arg(ratios_pic[i], 4, 'f', 2).arg(Tostr(pics[i].size())).arg(pics_path_confirmed[i]);
            else
                Logging_str += QString("image              %1  %2 %3\n").arg(ratios_pic[i], 4, 'f', 2).arg(Tostr(pics[i].size())).arg(pics_path_confirmed[i]);

        }

        Logging_str += QString("full desktop       %3  %4\n").arg(ratio_desk, 4, 'f', 2).arg(Tostr(desktop.size()));

        for (int i = 0; i < QApplication::desktop()->screenCount(); i++) {
            if (QApplication::desktop()->primaryScreen() == i) {
                Logging_str += QString("default screen #%1  %2  %3\n").arg(i)
                               .arg(ratio_default_screen, 4, 'f', 2)
                               .arg(Tostr(default_screen));
            }
            else {
                QRect scr_i = QApplication::desktop()->screenGeometry(i);
                Logging_str += QString("        screen #%1  %2  %3\n").arg(i)
                               .arg((double)scr_i.width() / scr_i.height(), 4, 'f', 2)
                               .arg(Tostr(scr_i));
            }
        }

        Logging_str += QString("Ratio case = %1").arg(ratio_case);

        LogToFile("LoggingDisplay.txt", Logging_str);
    }

#ifndef DEPLOY
    // SaveSceneToFile
    SaveSceneToFile("c:/Users/User/Pictures/Screenshots/EyesThanks");
#endif

    qDebug().noquote()  << QTime::currentTime().toString("ss.zzz") << "ShowRefreshment before showFullScreen";

#ifdef __linux__ // showFullScreen() doesn't work
    setWindowFlags(Qt::X11BypassWindowManagerHint);
    show();
    setGeometry(desktop);
#else
    showFullScreen();
    setGeometry(desktop);
#endif
}

void View::SaveSceneToFile(QString dir_path)
{
//    myscene->setBackgroundBrush(Qt::transparent);
    QTime timer;
    timer.start();

    QImage image(myscene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QDir dir(dir_path);
    if (!dir.exists()) dir.mkpath(".");
    QString methodName = QString(QMetaEnum::fromType<Methods>().valueToKey(Method));

    QPainter painter(&image);
    myscene->render(&painter);
    bool done = image.save(dir_path + QDir::separator() + QString("m%1-%2.png")
                           .arg(methodName)
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"))
                          );
    qDebug() << "SaveSceneToFile is" << done << "by" << timer.elapsed() << "ms.";
}

void View::UpdateValues(const QString &remains_str, const double &ratio)
{
    ProgressBarRect.setRight(qRound(ProgressBarBackground->rect().left() + ProgressBarBackground->rect().width() * (1 - ratio)));
    ProgressBar->setRect(ProgressBarRect);

    ProgressBarText->setPlainText(remains_str)  ;

    if (clockItem != nullptr)
        clockItem->setText(QTime::currentTime().toString("hh:mm"));

    if (picture_path.isEmpty() && IsBackgroundUpdate)
        SetBackground(Hue_start + ratio);
}


// HSV
// S - from uncolor to color
// V - from black to color or white
// 0 0 - black
// 0 0.5 grey
// 0 1 - white
// 1 0 - black
// 1 1 - color
void View::SetBackground(double hue_now)
{
    //qDebug() << QTime::currentTime().toString("ss.zzz") <<"SetBackground start";
    QElapsedTimer timer;
    timer.start();

    QRect desktop = QApplication::desktop()->geometry();
    QRect default_screen = QApplication::desktop()->screenGeometry(-1);

    QRect screen = desktop;

    if (Method == -1) {
        Method = rand() % COUNT_OF_METHODS;

        if (Method == UNICOLOROUS
                || Method == RANDOM_DOT
                || Method == RANDOM_DOTS

                //|| Method == OPENGLDOTS

           )
            IsBackgroundUpdate = true;
    }


    switch (Method) {
    case UNICOLOROUS: { // 0
        QColor c0 = QColor::fromHsvF(fmod(hue_now,  1), 1, 0.5);
        myscene->setBackgroundBrush(c0);
        break;
    }
    case LINEAR_GRADIENT_RAINBOW: {
//        QLinearGradient linearGrad(rand() % 2 ? screen.bottomLeft() : screen.topLeft(),
//                                   rand() % 2 ? screen.bottomRight() : screen.topRight());

        QLinearGradient rainbow(screen.topLeft(), screen.topRight());

        for (double ratio = 0, hue = 0; ratio <= 1.0; ratio += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(hue_now + hue * 0.8, 1), 1, 1);
            rainbow.setColorAt(1 - ratio, c);
            hue += 1. / 25;
        }
        myscene->setBackgroundBrush(QBrush(rainbow));

        QLinearGradient vertical(screen.topLeft(), screen.bottomLeft());
        vertical.setColorAt(0, QColor::fromRgbF(0, 0, 0, 0.9));
        vertical.setColorAt(0.5, QColor::fromRgbF(0, 0, 0, 0.1));
        vertical.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0.9));

        QGraphicsRectItem *item = new QGraphicsRectItem(screen);
        item->setPen(Qt::NoPen);
        item->setBrush(vertical);
        myscene->addItem(item);

        break;
    }
    case LINEAR_GRADIENT_STRIPES: { //
        QLinearGradient linearGrad(screen.topLeft(), screen.topRight());

        for (double ratio = 0, hue = 0; ratio <= 1.0; ratio += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(hue_now + hue * 0.8, 1), 1, 1);
            linearGrad.setColorAt(1 - ratio, c);
            hue += 1. / 25;
        }

        myscene->setBackgroundBrush(QBrush(linearGrad));

        double ratio = double(screen.width()) / screen.height();
        double standard_ratio = 16.0 / 9.0;
        double k = ratio / standard_ratio;
        int stripes_count = 10;

        int stripe_segment = screen.width() / int(stripes_count * k);
        int stripe_width = stripe_segment * 0.8;

        bool stripes_gradient = rand() % 2;

        if (stripes_gradient) {
            for (int i = -stripe_width / 2; i < screen.width(); i = i + stripe_segment) {

                QGraphicsRectItem *item = new QGraphicsRectItem(i, 0, stripe_width, screen.height());
                QLinearGradient grad(item->rect().topLeft(), item->rect().topRight());
                grad.setColorAt(0, QColor::fromRgbF(0, 0, 0, 0));
                grad.setColorAt(0.5, QColor::fromRgbF(0, 0, 0, 0.9));
                grad.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0));
                item->setPen(Qt::NoPen);
                item->setBrush(grad);
                myscene->addItem(item);
            }
        }
        else {
            stripe_width = stripe_segment * 0.4;
            for (int i = -stripe_width / 2; i < screen.width(); i = i + stripe_segment) {
                QGraphicsLineItem *item = new QGraphicsLineItem(i + stripe_width / 2, 0, i + stripe_width / 2, screen.height());
                item->setPen(QPen(QColor(Qt::black), stripe_width));
                item->setOpacity(0.7);
                myscene->addItem(item);
            }
        }

        break;
    }
    case RAINBOWED_RECTANGLES: {

        int w = qrand() % 50 + 7;
        double half = qrand() % 10 + 2;
        int h = 2 * int(half) + 1;
        qDebug() << w << " " << h;
        double cellw = screen.width() / double(w);
        double cellh = screen.height() / double(h);


        for (int i = 0; i < int(w); i++) {
            for (int j = 0; j < int(h); j++) {
                QGraphicsRectItem *textItemRect = new QGraphicsRectItem();
                textItemRect->setRect(i * cellw, j * cellh,
                                      (i == w - 1) ? cellw : cellw - 1,
                                      (j == h - 1) ? cellh : cellh - 1);

                textItemRect->setPen(Qt::NoPen);
                double hue = double(i) / w;
                double value = 1 - qAbs(j - half) / (half + 1);
                textItemRect->setBrush(QColor::fromHsvF(hue, 1.0, value));
                myscene->addItem(textItemRect);
            }
        }
        break;
    }

    case RANDOM_DOT: {
        if (ElapsedTimerDot == nullptr) {
            Item = new QGraphicsEllipseItem();
            myscene->addItem(Item);
            ElapsedTimerDot = new QElapsedTimer();
            myscene->setBackgroundBrush(Qt::black);
        }
        else if (ElapsedTimerDot->elapsed() > 500) {
            ElapsedTimerDot->start();
            int diameter_dot = qMin(screen.width(), screen.height()) / 10;
            QRect r(qrand() % (screen.width() - diameter_dot),
                    qrand() % (screen.height() - diameter_dot), diameter_dot, diameter_dot);

            Item->setRect(r);
            Item->setBrush(QColor::fromHsvF(qrand() % 256 / 255.0, 1, 1, 0.5));
        }
        break;
    }
    case RANDOM_DOTS: {
        if (ElapsedTimerDot == nullptr) {
            ElapsedTimerDot = new QElapsedTimer();
            //myscene->setCacheMode(QGraphicsItem::ItemCoordinateCache);
            //myscene->setItemIndexMethod(QGraphicsScene::NoIndex);
            //myscene->setBspTreeDepth(10);
            //setViewport(new QOpenGLWidget);
            group =  new QGraphicsItemGroup();
            myscene->addItem(group);
            myscene->setBackgroundBrush(Qt::black);
        }
        else if (ElapsedTimerDot->elapsed() > 100) {
            ElapsedTimerDot->start();

            int diameter_dot = qrand() % 100 + 50;
            QRect r(qrand() % (screen.width() - diameter_dot),
                    qrand() % (screen.height() - diameter_dot), diameter_dot, diameter_dot);

            QGraphicsEllipseItem *item = new QGraphicsEllipseItem(r);
            item->setBrush(QColor::fromHsvF(fmod(hue_now, 1), 1, 1));
            item->setPen(Qt::NoPen);
            item->setOpacity(0.5);
            item->setData(0, hue_now);
            //item->setCacheMode(QGraphicsItem::ItemCoordinateCache);
            group->addToGroup(item);

            for (auto i : group->childItems()) {
                if (i->opacity() < 0.02)
                    myscene->removeItem(i);
                i->setOpacity(i->opacity() - 0.002);
            }
        }

        break;
    }

//    case OPENGLDOTS: {
//        if (ElapsedTimerDot == nullptr) {
//            ElapsedTimerDot = new QElapsedTimer();
//            testitem = new TestItem(screen);
//            connect(this, SIGNAL(opengl_update(double, int)), testitem, SLOT(animate(double, int)));
//            QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
//            proxy->setWidget(testitem);
//            myscene->addItem(proxy);
//            myscene->setBackgroundBrush(Qt::black);

//            int diameter_dot = qrand() % 100 + 50;
//            emit opengl_update(fmod(hue_now, 1), diameter_dot);

//        }
//        else if (ElapsedTimerDot->elapsed() > 100) {
//            ElapsedTimerDot->start();

//            int diameter_dot = qrand() % 100 + 50;
//            //testitem->animate(fmod(hue_now,1), diameter_dot);
//            emit opengl_update(fmod(hue_now, 1), diameter_dot);
//        }
//        break;
//    }

    case NEO: {
        QString name = qgetenv("USER");
        if (name.isEmpty()) name = qgetenv("USERNAME");
        if (name.isEmpty() || name == "User") name = "Neo";

        QGraphicsSimpleTextItem *centerText = new QGraphicsSimpleTextItem();
        myscene->addItem(centerText);
        centerText->setText(QString("Wake up, %1...").arg(name));
        centerText->setBrush(QColor(0, 32, 0));
        centerText->setPen(QColor(0, 255, 0));
        centerText->setFont(QFont("PT Serif", 100));
        centerText->setZValue(3);

        if (centerText->boundingRect().width() < default_screen.width())
            centerText->setPos(default_screen.center() - QPoint(centerText->boundingRect().width(), centerText->boundingRect().height()) / 2);
        else {
            centerText->setText(QString("Wake up,\n%1...").arg(name));
            centerText->setPos(default_screen.center() - QPoint(centerText->boundingRect().width(), centerText->boundingRect().height()) / 2);
        }

        if (clockItem) {
            clockItem->setBrush(QColor(0, 152, 0));
            clockItem->setPen(Qt::NoPen);
            clockItem->setFont(QFont("PT Serif", clockItem->font().pointSize()));
            clockItem->setOpacity(1);
        }
        if (textItem)
            myscene->removeItem(textItem);


        QFont font_background("PT Serif", 14);

        QGraphicsSimpleTextItem *test = new QGraphicsSimpleTextItem("A");
        test->setFont(font_background);
        int basic_width = test->boundingRect().width();
        int basic_height = test->boundingRect().height();
        QFontMetrics fm(font_background);
        int line_height =  fm.boundingRect("a").height();

        const QVector<QVector<QVector<qint64>>> unicode_limits = {
            {{0x0250, 0x02AF}},         // 0 IPA                       = 5F
            {{0x16A0, 0x16EF}},         // 1 Runic                     = 4F
            {{0x20A0, 0x20BE}},         // 2  Currency Symbols  = 1E
            // {{0x0410, 0x044f}},          // 2 Cyrillic Russian          = 3F
            {{0x0400, 0x045f}},         // 3 Cyrillic Russian+         = 5F
            {
                {0x0400, 0x0482},       // 4 Cyrillic (1)            = 82
                {0x048A, 0x04ff},       //   Cyrillic (2)            = 75
                {0x0500, 0x052F},       //   Cyrillic Supplement     = 2F
                {0xA640, 0xA672},       //   Cyrillic Extended-B (1) = 32
                {0xA680, 0xA69B}        //   Cyrillic Extended-B (2) = 1B
            },
            {
                {0x2C00, 0x2C2E},       // 5 Глаголица  uppercase       = 2F
                {0x2C30, 0x2C5E}        //   Глаголица  lowercase       = 2F
            },
            {
                {0x0041, 0x005A},       // 6 A-Z            = 1A
                {0x0061, 0x007A},       //   a-z            = 1A
            },
            {
                {0x0041, 0x005A},       // 7 A-Z            = 1A
                {0x0061, 0x007A},       //   a-z            = 1A
                // {0x00C0, 0x00D6},       //   Latin-1 Supplement (1)
                // {0x00D8, 0x00DD},       //   Latin-1 Supplement (2)
                {0x00DE, 0x00F6},       //   Latin-1 Supplement (3)
                // {0x00F8, 0x00FF},       //   Latin-1 Supplement (4)
                // {0x0100, 0x017F},       //   Latin Extended-A
                // {0x0180, 0x024F},       //   Latin Extended-B
            },

            {
                // 8 Greek
                {0x391, 0x3A1},         //  Greek uppercase
                {0x393, 0x3A9},         //  Greek uppercase
                {0x3B1, 0x3C9},         //  Greek lowercase
                {0x3D8, 0x3E1},         //  Greek archaic koppa, stigma, digamma, koppa, sampi.
            },

            {
                // 9  Miscellaneous Symbols
                {0x262D, 0x262E},       // hammer and sickle, peace
                {0x263D, 0x2653},       // planets and zodiak
                {0x26B2, 0x26B8},       // astrology
                {0x26E6, 0x26E7},       // pentagrams
                //{0x2669,0x266F},        // music
            },

            {{0x0030, 0x0039}},         // 10 0-9            = A

            {
                {0x2160, 0x2169},       // 11  Roman numerals 1-10
                {0x216C, 0x216F},       //     Roman numerals L C D M
                //{0x2180,0x2182},
                //{0x2185,0x2186}
            },
        };

        int unicode_set = rand() % (unicode_limits.size());

        int limit = 0;
        QVector<int> limits;
        limits.reserve(unicode_limits[unicode_set].size() + 1);
        limits.append(limit);
        for (int i = 0; i < unicode_limits[unicode_set].size(); i++) {
            limit += unicode_limits[unicode_set][i][1] - unicode_limits[unicode_set][i][0] + 1;
            limits.append(limit);
        }

        {
            // every item, FullHD 63 ms.
            for (int column_pos_x = 0; column_pos_x < screen.width();) {
                int char_max = (screen.height() - basic_height) / line_height + 2;
                int char_count = 1.0 / 4.0 * char_max + 3.0 / 4.0 * (qrand() % char_max);
                for (int char_index = 0; char_index < char_count; char_index++) {
                    QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
                    myscene->addItem(item);
                    item->setBrush(QColor(0, 128, 0));
                    item->setPen(Qt::NoPen);
                    item->setFont(font_background);

                    int unicode = rand() % limits.last();

                    for (int i = 1; i < limits.size(); i++)
                        if (limits[i - 1] <= unicode && unicode < limits[i])
                            unicode = unicode - limits[i - 1] + unicode_limits[unicode_set][i - 1][0];

                    item->setText(QChar(unicode));
                    item->setPos(column_pos_x - item->boundingRect().width() / 2, char_index * line_height);
                }
                column_pos_x += basic_width * 3;
            }
        }

//        // for test cyrillic
//        static int unicode = 0;
//        int u;
//        if (unicode++ < borders.last()){
//            for (int i=1; i<borders.size();i++)
//                if (borders[i-1]<=unicode && unicode< borders[i]) u = unicode - borders[i-1] + limits[i-1+4][0];

//            item->setText(QChar(u));
//        }
//        else
//            item->setText(QChar(0x20));

//        {  // columnItem, FullHD 55ms.
//            for (int column_pos_x = 0; column_pos_x < screen.width();) {
//                QGraphicsSimpleTextItem *columnItem = new  QGraphicsSimpleTextItem();
//                myscene->addItem(columnItem);
//                columnItem->setBrush(QColor(0, 128, 0));
//                columnItem->setPen(Qt::NoPen);
//                //columnItem->setDefaultTextColor(QColor(0, 128, 0));
//                columnItem->setFont(font_background);
//                columnItem->setPos(column_pos_x, 0);

//                QString column_text;
//                int char_max = (screen.height() - basic_height) / line_height + 2;
//                char_max /= 2;
//                int char_count = 1.0 / 4.0 * char_max + 3.0 / 4.0 * (qrand() % char_max);

//                for (int char_index = 0; char_index < char_count; char_index++) {
//                    column_text += QString("%1\n%2\n")
//                                   .arg(QChar(first + qrand() % (last - first + 1)))
//                                   .arg(QChar(first + qrand() % (last - first + 1)));
//                }
//                columnItem->setText(column_text);
//                column_pos_x += columnItem->boundingRect().width() * 2;
//            }
//        }




//        {   // textItem    18ms
//            int line_width =  fm.boundingRect("a\t\t").width();
//            int row_count = (screen.height() - basic_height)/line_height+2;
//            int column_count = screen.width()/line_width;
//            QString text;
//            for(int i = 0; i < row_count; i++) {
//                for(int j = 0; j < column_count; j++){
//                    QChar ch (first + qrand() % (last-first+7) );
//                    text += QString("%1\t%2\t").arg(ch).arg(QChar(ch.unicode()+1));
//                }
//                text += "\n";
//            }
//            QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem();
//            myscene->addItem(textItem);
//            textItem->setBrush(QColor(0, 255, 0, 128));
//            textItem->setFont(font_background);
//            textItem->setText(text);
//        }

        myscene->setBackgroundBrush(Qt::black);
        break;
    }
//    case LINEAR_GRADIENT_TEST: {
        //QList<QLinearGradient> grads;
        //grads.append(QLinearGradient (screen.topLeft(), screen.topRight()));
        //grads.append(QLinearGradient (screen.topLeft(), screen.bottomLeft()));
//        grads.append(QLinearGradient (screen.topLeft(), screen.bottomRight()));

//        QList<QGraphicsEllipseItem *> items;

//        for (int i=0; i<10;i++)
//        {
//            items.append(new QGraphicsEllipseItem(rand()%screen.width(),rand()%screen.height(),300,300));
//            items[i]->setPen(Qt::NoPen);
//            myscene->addItem(items[i]);
//        }

//        QList<QRadialGradient> rgs;
//        for (auto &item:items)
//        {
//            rgs.append(QRadialGradient (item->rect().center(),item->rect().width()/2));
//        }




//      for (auto rg:rgs)
//       for (int i=0, k=2; i<k; i++)
//            rg.setColorAt((i)/double(k-1), QColor::fromRgbF(0,0,0,(i+1)%2*0.2));

//       for (int i=0; i<items.size();i++)
//        {
//            items[i]->setBrush(rgs.at(i));

//        }

//        myscene->setBackgroundBrush(QColor::fromHsvF(0, 1, 0.5));

//        for (int j = 0; j < 2; j++) {
//            QPoint p(rand() % screen.width(), rand() % screen.height());
//            QSize s(qMin(screen.width(), screen.height()), qMin(screen.width(), screen.height()));

//            QRect r;
//            r.moveCenter(p);
//            r.setSize(s);

//            QGraphicsEllipseItem *item = new QGraphicsEllipseItem(r);

//            item->setPen(Qt::NoPen);
//            myscene->addItem(item);

//            QRadialGradient rg = QRadialGradient(item->rect().center(), item->rect().width() / 2);
//            for (int i = 0, k = 2; i < k; i++)
//                rg.setColorAt((i) / double(k - 1), QColor::fromRgbF(rand() % 2, 0, 0, (i + 1) % 2 * 0.5));
//            item->setBrush(rg);
//        }


//        for (int i=0, k=21; i<k; i++)
//            grads[0].setColorAt((i+(rand()%100)/100.-0.5)/double(k-1), QColor::fromRgbF(0,0,0,(i+1)%2*0.5));

//        for (int i=0, k=11; i<k; i++)
//            grads[1].setColorAt((i+(rand()%100)/100.-0.5)/double(k-1), QColor::fromRgbF(0,0,0,(i+1)%2*0.5));


//        QList<QGraphicsRectItem *> items;
//        for (int i=0; i<2;i++)
//        {
//            items.append(new QGraphicsRectItem(screen));
//            items.last()->setPen(Qt::NoPen);
//            items.last()->setBrush(grads.at(i));
//            myscene->addItem(items.last());
//        }


//        break;
//    }
    }


    if (setting.isLogging) {
        QMetaEnum metaEnum = QMetaEnum::fromType<Methods>();

        QString timer_string = QString("Method %1: %2 ms, items %3")
                               .arg(metaEnum.valueToKey(Method)).arg(timer.elapsed()).arg(myscene->items().size());
        qDebug().noquote() << timer_string;
        LogToFile("LoggingSetBackground.txt", timer_string);
    }
    else {
        QString timer_string = QString("Method %1: %2 ms, items %3")
                               .arg(Method).arg(timer.elapsed()).arg(myscene->items().size());
        qDebug().noquote() << timer_string;
    }
}


void View::keyPressEvent(QKeyEvent *event)
{
    event->accept();
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        QFile pic(picture_path);

        if (pic.exists() && pic.remove()) {
            for (auto item : myscene->items())
                if (item->zValue() == -2) {
                    myscene->removeItem(item);
                    delete item;
                }
            SetBackground(qrand() % 360 / 360.);
        }
    }
    else if (event->key() == Qt::Key_Escape)
        close();

    // QGraphicsView::keyPressEvent(event);

}

void View::mousePressEvent(QMouseEvent *event)
{
    for (auto i : items(event->pos())) {
        if (ButtonRectItem == i) {
            //ButtonRectItem->setBrush(Qt::green);
            ButtonRectItem->setOpacity(1);
        }
    }
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    for (auto i : items(event->pos()))
        if (ButtonRectItem == i)
            close();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    bool IsButton = false;
    for (auto i : items(event->pos()))
        if (ButtonRectItem == i)
            IsButton = true;

    if (IsButton) {
        if (event->buttons() == Qt::NoButton)
            ButtonRectItem->setOpacity(0.8);
        else
            ButtonRectItem->setOpacity(1);
    }
    else
        ButtonRectItem->setOpacity(0.4);

}

void View::closeEvent(QCloseEvent *event)
{
    //qDebug().noquote()  << QTime::currentTime().toString("ss.zzz") << "View::closeEvent";
    //for (auto item : myscene->items())
    //   if (item->zValue() == -1 || item->zValue() == -2) {
    //       myscene->removeItem(item);
    //       delete item;
    //   }

    myscene->clear();
    myscene->deleteLater();


    emit view_close();
    //close();

    //hide();
    //event->ignore();

    //QGraphicsView::close();
    QGraphicsView::closeEvent(event);
}
