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

#ifndef VIEW_H
#define VIEW_H
#include <QtGui>
#include <QtCore>
#include <QCloseEvent>
#include <QGraphicsView>

#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include "timer.h"
#include "graphicstextitemfixbound.h"

#define WidgetMode

class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QWidget *parent = 0);
    ~View();

    void ShowRefreshment(QString pic_path, QString clock, QString ProgressBarText, bool isLogging, bool isPrettyFont, QString AspectMode);
    void UpdateValues(QString remains_str, double ratio);

signals:
    void view_close();


private:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

    QString picture_path;

    QGraphicsScene * myscene;
    QGraphicsTextItem * clockItem;

    QGraphicsRectItem * ProgressBar;
    QGraphicsRectItem * ProgressBarBound;
    QGraphicsRectItem * ProgressBarBackground;
    GraphicsTextItemFixBound * ProgressBarText;
    QRect ProgressBarRect;
    QGraphicsRectItem * ButtonRectItem;
    GraphicsTextItemFixBound * ButtonText;
};




#endif // VIEW_H
