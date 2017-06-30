//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#ifndef TESTITEM_H
#define TESTITEM_H
#include <QOpenGLWidget>



class TestItem : public QOpenGLWidget
{
Q_OBJECT
public:
    explicit TestItem(const QRect &screen);

public slots:
    void animate(double _hue, int _diameter);

private:
    void paintGL();
    void resizeGL(int w, int h);
    void initializeGL();

    double hue = 0;
    int diameter = 0;
    QRect screen;
};

#endif // REFRESHMENT_H
