//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//


#include <QGraphicsBlurEffect>
#include "view.h"
#include "timer.h"
#include "transliteration-iso9a.h"
#include "QUtfString.h"
#include "QUtfStringList.h"
#include "charactersets.h"

View::View(QWidget *parent): QGraphicsView(parent),
    default_screen(QApplication::desktop()->screenGeometry(-1)),
    desktop(QApplication::desktop()->geometry()),
    clockItem(nullptr), textItem(nullptr), setting(), Item(nullptr),
    MethodIndex(-1), Hue_start(0), IsBackgroundUpdate(false), RunnedFirstTime(false)
{
#ifdef DEPLOY
    setWindowFlags(Qt::WindowStaysOnTopHint);
#endif

    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setAttribute(Qt::WA_DeleteOnClose);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
    setStyleSheet("QGraphicsView { border-style: none; }");

    myscene = new QGraphicsScene();
    myscene->setBackgroundBrush(Qt::black);
    setScene(myscene);

    ProgressBarText = new QGraphicsSimpleTextItem();
    QFont font;
    font.setPointSize(16);
    ProgressBarText->setFont(font);
    ProgressBarText->setBrush(Qt::black);
    ProgressBarText->setZValue(3);
    ProgressBarText->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    myscene->addItem(ProgressBarText);

    ProgressBar = new QGraphicsRectItem();
    ProgressBar->setPen(Qt::NoPen);
    ProgressBar->setBrush(Qt::white);
    ProgressBar->setOpacity(0.8);
    ProgressBar->setZValue(2.5);
    ProgressBar->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    myscene->addItem(ProgressBar);

    ProgressBarBackground = new QGraphicsRectItem();
    ProgressBarBackground->setPen(Qt::NoPen);
    ProgressBarBackground->setBrush(Qt::white);
    ProgressBarBackground->setOpacity(0.4);
    ProgressBarBackground->setZValue(2);
    myscene->addItem(ProgressBarBackground);

    ProgressBarBound = new QGraphicsRectItem();
    ProgressBarBound->setPen(QPen(Qt::black));
    ProgressBarBound->setBrush(Qt::NoBrush);
    ProgressBarBound->setZValue(3);
    myscene->addItem(ProgressBarBound);

    ButtonText = new QGraphicsSimpleTextItem();
    font.setPointSize(14);
    ButtonText->setFont(font);
    ButtonText->setBrush(Qt::black);
    ButtonText->setZValue(3);
    ButtonText->setText(tr("Close"));
    myscene->addItem(ButtonText);

    ButtonRectItem = new QGraphicsRectItem();
    ButtonRectItem->setPen(QPen(Qt::black));
    ButtonRectItem->setBrush(Qt::white);
    ButtonRectItem->setOpacity(0.4);
    ButtonRectItem->setZValue(2);
    myscene->addItem(ButtonRectItem);
}

inline QString str_from(QRect r)
{
    return QString("(%1,%2)[%3x%4]").arg(r.x(), 5).arg(r.y(), 5).arg(r.width(), 4).arg(r.height(), 4);
}

inline QString str_from(QSize s)
{
    return QString("             [%3x%4]").arg(s.width(), 4).arg(s.height(), 4);
}

inline static void CreateBlurredBackgroundForItem(const QGraphicsItem *item, QGraphicsScene *scene)
{
    QGraphicsRectItem *rect = new QGraphicsRectItem();
    rect->setRect(item->boundingRect());
    rect->setPos(item->pos());
    rect->setZValue(2);
    rect->setPen(Qt::NoPen);
    rect->setBrush(Qt::black);
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
    blur->setBlurRadius(20);
    rect->setGraphicsEffect(blur);
    scene->addItem(rect);
}

void View::ShowRefreshment(const QList<QString> &pics_path, const QString &clock, const QString &text, const  Setting &_setting, Timer *viewtimer)
{
    setting = _setting;
    QElapsedTimer timer; timer.start();
    myscene->setSceneRect(desktop);
    qreal ratio_desk = desktop.ratio();
    qreal ratio_default_screen = default_screen.ratio();

    // ClockItem and TextItem
    if (!clock.isEmpty() || !text.isEmpty()) {
        int font_size = 30;
        QFont font(setting.isPrettyFont ? "UKIJ Diwani Yantu Mod" : "FreeSerif Mod", font_size, QFont::Bold);
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
            textItem->setFont(font);
            textItem->setPos(default_screen.topLeft() + QPoint(50, 25));
            textItem->setZValue(3);
            textItem->setBrush(fill_color);
            textItem->setPen(outline_color);
            //textItem->setPen(Qt::NoPen);
            textItem->setOpacity(opacity);
            myscene->addItem(textItem);

//            QGraphicsRectItem *rect = new QGraphicsRectItem();
//            rect->setRect(textItem->boundingRect());
//            rect->setPos(textItem->pos());
//            rect->setZValue(2);
//            rect->setPen(Qt::NoPen);
//            rect->setBrush(Qt::black);
//            rect->setOpacity(0.5);

//            QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
//            blur->setBlurRadius(20);
//            rect->setGraphicsEffect(blur);

//            myscene->addItem(rect);

        }
    }

    // Picture and Background
    QList<qreal> ratios_pic;
    QList<QPixmap> pics;
    QList<QString> pics_path_confirmed;

    for (auto pic_path : pics_path) {
        QPixmap pic = QPixmap(pic_path);
        if (!pic.isNull()) {
            pics.append(pic);
            ratios_pic.append((qreal)pic.width() / pic.height());
            pics_path_confirmed.append(pic_path);
        }
    }


    QString display_case = "No image";
    int Method = -1;

    if (pics.size() > 0) {
        // find out which one of pic apropriate, if "inside" - comparison to def screen, if outside or auto - comparison to desk
        qreal ratio = (setting.imageAspectMode == ImageAspectMode::Inside) ? ratio_default_screen : ratio_desk;
        qreal min_value = std::numeric_limits<qreal>::max();
        qreal min_index = 0;

        for (int i = 0; i < pics.size(); i++) {
            if (abs(ratios_pic[i] - ratio) < min_value) {
                min_value = abs(ratios_pic[i] - ratio);
                min_index = i;
            }
        }

        picture_path = pics_path_confirmed[min_index];
        QPixmap pic = pics[min_index];
        qreal ratio_pic = ratios_pic[min_index];

        if (setting.imageAspectMode == ImageAspectMode::Inside)      display_case = "Default_screen Inside";
        else if (setting.imageAspectMode == ImageAspectMode::Outside)     display_case = "Full_desktop Outside";
        else if (setting.imageAspectMode == ImageAspectMode::Auto) {
            if (std::abs(ratio_desk - ratio_pic) <= std::abs(ratio_default_screen - ratio_pic)) { // Full_desktop
                if (ratio_pic / ratio_desk < 0.7 || ratio_pic / ratio_desk > 1 / 0.7)                     display_case = "Full_desktop Inside";
                else                                                                                      display_case = "Full_desktop Outside";
            }
            else {   //Default_screen
                if (ratio_pic / ratio_default_screen < 0.7 || ratio_pic / ratio_default_screen > 1 / 0.7)  display_case = "Default_screen Inside";
                else                                                                                       display_case = "Default_screen Outside";
            }
        }


        QGraphicsPixmapItem *pic_item;
        if (display_case == "Full_desktop Outside") {
            pic = pic.scaled(desktop.size(), Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(desktop.topLeft() + QPoint(desktop.width() - pic.width(), desktop.height() - pic.height()) / 2);
        }
        else if (display_case == "Default_screen Outside") {
            pic = pic.scaled(default_screen.size(), Qt::KeepAspectRatioByExpanding);
            pic_item = new QGraphicsPixmapItem(pic);
            pic_item->setPos(default_screen.topLeft() + QPoint(default_screen.width() - pic.width(), default_screen.height() - pic.height()) / 2);

        }
        else if (display_case == "Full_desktop Inside") {
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
        Method = SetPredeterminedBackground(Hue_start);
    }

    // ProgressBar and ButtonRect
    {
        ProgressBarText->setText(viewtimer->remains_str());
        QPoint textsize = QPoint(ProgressBarText->boundingRect().width(), ProgressBarText->boundingRect().height());
        int y_pos = default_screen.height() - textsize.y() - 25 ;
        ProgressBarRect = QRect(default_screen.left() + default_screen.width() / 5, y_pos,
                                3 * default_screen.width() / 5, textsize.y());

        ProgressBarBackground->setRect(ProgressBarRect);
        ProgressBar->setRect(ProgressBarRect.adjusted(0, 0, 0/*-ProgressBarRect.width() / 2*/, 0));
        ProgressBarBound->setRect(ProgressBarRect.adjusted(-1, -1, 0, 0));
        ButtonRectItem->setRect(QRect(ProgressBarBackground->boundingRect().right() + 25, y_pos, 100, textsize.y()));
        ProgressBarText->setPos(ProgressBarRect.center() - textsize / 2);

        QPointF p = ButtonRectItem->rect().center() -
                    QPoint(ButtonText->boundingRect().width(), ButtonText->boundingRect().height()) / 2;
        ButtonText->setPos(p.x(), p.y());
    }

    // Logging
    if (setting.isLogging) {
        QString Logging_str = "";

        if (Method != -1) {
            QMetaEnum metaEnum = QMetaEnum::fromType<Methods>();
            display_case = metaEnum.valueToKey(Method);
        }
        Logging_str += QString("%1 Display case = %2, %3 ms., items = %4\n")
                       .arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"))
                       .arg(display_case).arg(timer.elapsed()).arg(myscene->items().size());

        qDebug().noquote()  << QString("%1 Display case = %2, %3 ms., items = %4")
                            .arg(QTime::currentTime().toString("mm.ss.zzz"))
                            .arg(display_case).arg(timer.elapsed()).arg(myscene->items().size());


        for (int i = 0; i < pics.size(); i++) {
            if (picture_path == pics_path_confirmed[i])
                Logging_str += QString(" - image showing      %1  %2 %3\n")
                               .arg(ratios_pic[i], 4, 'f', 2).arg(str_from(pics[i].size())).arg(pics_path_confirmed[i]);
            else
                Logging_str += QString(" - image              %1  %2 %3\n")
                               .arg(ratios_pic[i], 4, 'f', 2).arg(str_from(pics[i].size())).arg(pics_path_confirmed[i]);
        }

        if (default_screen == desktop)
            Logging_str += QString(" - single screen      %3  %4\n").arg(ratio_desk, 4, 'f', 2).arg(str_from(desktop.size()));
        else {
            Logging_str += QString(" - full desktop       %3  %4\n").arg(ratio_desk, 4, 'f', 2).arg(str_from(desktop.size()));

            for (int i = 0; i < QApplication::desktop()->screenCount(); i++) {
                if (QApplication::desktop()->primaryScreen() == i) {
                    Logging_str += QString(" - - default screen #%1  %2  %3\n").arg(i)
                                   .arg(ratio_default_screen, 4, 'f', 2)
                                   .arg(str_from(default_screen));
                }
                else {
                    QRect scr_i = QApplication::desktop()->screenGeometry(i);
                    Logging_str += QString(" - -         screen #%1  %2  %3\n").arg(i)
                                   .arg((qreal)scr_i.width() / scr_i.height(), 4, 'f', 2)
                                   .arg(str_from(scr_i));
                }
            }
        }


        LogToFile("LoggingDisplay.txt", Logging_str);
    }

#ifndef DEPLOY
    // SaveSceneToFile
    SaveSceneToFile("c:/Users/User/Pictures/Screenshots/EyesThanks");
#endif

    qDebug().noquote()  << QTime::currentTime().toString("ss.zzz") << "ShowRefreshment saving by" << timer.restart() << "ms.";

#ifdef __linux__ // showFullScreen() doesn't work
    setWindowFlags(Qt::X11BypassWindowManagerHint);
    show();
    setGeometry(desktop);
#else
    activateWindow();
    showFullScreen();
    setFocus();
    setGeometry(desktop);
#endif
}

int View::SetPredeterminedBackground(qreal hue_now)
{
    // HSV
    // S - from uncolor to color
    // V - from black to color or white
    // 0 0 - black   // 1 0 - black
    // 0 0.5 grey    // 1 1 - color
    // 0 1 - white
    Q_UNUSED(hue_now);

    QElapsedTimer timer;
    timer.start();



    if (MethodIndex == -1) {
        if (setting.isSpectrum) MethodsEnabled.append(SPECTRUM);
        if (setting.isTiling  ) MethodsEnabled.append(TILING);
        if (setting.isStripes ) MethodsEnabled.append(STRIPES);
        if (setting.isCircle  ) MethodsEnabled.append(RANDOM_CIRCLE);
        if (setting.isCircles ) MethodsEnabled.append(RANDOM_CIRCLES);
        if (setting.isNeo     ) MethodsEnabled.append(NEO);

        if (MethodsEnabled.size()==0) {
            myscene->setBackgroundBrush(Qt::black);
            return -1;
        }

        MethodIndex = qrand() % MethodsEnabled.size();

        if (MethodsEnabled.at(MethodIndex) == RANDOM_CIRCLE)
            IsBackgroundUpdate = true;
    }

    switch (MethodsEnabled.at(MethodIndex)) {
    case SPECTRUM: {
        QLinearGradient rainbow(desktop.topLeft(), desktop.topRight());

        for (qreal hue = 0; hue <= 1.0; hue += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(0 + hue * 0.8, 1), 1, 1);
            rainbow.setColorAt(hue, c);
        }
        myscene->setBackgroundBrush(QBrush(rainbow));
        //ProgressBar->setBrush(QBrush(rainbow));
        //ProgressBarBackground->setBrush(QBrush(rainbow));
        //ButtonRectItem->setBrush(QBrush(rainbow));

        QLinearGradient vertical(desktop.topLeft(), desktop.bottomLeft());
        int mode = qrand() % 3;
        if (mode == 0) {
            //vertical.setColorAt(0,    QColor::fromRgbF(0, 0, 0, 0.6));
            //vertical.setColorAt(0.25, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.35,  QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.5,  QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.65,  QColor::fromRgbF(0, 0, 0, 1.0));

            //vertical.setColorAt(0.75, QColor::fromRgbF(0, 0, 0, 1.0));
            //vertical.setColorAt(1,    QColor::fromRgbF(0, 0, 0, 0.6));
        }
        else if (mode == 1) {
            //vertical.setColorAt(0,    QColor::fromRgbF(0, 0, 0, 0.6));
            //vertical.setColorAt(0.25, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.275, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.375, QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.475, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.525, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.625, QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.725, QColor::fromRgbF(0, 0, 0, 1.0));

            //vertical.setColorAt(0.75, QColor::fromRgbF(0, 0, 0, 1.0));
            //vertical.setColorAt(1,    QColor::fromRgbF(0, 0, 0, 0.6));
        }
        else if (mode == 2) {
            //vertical.setColorAt(0,    QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.25, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.30, QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.35, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.4,  QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.5,  QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.6,  QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.65, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.70, QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.75, QColor::fromRgbF(0, 0, 0, 1.0));
            //vertical.setColorAt(1,    QColor::fromRgbF(0, 0, 0, 0.6));
        }
        QGraphicsRectItem *item = new QGraphicsRectItem(desktop);
        item->setPen(Qt::NoPen);
        item->setBrush(vertical);
        myscene->addItem(item);

        break;
    }
    case TILING: {
        switch (qrand() % 5) {
        case 0: { // square
            int h = qrand() % 20 + 9;
            int w = qRound(h * desktop.ratio());
            qreal cellw = desktop.widthF() / w;
            qreal cellh = desktop.heightF() / h;

            for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                    QGraphicsRectItem *squareItem = new QGraphicsRectItem();
                    squareItem->setPen(QPen(Qt::black, 1));
                    squareItem->setPos(desktop.topLeft() + QPoint(i * cellw, j * cellh));
                    squareItem->setRect(0, 0, cellw, cellh);
                    qreal hue = Hue_start + qreal(i) * 0.8 / w ;
                    hue -= floor(hue);
                    qreal value = qrand() % 50 / 100.0 + 0.25 ;
                    squareItem->setBrush(QColor::fromHsvF(hue, 1.0, value));
                    myscene->addItem(squareItem);
                }
            }
            break;
        }
        case 1: { // square transform
            int h =  qrand() % 20 + 7;
            qreal w = qRound(h * desktop.ratio() * 2 * 2) / 2; // ok.
            qreal cellh = desktop.heightF() / h;
            qreal cellw = desktop.widthF() / (w / 2);

            auto square = QPolygonF()
                          << QPointF(-cellw / 2, 0)
                          << QPointF(0, -cellh / 2)
                          << QPointF(cellw / 2, 0)
                          << QPointF(0, cellh / 2);
            for (int i = 0; i < w + 1 ; i++) {
                for (int j = 0; j < h + 1; j++) {
                    qreal hue = Hue_start + qreal(i) * 0.8 / w;
                    TilingItem *squareItem = new TilingItem(square, hue);
                    squareItem->setPos(desktop.topLeft() + QPointF(i * 0.5 * cellw, (j + (i % 2) * 0.5) * cellh));
                    myscene->addItem(squareItem);
                }
            }
            break;
        }
        case 2: { // hexagon
            qreal k = 2 / qSqrt(3);
            int h = qrand() % 30 + 7;
            qreal cellh = desktop.heightF() / h;
            qreal cellw = cellh * k;
            int x = qRound((2 * desktop.widthF() / cellw + 1) / 3);
            int w = 2 * x;
            cellw = 2 * desktop.widthF() / (3 * x - 1);

            auto hexagon = QPolygonF()
                           << QPointF(-cellw / 2,  0)
                           << QPointF(-cellw / 4, -cellh / 2)
                           << QPointF(cellw / 4, -cellh / 2)
                           << QPointF(cellw / 2,  0)
                           << QPointF(cellw / 4,  cellh / 2)
                           << QPointF(-cellw / 4,  cellh / 2);

            for (int i = 0; i < w + 1; i++) {
                for (int j = 0; j < h + 1; j++) {
                    qreal hue = Hue_start + qreal(i) * 0.8 / w;
                    TilingItem *hexagonItem = new TilingItem(hexagon, hue);
                    hexagonItem->setPos(desktop.topLeft() + QPointF((0.75 * i - 0.25) * cellw, (j - 0.5) * cellh + (i % 2 ? 0 : 0.5 * cellh)));
                    myscene->addItem(hexagonItem);
                }
            }
            break;
        }
        case 3: { // triangle
            qreal k = 2 / qSqrt(3);
            int h = 2 * (qrand() % 10 + 3); // even
            qreal cellh = desktop.heightF() / h;
            qreal cellw = cellh * k;
            int x = qRound(desktop.widthF() / cellw);
            int w = 2 * x - 1;
            cellw = desktop.widthF() / x;

            auto triangle = QPolygonF()
                            << QPointF(-cellw / 2, cellh / 3)
                            << QPointF(0,    - 2 * cellh / 3)
                            << QPointF(cellw / 2,  cellh / 3);

            for (int i = 0; i < w + 2  ; i++) {
                for (int j = 0; j < h; j++) {
                    qreal hue = Hue_start + qreal(i) * 0.8 / w;
                    TilingItem *triangleItem = new TilingItem(triangle, hue);
                    triangleItem->setPos(desktop.topLeft() + QPointF(
                                             cellw * (0.5 * i),
                                         cellh * j + cellh * ((j + i) % 2 ? 1.0 / 3.0 : 2.0 / 3.0)));
                    myscene->addItem(triangleItem);

                    if ((j + i) % 2) triangleItem->setRotation(180);
                }
            }
            break;
        }
        case 4: { // square+triangle
            int h = 2 * qrand() % 10 + 5; // odd
            int w = qCeil(h * desktop.ratio());
            qreal alpha = M_PI / 3;
            qreal cell = desktop.heightF() / h;
            qreal a = cell / (qSin(M_PI / 4 + alpha / 2) * qSqrt(2) * 2); // half square_side

            auto points = QVector<QPointF>()
                          << QPointF(-a,  a) << QPointF(-a, -a)
                          << QPointF(a, -a) << QPointF(a,  a)
                          << QPointF(2 * a * qCos(alpha) - a, -2 * a * qSin(alpha) - a)
                          << QPointF(2 * a * qSin(alpha) + a,  2 * a * qCos(alpha) - a)
                          << QPointF(-2 * a * qCos(alpha) + a, -2 * a * qSin(alpha) - a)
                          << QPointF(2 * a * qSin(alpha) + a, -2 * a * qCos(alpha) + a) ;
            for (int i = 0; i < w + 1; i++) {
                for (int j = 0; j < h + 1; j++) {
                    qreal hue = Hue_start + qreal(i) * 0.8 / w;
                    qreal hue1 = hue +  0.5 * 0.8 / w;
                    QPointF pos = desktop.topLeft() + QPointF{cell  * i, cell  * j};

                    QGraphicsItemGroup *group = new QGraphicsItemGroup();
                    myscene->addItem(group);

                    auto square = QPolygonF() << points[0] << points[1] << points[2] << points[3];
                    TilingItem *squareItem = new TilingItem(square, hue);
                    squareItem->setPos(pos);
                    squareItem->setZValue(0.5);
                    group->addToGroup(squareItem);

                    auto triangleUp = QPolygonF() << points[1] << points[2] << points[((j + i) % 2) ? 6 : 4];
                    TilingItem *triangleUpItem = new TilingItem(triangleUp, hue);
                    triangleUpItem->setPos(pos);
                    triangleUpItem->setZValue(1);
                    group->addToGroup(triangleUpItem);

                    auto triangleRight = QPolygonF() << points[2] << points[3] << points[((j + i) % 2) ? 7 : 5];
                    TilingItem *triangleRightItem = new TilingItem(triangleRight, hue1);
                    triangleRightItem->setPos(pos);
                    triangleRightItem->setZValue(1);
                    group->addToGroup(triangleRightItem);

                    group->setTransformOriginPoint(pos);

                    if ((j + i) % 2)
                        group->setRotation(-alpha / 2 * 180 / M_PI);
                    else
                        group->setRotation(alpha / 2 * 180 / M_PI);
                }
            }
            break;
        }
        default:
            break;
        }

        break;
    }
    case STRIPES: {
        QLinearGradient rainbow(desktop.topLeft(), desktop.topRight());

        for (qreal ratio = 0, hue = 0; ratio <= 1.0; ratio += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(Hue_start + hue * 0.8, 1), 1, 0.9);
            rainbow.setColorAt(1 - ratio, c);
            hue += 1. / 25;
        }
        myscene->setBackgroundBrush(QBrush(rainbow));

        qreal ratio = desktop.ratio();
        qreal standard_ratio = 16.0 / 9.0;
        qreal k = ratio / standard_ratio;
        int stripes_count = 10;

        int stripe_segment = desktop.width() / int(stripes_count * k);
        int stripe_width = stripe_segment * 0.8;

        bool stripes_gradient = qrand() % 2;
        if (stripes_gradient) {
            for (int i = -stripe_width / 2; i < desktop.width(); i = i + stripe_segment) {
                QGraphicsRectItem *item = new QGraphicsRectItem(desktop.left() + i, 0, stripe_width, desktop.height());
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
            for (int i = -stripe_width / 2; i < desktop.width(); i = i + stripe_segment) {
                QGraphicsLineItem *item = new QGraphicsLineItem(desktop.left() + i + stripe_width / 2, 0, desktop.left() + i + stripe_width / 2, desktop.height());
                item->setPen(QPen(QColor(Qt::black), stripe_width));
                item->setOpacity(0.7);
                myscene->addItem(item);
            }
        }
        break;
    }
    case RANDOM_CIRCLE: {
        if (Item == nullptr) {
            Item = new QGraphicsEllipseItem();
            myscene->addItem(Item);
        }

        int diameter_dot = qMin(desktop.width(), desktop.height()) / 10;
        QRect r(desktop.topLeft() +
                QPoint(qrand() % (desktop.width() - diameter_dot),
                qrand() % (desktop.height() - diameter_dot)), QSize(diameter_dot, diameter_dot));

        Item->setRect(r);
        Item->setBrush(QColor::fromHsvF(qrand() % 256 / 255.0, 1, 1, 0.5));

        break;
    }
    case RANDOM_CIRCLES: {
        QGraphicsItemGroup *group =  new QGraphicsItemGroup();
        myscene->addItem(group);

        int count_item = desktop.height() * desktop.width() / (1920.0 * 1080.0) * 100;
        for (int i = 0; i < count_item; i++) {
            int diameter_dot = qrand() % 100 + 50;
            QRect r(desktop.topLeft() +
                    QPoint(qrand() % (desktop.width() - diameter_dot),
                    qrand() % (desktop.height() - diameter_dot)), QSize(diameter_dot, diameter_dot));

            QGraphicsEllipseItem *item = new QGraphicsEllipseItem(r);
            item->setBrush(QColor::fromHsvF(fmod(Hue_start + qreal(i) / count_item / 2, 1), 1, 1));
            item->setPen(Qt::NoPen);
            item->setOpacity(qreal(i) / count_item / 2);
            group->addToGroup(item);
        }

        break;
    }
    case NEO: {
        QFont font_background("FreeSerif Mod", 17);
        QFont font_foreground("FreeSerif Mod", 100);

        if (clockItem) {
            clockItem->setBrush(QColor(0, 128, 0));
            clockItem->setPen(Qt::NoPen);
            clockItem->setFont(QFont("FreeSerif Mod", clockItem->font().pointSize()));
            clockItem->setOpacity(1);
            CreateBlurredBackgroundForItem(clockItem, myscene);
        }
        if (textItem)
            myscene->removeItem(textItem);

        if (qrand() % 10 == 0) {
            QString name = transliteraction(qgetenv("USER"));
            if (name.isEmpty()) name = transliteraction(qgetenv("USERNAME"));
            if (name.isEmpty() || name == "User" || name.size() > 25 || (qrand() % 10 == 0))
                name = transliteraction(QString("Neo"));

            QGraphicsSimpleTextItem *centerTextItem = new QGraphicsSimpleTextItem();
            myscene->addItem(centerTextItem);
            centerTextItem->setText(QString("Wake up, %1...").arg(name));
            centerTextItem->setBrush(QColor(0, 48, 0));
            centerTextItem->setPen(QPen(QColor(0, 255, 0), 2));
            centerTextItem->setFont(font_foreground);
            centerTextItem->setZValue(3);
            QPoint crect(centerTextItem->boundingRect().width(),
                         centerTextItem->boundingRect().height());

            if (crect.x() < default_screen.width() * 0.8)
                centerTextItem->setPos(default_screen.center() - crect / 2);
            else {
                centerTextItem->setText(QString("Wake up,\n%1...").arg(name));
                crect = QPoint(centerTextItem->boundingRect().width(),
                               centerTextItem->boundingRect().height());
                centerTextItem->setPos(default_screen.center() - crect / 2);
            }
        }

        QFontMetrics fm(font_background);
        int basic_height =  fm.boundingRect("А").height();
        int basic_width =  fm.boundingRect("А").width();

        QGraphicsItemGroup *group =  new QGraphicsItemGroup();
        myscene->addItem(group);

        CharacterSets characterSets(":res/character_sets.xml");
        //              -------
        int chs_index = qrand() % characterSets.size();
        //              -------
        QUtfString characters = characterSets.get_characters(chs_index);
        QUtfString title = characterSets.get_title(chs_index);

        const QVector<QPair<QUtfString, QUtfString>>            // workaround for alphabet members, not included in the unicode
        replaceRule {{u8"ch", QUtfString(0x2460)}, {u8"Ch", QUtfString(0x2461)}, // 0x2460 - Enclosed Alphanumerics Block
                     {u8"dź", QUtfString(0x2462)}, {u8"Dź", QUtfString(0x2463)}}; // for uniform randomize

        for (auto &pair : replaceRule)
            characters.replace(pair.first, pair.second);

        qDebug() << "unicode symbols ="  << characters.size();
        if (!title.isEmpty())
            for (int index = 0, max = title.size(); index < max; index++) {
                QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
                group->addToGroup(item);
                item->setBrush(Qt::darkGreen);
                item->setPen(Qt::NoPen);
                item->setFont(font_background);
                item->setText(QUtfString(title.at(index)));
                item->setPos(desktop.topLeft() + QPointF(basic_width - item->boundingRect().width() / 2, index * basic_height));
            }

        for (int pos_x = basic_width * (title.isEmpty() ? 1 : 4); pos_x < desktop.width(); pos_x += basic_width * 3) {
            int max_pos_y = 1.0 / 4.0 * desktop.height() + 3.0 / 4.0 * (qrand() % desktop.height());
            for (int pos_y = 0; pos_y < max_pos_y; pos_y += basic_height) {
                QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
                group->addToGroup(item);
                item->setBrush(Qt::darkGreen);
                item->setPen(Qt::NoPen);
                item->setFont(font_background);

                int charIndex = qrand() % characters.size();
                QUtfString character = characters.at(charIndex);

                for (auto &pair : replaceRule) {
                    if (character == pair.second) {
                        character = pair.first;
                        break;
                    }
                }
                item->setText(character);
                item->setPos(desktop.topLeft() + QPointF(pos_x - item->boundingRect().width() / 2, pos_y));
            }

        }
        myscene->setBackgroundBrush(Qt::black);
        break;
    }
    default:{ // MethodIndex == -1 && methodsEnabled.size() == 0
        myscene->setBackgroundBrush(Qt::black);
    }
    }

    return MethodIndex;
}

void View::SaveSceneToFile(QString dir_path)
{
//    myscene->setBackgroundBrush(Qt::transparent);
    QElapsedTimer timer;
    timer.start();

    QImage image(myscene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QDir dir(dir_path);
    if (!dir.exists()) dir.mkpath(".");
    QString methodName = QString(QMetaEnum::fromType<Methods>().valueToKey(MethodIndex));

    QPainter painter(&image);
    myscene->render(&painter);
    bool done = image.save(dir_path + QDir::separator() + QString("m%1-%2.png")
                           .arg(methodName)
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd-HHmmsszzz"))
                          );
    qDebug() << "SaveSceneToFile is" << done << "by" << timer.elapsed() << "ms.";
}

void View::UpdateValues(const QString &remains_str, const qreal &ratio)
{
    ProgressBarRect.setRight(qRound(ProgressBarBackground->rect().left() + ProgressBarBackground->rect().width() * (1 - ratio)));
    ProgressBar->setRect(ProgressBarRect);

    ProgressBarText->setText(remains_str);

    if (clockItem != nullptr)
        clockItem->setText(QTime::currentTime().toString("hh:mm"));

    if (picture_path.isEmpty() && IsBackgroundUpdate)
        SetPredeterminedBackground(Hue_start + ratio);
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
            SetPredeterminedBackground(qrand() % 360 / 360.);
        }
    }
    else if (event->key() == Qt::Key_Escape)
        close();
}

void View::mousePressEvent(QMouseEvent *event)
{
    for (auto i : items(event->pos())) {
        if (ButtonRectItem == i)
            ButtonRectItem->setOpacity(1);
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
    myscene->clear();
    myscene->deleteLater();

    emit view_close();
    //close();

    //hide();
    //event->ignore();

    //QGraphicsView::close();
    QGraphicsView::closeEvent(event);
}
