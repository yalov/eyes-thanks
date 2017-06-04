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

View::View(QWidget *parent): QGraphicsView(parent), clockItem(NULL)
{
    myscene = new QGraphicsScene();
    setWindowFlags(Qt::WindowStaysOnTopHint);
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
    ProgressBar->setOpacity(0.9);
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
    ButtonRectItem->setOpacity(0.9);
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

void View::ShowRefreshment(QList<QString> pics_path, QString clock, QString text, bool isLogging, bool isPrettyFont, ImageAspectMode AspectMode)
{
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
                if (ratio_pic / ratio_desk < 0.7 || ratio_pic / ratio_desk > 1 / 0.7)                     {ratio_case = "Full_desktop Inside";  }
                else                                                                                      {ratio_case = "Full_desktop Outside"; }
            } else { //Default_screen
                if (ratio_pic / ratio_default_screen < 0.7 || ratio_pic / ratio_default_screen > 1 / 0.7)  ratio_case = "Default_screen Inside";
                else                                                                                       ratio_case = "Default_screen Outside";
            }
        }

        QGraphicsPixmapItem *pic_item;
        if (ratio_case == "Full_desktop Outside") {
            pic = pic.scaled(desktop.size(), Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(desktop.topLeft() + QPoint(desktop.width() - pic.width(), desktop.height() - pic.height()) / 2);
        } else if (ratio_case == "Default_screen Outside") {
            pic = pic.scaled(default_screen.size(), Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(default_screen.topLeft() + QPoint(default_screen.width() - pic.width(), default_screen.height() - pic.height()) / 2);

        } else if (ratio_case == "Full_desktop Inside") {
            pic = pic.scaled(desktop.size(), Qt::KeepAspectRatio);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(desktop.topLeft() + QPoint(desktop.width() - pic.width(), desktop.height() - pic.height()) / 2);
        } else { //if (ratio_var == "Default_screen Inside")
            pic = pic.scaled(default_screen.size(), Qt::KeepAspectRatio);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(default_screen.topLeft() + QPoint(default_screen.width() - pic.width(), default_screen.height() - pic.height()) / 2);
        }

        pic_item->setZValue(-2);
        myscene->addItem(pic_item);

    } else { // pics.size()==0
        Hue = qrand() % 360 / 360.0;
        setGradient(Hue);
    }

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
            } else {
                QRect scr_i = QApplication::desktop()->screenGeometry(i);
                Logging_str += QString("        screen #%1  %2  %3\n").arg(i)
                               .arg((double)scr_i.width() / scr_i.height(), 4, 'f', 2)
                               .arg(Tostr(scr_i));
            }
        }

        Logging_str += QString("Ratio case = %1").arg(ratio_case);

        QFile file("LoggingDisplay.txt");
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            out << Logging_str;
            file.close();
        }

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



#ifdef __linux__ // showFullScreen() doesn't work
    setWindowFlags(Qt::X11BypassWindowManagerHint);
    show();
    setGeometry(desktop);
#else
    showFullScreen();
    setGeometry(desktop);
#endif
}



void View::UpdateValues(QString remains_str, double ratio)
{
    this->ProgressBarRect.setRight(qRound(ProgressBarBackground->rect().left() + ProgressBarBackground->rect().width() * (1 - ratio)));
    ProgressBar->setRect(ProgressBarRect);


    this->ProgressBarText->setPlainText(remains_str)  ;

    if (clockItem != NULL)
        clockItem->setPlainText(QTime::currentTime().toString("hh:mm"));

    if (picture_path.isEmpty()) {
        setGradient(Hue + ratio);
    }
}



void View::setGradient(double hue_first)
{
    //QRect desktop = QApplication::desktop()->geometry();
    QRect default_screen = QApplication::desktop()->screenGeometry(-1);
    //QLinearGradient linearGrad(default_screen.topLeft(), default_screen.bottomRight());
    //QLinearGradient linearGrad(default_screen.topLeft(), default_screen.bottomLeft());
    QLinearGradient linearGrad(default_screen.topLeft(), default_screen.topRight());

//    QColor c1;
//    c1.setHsvF(fmod(hue_first,  1), 1, 0.5);
//    QColor c2;
//    c2.setHsvF(fmod(hue_first+0.1 , 1), 1, 0.5);
//    QColor c3;
//    c3.setHsvF(fmod(hue_first+0.2 , 1), 1, 0.5);

   // linearGrad.setColorAt(0.4, c1);
   // linearGrad.setColorAt(0.5, c2);
   // linearGrad.setColorAt(0.6, c3);

//    double j=0;
//    for (double i=0; i <= 1 ;i += 32.0/default_screen.width())
//    {
//        QColor c;
//        c.setHsvF(fmod(hue_first+j , 1), 1, 0.5);
//        linearGrad.setColorAt(i, c);
//        j+=0.02;
//    }


    //myscene->setBackgroundBrush(QBrush(linearGrad));
    QColor c0;
    c0.setHsvF(fmod(hue_first,  1), 1, 0.5);
    myscene->setBackgroundBrush(c0);

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
            setGradient(rand() % 360 / 360.);
        }
    } else if (event->key() == Qt::Key_Escape) {
        close();
    }

    // QGraphicsView::keyPressEvent(event);

}

void View::mousePressEvent(QMouseEvent *event)
{
    for (auto i : items(event->pos())) {
        if (ButtonRectItem == i) {
            //ButtonRectItem->setBrush(Qt::red);
            ButtonRectItem->setOpacity(1);
        }

    }

}
void View::mouseReleaseEvent(QMouseEvent *event)
{
    for (auto i : items(event->pos()))
        if (ButtonRectItem == i)
            close();

    ButtonRectItem->setOpacity(0.9);

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
    close();

    //QGraphicsView::close();
    QGraphicsView::closeEvent(event);
}
