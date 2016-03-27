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

View::View(QWidget *parent): QGraphicsView(parent), clockItem(NULL)
{
    myscene = new QGraphicsScene();
    setWindowFlags( Qt::WindowStaysOnTopHint);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet( "QGraphicsView { border-style: none; }" );
    setScene(myscene);


    ProgressBarText = new GraphicsTextItemFixBound();
    QFont font; font.setPointSize(16);
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

void View::mousePressEvent(QMouseEvent * event)
{
    for (auto i: items(event->pos()))
    {
        if (ButtonRectItem == i)
        {
            //ButtonRectItem->setBrush(Qt::red);
            ButtonRectItem->setOpacity(1);
        }

    }

}
void View::mouseReleaseEvent(QMouseEvent * event)
{
    for (auto i: items(event->pos()))
    {
        if (ButtonRectItem == i)
        {
            ButtonRectItem->setOpacity(0.9);
            close();
        }
        else
        {
            ButtonRectItem->setOpacity(0.9);
        }

    }

}


inline QString Tostr(QRect r)
{
    return QString("(%1,%2)[%3x%4]").arg(r.x(),5).arg(r.y(),5).arg(r.width(),4).arg(r.height(),4);
}

void View::ShowRefreshment(QString pic_path, QString clock, QString text, bool isLogging, bool isPrettyFont, QString AspectMode)
{
    picture_path = pic_path;
    //qDebug() << "isLogging " << isLogging;
    QPixmap pic = QPixmap(picture_path);
    QRect pic_rect = pic.rect();
    QRect desktop = QApplication::desktop()->geometry();
    QRect default_screen = QApplication::desktop()->screenGeometry(-1);

    myscene->setSceneRect(desktop);
    myscene->setBackgroundBrush(QBrush(Qt::black));

    QString ratio_case = "No image";
    double ratio_pic = 0.0;
    double ratio_desk = (double)desktop.width()/desktop.height();
    double ratio_default_screen = (double)default_screen.width()/default_screen.height();

    if (!pic.isNull())
    {
        ratio_pic = (double)pic.width()/pic.height();

        QGraphicsPixmapItem * pic_item;

        if (AspectMode == tr("Outside"))                                 ratio_case = "Full_desktop Outside";
        else if (AspectMode == tr("Inside"))                             ratio_case = "Default_screen Inside";

        else if (std::abs(ratio_desk - ratio_pic) < 0.6 )                ratio_case = "Full_desktop Outside";
        else if (std::abs(ratio_default_screen - ratio_pic) < 0.6 )      ratio_case = "Default_screen Outside";
        //else if (ratio_pic > 2.9 && ratio_desk > 2.9)                    ratio_case = "Full_desktop Outside";
        else if (std::abs(ratio_desk - ratio_pic) < std::abs(ratio_default_screen - ratio_pic)) ratio_case = "Full_desktop Inside";
        else                                                             ratio_case = "Default_screen Inside";


        if ( ratio_case == "Full_desktop Outside")
        {
            pic = pic.scaled(desktop.size(),Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(desktop.topLeft());
        }
        else if (ratio_case == "Default_screen Outside")
        {
            pic = pic.scaled(default_screen.size(),Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(default_screen.topLeft());

        }
        else if (ratio_case == "Full_desktop Inside")
        {
            pic = pic.scaled(desktop.size(),Qt::KeepAspectRatio);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(desktop.topLeft());
        }
        else //if (ratio_var == "Default_screen Inside")
        {
            pic = pic.scaled(default_screen.size(),Qt::KeepAspectRatio);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(default_screen.topLeft());
        }


        pic_item->setZValue(-1);
        myscene->addItem( pic_item);

    }

    if (isLogging)
    {
        QString Logging_str = "";

        if (!pic.isNull())
        {
            Logging_str += pic_path + "\n";
            Logging_str += QString("image              %1  %2\n").arg(ratio_pic,4,'f',2).arg(Tostr(pic_rect));
        }

        Logging_str += QString("full desktop       %3  %4\n").arg(ratio_desk,4,'f',2).arg(Tostr(desktop));


        for (int i = 0; i< QApplication::desktop()->screenCount(); i++)
        {
            if (QApplication::desktop()->primaryScreen() == i)
            {
                Logging_str += QString("default screen #%1  %2  %3\n").arg(i)
                        .arg(ratio_default_screen,4,'f',2)
                        .arg(Tostr (default_screen));
            }
            else
            {
                QWidget * scr_i = QApplication::desktop()->screen(i);
                Logging_str += QString("        screen #%1  %2  %3\n").arg(i)
                        .arg((double)scr_i->width()/scr_i->height(),4,'f',2)
                        .arg(Tostr (scr_i->rect()));
            }
        }

        Logging_str += QString("Ratio case = %1").arg(ratio_case);

        /*
        QGraphicsTextItem * LoggingItem = new QGraphicsTextItem();
        LoggingItem->setPlainText(Logging_str);
        LoggingItem->setPos(default_screen.topLeft() + QPoint(50, default_screen.height()/2));
        LoggingItem->setZValue(-1);

        QFont font("Monospace");
        font.setStyleHint(QFont::TypeWriter);

        LoggingItem->setFont(font);
        LoggingItem->setDefaultTextColor(QColor(Qt::white));
        LoggingItem->setOpacity(0.5);

        QGraphicsRectItem * LoggingItemRect = new QGraphicsRectItem();
        LoggingItemRect->setRect(LoggingItem->boundingRect());
        LoggingItemRect->setPos(LoggingItem->pos());
        LoggingItemRect->setZValue(-1);
        LoggingItemRect->setPen(Qt::NoPen);
        LoggingItemRect->setBrush(Qt::black);
        LoggingItemRect->setOpacity(0.5);

        myscene->addItem( LoggingItemRect);
        myscene->addItem( LoggingItem);
        */

        QFile file("LoggingDisplay.txt");
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            out << Logging_str;
            file.close();
        }

    }


    this->ProgressBarText->setPlainText("0:00");

    int label_height = this->ProgressBarText->boundingRect().height();
    int label_y_pos = default_screen.height() - 25 - label_height;
    int label_x_pos = default_screen.left()+2.5*default_screen.width()/5 - this->ProgressBarText->boundingRect().width()/2 + 10;

    this->ProgressBarText->setPos(label_x_pos , label_y_pos);

    ProgressBarRect = QRect(default_screen.left()+default_screen.width()/5 ,label_y_pos, 3*default_screen.width() / 5,label_height );

    ProgressBarBackground->setRect(ProgressBarRect);
    ProgressBar->setRect(ProgressBarRect.adjusted(0,0,-ProgressBarRect.width()/2,0));

    ProgressBarBound->setRect(ProgressBarRect.adjusted(-1, -1, 0, 0));


    ButtonRectItem->setRect(QRect(ProgressBarBackground->boundingRect().right()+ 25 , label_y_pos, 100,label_height ));

    QPointF p = ButtonRectItem->rect().center() -
            QPoint(ButtonText->boundingRect().width(),ButtonText->boundingRect().height())/2;
    ButtonText->setPos(p.x(),p.y());

    if (!clock.isEmpty())
    {

        clockItem = new QGraphicsTextItem();
        clockItem->setFont(QFont(isPrettyFont?"UKIJ Diwani Yantu":"",30,30));
        clockItem->setDefaultTextColor(QColor(Qt::white));
        clockItem->setOpacity(0.5);
        clockItem->setZValue(-1);
        clockItem->setPlainText(clock);
        clockItem->setPos(default_screen.topRight() + QPoint(-50 -clockItem->boundingRect().width() , 25) );

        QGraphicsRectItem * clockItemRect = new QGraphicsRectItem();
        clockItemRect->setRect(clockItem->boundingRect());
        clockItemRect->setPos(clockItem->pos());
        clockItemRect->setZValue(-1);
        clockItemRect->setPen(Qt::NoPen);
        clockItemRect->setBrush(Qt::black);
        clockItemRect->setOpacity(0.5);

        myscene->addItem( clockItemRect);
        myscene->addItem(clockItem);
    }

    if (!text.isEmpty())
    {
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        textItem->setPlainText(text);
        textItem->setPos(default_screen.topLeft() + QPoint(50, 25));
        textItem->setZValue(-1);
        textItem->setFont(QFont(isPrettyFont?"UKIJ Diwani Yantu":"",30,30));

        textItem->setDefaultTextColor(QColor(Qt::white));
        textItem->setOpacity(0.5);

        QGraphicsRectItem * textItemRect = new QGraphicsRectItem();
        textItemRect->setRect(textItem->boundingRect());
        textItemRect->setPos(textItem->pos());
        textItemRect->setZValue(-1);
        textItemRect->setPen(Qt::NoPen);
        textItemRect->setBrush(Qt::black);
        textItemRect->setOpacity(0.5);

        myscene->addItem( textItemRect);
        myscene->addItem( textItem);
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
    this->ProgressBarRect.setRight( qRound(ProgressBarBackground->rect().left() + ProgressBarBackground->rect().width() * (1-ratio) ));
    ProgressBar->setRect(ProgressBarRect);


    this->ProgressBarText->setPlainText(remains_str);

    if (clockItem != NULL)
        clockItem->setPlainText( QTime::currentTime().toString("hh:mm"));
}


void View::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Delete)
    {
        QFile pic (picture_path);

        if (pic.exists() && pic.remove())
        {
            for (auto item : myscene->items())
                if (item->zValue() == -1 && item->boundingRect().height() > 500)
                {
                    myscene->removeItem(item);
                    delete item;
                }
        }
    }
    else if (event->key() == Qt::Key_Q)
    {
        close();
    }

    QGraphicsView::keyPressEvent(event);
}



void View::closeEvent(QCloseEvent *event)
{
    emit view_close();
    for (auto item : myscene->items())
        if (item->zValue() == -1)
        {
            myscene->removeItem(item);
            delete item;
        }

    hide();
    event->ignore();

    QGraphicsView::close();
}
