//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//                                                                                  //
//      This file is part of Eyes' Thanks.                                          //
//                                                                                  //
//      Eyes' Thanks is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License either                 //
//      version 3 of the License, or (at your option) any later version.            //
//                                                                                  //
//      Eyes' Thanks is distributed in the hope that it will be useful,             //
//      but WITHOUT ANY WARRANTY; without even the implied warranty of              //
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               //
//      GNU General Public License for more details.                                //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with Eyes' Thanks.  If not, see <http://www.gnu.org/licenses/>.       //
//----------------------------------------------------------------------------------//

#include "view.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>

View::View(QWidget *parent): QGraphicsView(parent), clockItem(NULL), Item(NULL), ElapsedTimerDot(NULL), Method(-1), IsBackgroundUpdate(false), RunnedFirstTime(false)
{
    myscene = new QGraphicsScene();
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QGraphicsView { border-style: none; }");

    setScene(myscene);


    ProgressBarText = new GraphicsTextItemFixBound();
    QFont font;
    font.setPointSize(16);
    ProgressBarText->setFont(font);
    ProgressBarText->setDefaultTextColor(Qt::black);
    ProgressBarText->setZValue(3);
    myscene->addItem(ProgressBarText);

    ProgressBar = new QGraphicsRectItem();
    ProgressBar->setPen(Qt::NoPen);
    ProgressBar->setBrush(Qt::white);
    ProgressBar->setOpacity(0.8);
    ProgressBar->setZValue(2);
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

void View::ShowRefreshment(QList<QString> pics_path, QString clock, QString text, Setting _setting)
{
    //qDebug() << QTime::currentTime().toString("ss.zzz") << "ShowRefreshment start" ;
    setting = _setting;

    bool isLogging = setting.isLogging;
    bool isPrettyFont = setting.isPrettyFont;
    ImageAspectMode AspectMode = setting.imageAspectMode;

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
    QRect desktop = QApplication::desktop()->geometry();
    QRect default_screen = QApplication::desktop()->screenGeometry(-1);
    double ratio_desk = (double)desktop.width() / desktop.height();
    double ratio_default_screen = (double)default_screen.width() / default_screen.height();

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

    this->ProgressBarText->setPlainText("0:00");

    int label_height = this->ProgressBarText->boundingRect().height();
    int label_y_pos = default_screen.height() - 25 - label_height;
    int label_x_pos = default_screen.left() + 2.5 * default_screen.width() / 5 - this->ProgressBarText->boundingRect().width() / 2 + 10;

    this->ProgressBarText->setPos(label_x_pos, label_y_pos);

    ProgressBarRect = QRect(default_screen.left() + default_screen.width() / 5, label_y_pos, 3 * default_screen.width() / 5, label_height);

    ProgressBarBackground->setRect(ProgressBarRect);
    ProgressBar->setRect(ProgressBarRect.adjusted(0, 0, -ProgressBarRect.width() / 2, 0));

    ProgressBarBound->setRect(ProgressBarRect.adjusted(-1, -1, 0, 0));


    ButtonRectItem->setRect(QRect(ProgressBarBackground->boundingRect().right() + 25, label_y_pos, 100, label_height));

    QPointF p = ButtonRectItem->rect().center() -
                QPoint(ButtonText->boundingRect().width(), ButtonText->boundingRect().height()) / 2;
    ButtonText->setPos(p.x(), p.y());

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

    if (!clock.isEmpty()) {

        clockItem = new QGraphicsTextItem();
        clockItem->setPlainText(clock);
        clockItem->setZValue(3);
        clockItem->setDefaultTextColor(QColor(Qt::white));
        clockItem->setFont(QFont(isPrettyFont ? "UKIJ Diwani Yantu" : "PT Serif", 30, 30));
        clockItem->setPos(default_screen.topRight() + QPoint(-50 - clockItem->boundingRect().width(), 25));
        clockItem->setOpacity(0.5);

        //QGraphicsDropShadowEffect * shadow1 = new QGraphicsDropShadowEffect();
        //shadow1->setColor(QColor(0,0,0));
        //shadow1->setOffset( 0, 0 );
        //shadow1->setBlurRadius( 20 );
        //clockItem->setGraphicsEffect(shadow1);


        QGraphicsRectItem *clockItemRect = new QGraphicsRectItem();
        clockItemRect->setRect(clockItem->boundingRect());
        clockItemRect->setPos(clockItem->pos());
        clockItemRect->setZValue(2);
        clockItemRect->setPen(Qt::NoPen);
        clockItemRect->setBrush(Qt::black);
        clockItemRect->setOpacity(0.5);

        QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
        blur->setBlurRadius(20);
        clockItemRect->setGraphicsEffect(blur);

        myscene->addItem(clockItemRect);
        myscene->addItem(clockItem);
    }

    if (!text.isEmpty()) {
        QGraphicsTextItem *textItem = new QGraphicsTextItem();
        textItem->setPlainText(text);
        textItem->setPos(default_screen.topLeft() + QPoint(50, 25));
        textItem->setZValue(3);
        textItem->setDefaultTextColor(QColor(255, 255, 255, 255));
        textItem->setFont(QFont(isPrettyFont ? "UKIJ Diwani Yantu" : "PT Serif", 30, 30));

        //PT Serif
        //QGraphicsDropShadowEffect * shadow = new QGraphicsDropShadowEffect();
        //shadow->setColor(QColor(0,0,0));
        //shadow->setOffset( 0, 0 );
        //shadow->setBlurRadius( 20 );
        //textItem->setGraphicsEffect(shadow);

        textItem->setOpacity(0.5);

        QGraphicsRectItem *textItemRect = new QGraphicsRectItem();
        textItemRect->setRect(textItem->boundingRect());
        textItemRect->setPos(textItem->pos());
        textItemRect->setZValue(2);
        textItemRect->setPen(Qt::NoPen);
        textItemRect->setBrush(Qt::black);
        textItemRect->setOpacity(0.5);

        QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
        blur->setBlurRadius(20);
        textItemRect->setGraphicsEffect(blur);

        myscene->addItem(textItemRect);
        myscene->addItem(textItem);
    }

    // SaveSceneToFile
    SaveSceneToFile("c:/Users/User/Pictures/Screenshots/EyesThanks");

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

    QPainter painter(&image);
    myscene->render(&painter);
    bool done = image.save(dir_path + QDir::separator() + QString("M%1-%2.png")
                           .arg(Method)
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss"))
                          );
    qDebug() << "SaveSceneToFile is" << done << "by" << timer.elapsed() << "ms.";
}

void View::UpdateValues(QString remains_str, double ratio)
{
    this->ProgressBarRect.setRight(qRound(ProgressBarBackground->rect().left() + ProgressBarBackground->rect().width() * (1 - ratio)));
    ProgressBar->setRect(ProgressBarRect);


    this->ProgressBarText->setPlainText(remains_str)  ;

    if (clockItem != NULL)
        clockItem->setPlainText(QTime::currentTime().toString("hh:mm"));

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
        Method = RANDOM_DOTS_2 % COUNT_OF_METHODS;

        if (Method == UNICOLOROUS
         || Method == RANDOM_DOT
         || Method == RANDOM_DOTS
         //|| Method == RANDOM_DOTS_RANDOM_COLOR

           )
            IsBackgroundUpdate = true;
    }


    switch (Method) {
    case UNICOLOROUS: { // 0
        QColor c0 = QColor::fromHsvF(fmod(hue_now,  1), 1, 0.5);
        myscene->setBackgroundBrush(c0);
        break;
    }
//    case LINEAR_GRADIENT_DIAGONAL: {  // <2
//        QLinearGradient linearGrad(screen.topLeft(), screen.topRight());

//        QColor c1 = QColor::fromHsvF(fmod(hue_now*10,       1), 1, 0.3);
//        QColor c2 = QColor::fromHsvF(fmod(hue_now*10 + 0.1, 1), 1, 0.5);
//        QColor c3 = QColor::fromHsvF(fmod(hue_now*10 + 0.2, 1), 1, 0.3);
//        linearGrad.setColorAt(0.15, c1);
//        linearGrad.setColorAt(0.5, c2);
//        linearGrad.setColorAt(0.85, c3);
//        myscene->setBackgroundBrush(QBrush(linearGrad));
//        break;
//    }
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
    case LINEAR_GRADIENT_RAINBOW: {
        QLinearGradient linearGrad(rand() % 2 ? screen.bottomLeft() : screen.topLeft(),
                                   rand() % 2 ? screen.bottomRight() : screen.topRight());

        for (double ratio = 0, hue = 0; ratio <= 1.0; ratio += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(hue_now + hue * 0.8, 1), 1, 1);
            linearGrad.setColorAt(1 - ratio, c);
            hue += 1. / 25;
        }

        for (int i = 0; i < screen.width(); i = i + 100) {
            QGraphicsLineItem *item = new QGraphicsLineItem(i, 0, i, screen.height());
            item->setPen(QPen(QColor(Qt::black), 80));
            myscene->addItem(item);
        }

        myscene->setBackgroundBrush(QBrush(linearGrad));
        break;
    }
    case LINEAR_GRADIENT_RAINBOW_2: { // <6
        QLinearGradient linearGrad(screen.topLeft(), screen.topRight());

        for (double ratio = 0, hue = 0; ratio <= 1.0; ratio += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(hue_now + hue * 0.8, 1), 1, 1);
            linearGrad.setColorAt(1 - ratio, c);
            hue += 1. / 25;
        }


        QLinearGradient grad(screen.topLeft(), screen.bottomLeft());
        grad.setColorAt(0, QColor::fromRgbF(0, 0, 0, 1));
        grad.setColorAt(0.5, QColor::fromRgbF(0, 0, 0, 0.1));
        grad.setColorAt(1, QColor::fromRgbF(0, 0, 0, 1));

        QGraphicsRectItem *item = new QGraphicsRectItem(screen);
        item->setPen(Qt::NoPen);
        item->setBrush(grad);
        myscene->addItem(item);




        myscene->setBackgroundBrush(QBrush(linearGrad));
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


//        for (int i=0; i<screen.width(); i=i+100) {
//            QGraphicsLineItem *item = new QGraphicsLineItem(i,0,i,screen.height());
//            item->setPen(QPen(QColor(Qt::black),80));
//            item->setOpacity(0.8);
//            myscene->addItem(item);
//        }

        double ratio = double(screen.width())/screen.height();
        double standard_ratio = 16.0/9.0;
        double k = ratio/standard_ratio;

        int stripe_segment = screen.width()/int(7*k);
        int stripe_width = stripe_segment*0.8;

        for (int i = -stripe_width/2; i < screen.width(); i = i + stripe_segment) {

            QGraphicsRectItem *item = new QGraphicsRectItem(i, 0, stripe_width, screen.height());
            QLinearGradient grad(item->rect().topLeft(), item->rect().topRight());
            grad.setColorAt(0, QColor::fromRgbF(0, 0, 0, 0));
            grad.setColorAt(0.5, QColor::fromRgbF(0, 0, 0, 0.9));
            grad.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0));
            item->setPen(Qt::NoPen);
            item->setBrush(grad);
            myscene->addItem(item);
            myscene->addItem(item);
        }
        break;
    }
    case LINEAR_GRADIENT_STRIPES_2: { //
        QLinearGradient linearGrad(screen.topLeft(), screen.topRight());

        for (double ratio = 0, hue = 0; ratio <= 1.0; ratio += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(hue_now + hue * 0.8, 1), 1, 1);
            linearGrad.setColorAt(1 - ratio, c);
            hue += 1. / 25;
        }

        myscene->setBackgroundBrush(QBrush(linearGrad));


        double ratio = double(screen.width())/screen.height();
        double standard_ratio = 16.0/9.0;
        double k = ratio/standard_ratio;

        int stripe_segment = screen.width()/int(7*k);
        int stripe_width = stripe_segment*0.4;

        for (int i=0; i<screen.width(); i = i + stripe_segment) {
            QGraphicsLineItem *item = new QGraphicsLineItem(i,0,i,screen.height());
            item->setPen(QPen(QColor(Qt::black),stripe_width));
            item->setOpacity(0.7);
            myscene->addItem(item);
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
        if (Item == NULL) {
            Item = new QGraphicsEllipseItem();
            myscene->addItem(Item);
            ElapsedTimerDot = new QElapsedTimer();
        }

        if (!ElapsedTimerDot->isValid() || ElapsedTimerDot->elapsed() > 1000) {
            int diameter_dot = 50;
            ElapsedTimerDot->start();
            QRect r(qrand() % (screen.width() - diameter_dot),
                    qrand() % (screen.height() - diameter_dot), diameter_dot, diameter_dot);

            Item->setRect(r);
            Item->setBrush(QColor::fromHsvF(qrand() % 256 / 256.0, 1, 1, 0.5));
        }

        myscene->setBackgroundBrush(Qt::black);
        break;
    }
    case RANDOM_DOTS: {
        if (ElapsedTimerDot == NULL)
            ElapsedTimerDot = new QElapsedTimer();
        else if (ElapsedTimerDot->elapsed() > 100) {
            ElapsedTimerDot->start();

            int diameter_dot = qrand() % 100 + 50;
            QRect r(qrand() % (screen.width() - diameter_dot),
                    qrand() % (screen.height() - diameter_dot), diameter_dot, diameter_dot);

            QGraphicsEllipseItem *item = new QGraphicsEllipseItem(r);
            item->setBrush(QColor::fromHsvF(fmod(hue_now, 1), 1, 1));
            item->setPen(Qt::NoPen);
            item->setOpacity(0.5);
            myscene->addItem(item);
            qDebug() << myscene->items().size();
        }
        myscene->setBackgroundBrush(Qt::black);
        break;
    }
    case RANDOM_DOTS_2: { // 28-32
        //if (ElapsedTimerDot == NULL)
        //    ElapsedTimerDot = new QElapsedTimer();
        //else if (ElapsedTimerDot->elapsed() > 2000) {
         //   ElapsedTimerDot->start();

            for (int i=0;i<1000;i++) {

            int diameter_dot = 75;
            QRect r(qrand() % (screen.width() - diameter_dot),
                    qrand() % (screen.height() - diameter_dot), diameter_dot, diameter_dot);

            QGraphicsEllipseItem *item = new QGraphicsEllipseItem(r);
            item->setBrush(QColor::fromHsvF(fmod(hue_now, 1), 1, 1));
            item->setPen(Qt::NoPen);
            item->setOpacity(0.5);
            myscene->addItem(item);
            }
        //}

        myscene->setBackgroundBrush(Qt::black);
        break;
    }
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


        QFont font_background("PT Serif", 14);

        QGraphicsSimpleTextItem *test = new QGraphicsSimpleTextItem("a");
        test->setFont(font_background);
        int basic_height = test->boundingRect().height();
        QFontMetrics fm(font_background);
        int line_height =  fm.boundingRect("a").height();

        for (int column_pos_x = 0; column_pos_x < screen.width();) {
            QGraphicsSimpleTextItem *columnItem = new  QGraphicsSimpleTextItem();
            myscene->addItem(columnItem);
            columnItem->setBrush(QColor(0, 128, 0));
            columnItem->setPen(Qt::NoPen);
            columnItem->setFont(font_background);
            columnItem->setPos(column_pos_x, 0);

            QString column_text;
            int char_max = (screen.height() - basic_height) / line_height + 2;
            char_max /= 2;
            int char_count = 1.0 / 4.0 * char_max + 3.0 / 4.0 * (qrand() % char_max);

            for (int char_index = 0; char_index < char_count; char_index++) {
                // Cyrillic 1024-1279, Extended-B 42624-42651   42560-42606
                // Runic 5792-5880
                // IPA 592-687
                int first = 592;
                int last  = 687;
                column_text += QString("%1\n%2\n")
                               .arg(QChar(first + qrand() % (last - first + 1)))
                               .arg(QChar(first + qrand() % (last - first + 1)));
            }
            columnItem->setText(column_text);
            column_pos_x += columnItem->boundingRect().width() * 2;
        }


//        int line_width =  fm.boundingRect("a\t\t").width();
//        int row_count = (screen.height() - basic_height)/line_height+2;
//        int column_count = screen.width()/line_width;
//        qDebug() << row_count << " " << column_count;
//        QString text;
//        for(int i = 0; i < row_count; i++) {
//            for(int j = 0; j < column_count; j++){
//                int first = 592;
//                int last  = 687;
//                QChar ch (first + qrand() % (last-first+7) );
//                text += QString("%1\t%2\t")
//                        .arg(ch)
//                        .arg(QChar(ch.unicode()+1));

//            }
//            text += "\n";
//        }
//        QGraphicsTextItem *textItem1 = new QGraphicsTextItem();
//        myscene->addItem(textItem1);
//        textItem1->setDefaultTextColor(QColor(0, 255, 0, 128));
//        textItem1->setFont(font_background);
//        textItem1->setPlainText(text);


        myscene->setBackgroundBrush(Qt::black);
        break;
    }
    }


    if (setting.isLogging) {
        QMetaEnum metaEnum = QMetaEnum::fromType<Methods>();

        QString timer_string = QString("Method %1: %2 ms.").arg(metaEnum.valueToKey(Method)).arg(timer.elapsed());
        qDebug().noquote() << timer_string;
        LogToFile("LoggingSetBackground.txt", timer_string);
    }
    else {
        QString timer_string = QString("Method %1: %2 ms.").arg(Method).arg(timer.elapsed());
        qDebug().noquote() << timer_string;
    }
    // qDebug() << QTime::currentTime().toString("ss.zzz") <<"SetBackground end";
    qDebug() << "items" << myscene->items().size();

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
    emit view_close();
    //for (auto item : myscene->items())
    //   if (item->zValue() == -1 || item->zValue() == -2) {
    //       myscene->removeItem(item);
    //       delete item;
    //   }

    //hide();
    //event->ignore();
    //close();

    //QGraphicsView::close();
    QGraphicsView::closeEvent(event);
}
