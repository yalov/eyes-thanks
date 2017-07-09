//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "testitem.h"

#include <QPainter>
#include <QDebug>

#include <GL/glu.h>


TestItem::TestItem(const QRect &_screen) : QOpenGLWidget()
{
    //setAttribute(Qt::WA_OpaquePaintEvent);
    //setUpdatesEnabled(false);
    //setAutoFillBackground(false);
    setFixedSize(_screen.size());
    screen = _screen;
}


void TestItem::animate(double _hue = 0, int _diameter = 50)
{
    hue = _hue;
    diameter = _diameter;
    update();
    qDebug() << "animate";

}


void TestItem::initializeGL()
{
//    glClearColor(0, 0, 0, 1);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_LIGHTING);
//    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//    glEnable(GL_COLOR_MATERIAL);
    qDebug() << "initializeGL";

}

void TestItem::paintGL()
{

//    QColor c1 = QColor::fromHsvF(fmod(hue,1), 1,0.4);
//    QColor c2 = QColor::fromHsvF(fmod(hue+0.333,1),1,0.8);
//    QColor c3 = QColor::fromHsvF(fmod(hue+0.666,1),1,0.4);

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    QPainter * painter = new QPainter(this);
//    //painter->setRenderHint(QPainter::Antialiasing);
//    for (int i=0; i<200; i++)
//    {
//    painter->setBrush(QColor::fromHsvF(fmod(hue+ rand()%100/100.0,1),1,1,0.5));
//    painter->setPen(Qt::NoPen);
//    diameter = /*rand()%50+50*/100;
//    QRect r(qrand() % (screen.width() - diameter),
//            qrand() % (screen.height() - diameter), diameter, diameter);
//    painter->drawEllipse(r);
//    }
//    painter->end();


    QPainter * painter = new QPainter(this);
    //painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QColor::fromHsvF(fmod(hue,1),1,1,0.5));
    painter->setPen(Qt::NoPen);

    QRect r(qrand() % (screen.width() - diameter),
            qrand() % (screen.height() - diameter), diameter, diameter);
    painter->drawEllipse(r);

    painter->end();
}

void TestItem::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
    qDebug() << "resizeGL";
//    glViewport(0, 0, w, h);
}



//        case OPENGLDOTS: {
//            if (ElapsedTimerDot == nullptr) {
//                ElapsedTimerDot = new QElapsedTimer();
//                testitem = new TestItem(screen);
//                connect(this, SIGNAL(opengl_update(double, int)), testitem, SLOT(animate(double, int)));
//                QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
//                proxy->setWidget(testitem);
//                myscene->addItem(proxy);

//                int diameter_dot = qrand() % 100 + 50;
//                emit opengl_update(fmod(hue_now, 1), diameter_dot);

//            }
//            else if (ElapsedTimerDot->elapsed() > 100) {
//                ElapsedTimerDot->start();

//                int diameter_dot = qrand() % 100 + 50;
//                //testitem->animate(fmod(hue_now,1), diameter_dot);
//                emit opengl_update(fmod(hue_now, 1), diameter_dot);
//            }
//            break;
//        }
