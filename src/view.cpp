//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//


#include <QGraphicsBlurEffect>
#include "view.h"
#include "timer.h"
#include "transliteration-iso9a.h"

View::View(QWidget *parent): QGraphicsView(parent),
    default_screen(QApplication::desktop()->screenGeometry(-1)),
    desktop(QApplication::desktop()->geometry()),
    clockItem(nullptr), textItem(nullptr), setting(), Item(nullptr),
    Method(-1), Hue_start(0), IsBackgroundUpdate(false), RunnedFirstTime(false)
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

//inline int positive_modulo(int i, int n)
//{
//    return (i % n + n) % n;
//}

void View::ShowRefreshment(const QList<QString> &pics_path, const QString &clock, const QString &text, const  Setting &setting, Timer *viewtimer)
{
    QElapsedTimer timer; timer.start();
    myscene->setSceneRect(desktop);
    qreal ratio_desk = desktop.ratio();
    qreal ratio_default_screen = default_screen.ratio();

    // ClockItem and TextItem
    if (!clock.isEmpty() || !text.isEmpty()) {
        int font_size = 30;
        QFont font(setting.isPrettyFont ? "UKIJ Diwani Yantu" : "FreeSerif Mod", font_size, QFont::Bold);
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
        Method = SetBackground(Hue_start);
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
                Logging_str += QString(" - image showing      %1  %2 %3\n").arg(ratios_pic[i], 4, 'f', 2).arg(str_from(pics[i].size())).arg(pics_path_confirmed[i]);
            else
                Logging_str += QString(" - image              %1  %2 %3\n").arg(ratios_pic[i], 4, 'f', 2).arg(str_from(pics[i].size())).arg(pics_path_confirmed[i]);
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
    setGeometry(desktop);
    activateWindow();
    showFullScreen();
    setFocus();
#endif
}

int View::SetBackground(qreal hue_now)
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

    if (Method == -1) {
        Method = qrand() % COUNT_OF_METHODS;

        if (Method == RANDOM_CIRCLE)
            IsBackgroundUpdate = true;
    }

    switch (Method) {
    case RAINBOW: {
        if (0.20 < Hue_start && Hue_start < 0.35)
            Hue_start = Hue_start * 5 + 0.35; // 1.35 << 2.10 // no blue in the center

        QLinearGradient rainbow(desktop.topLeft(), desktop.topRight());

        for (qreal hue = 0; hue <= 1.0; hue += 1. / 25) {
            QColor c = QColor::fromHsvF(fmod(Hue_start + hue * 0.8, 1), 1, 1);
            rainbow.setColorAt(hue, c);
        }
        myscene->setBackgroundBrush(QBrush(rainbow));
        ProgressBar->setBrush(QBrush(rainbow));
        ProgressBar->setOpacity(1);
        ProgressBarBackground->setBrush(QBrush(rainbow));
        ProgressBarBackground->setOpacity(0.4);
        ButtonRectItem->setBrush(QBrush(rainbow));

        QLinearGradient vertical(desktop.topLeft(), desktop.bottomLeft());
        int mode = qrand() % 3;
        if (mode == 0) {
            vertical.setColorAt(0,    QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.25, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.35,  QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.5,  QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.65,  QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.75, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(1,    QColor::fromRgbF(0, 0, 0, 0.6));
        }
        else if (mode == 1) {
            vertical.setColorAt(0,    QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.25, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.275, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.375, QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.475, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.525, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.625, QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.725, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.75, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(1,    QColor::fromRgbF(0, 0, 0, 0.6));
        }
        else if (mode == 2) {
            vertical.setColorAt(0,    QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.25, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.30, QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.35, QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.4,  QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.5,  QColor::fromRgbF(0, 0, 0, 0.4));
            vertical.setColorAt(0.6,  QColor::fromRgbF(0, 0, 0, 1.0));

            vertical.setColorAt(0.65, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(0.70, QColor::fromRgbF(0, 0, 0, 0.6));
            vertical.setColorAt(0.75, QColor::fromRgbF(0, 0, 0, 1.0));
            vertical.setColorAt(1,    QColor::fromRgbF(0, 0, 0, 0.6));
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
                    squareItem->setPos(i * cellw, j * cellh);
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
                    squareItem->setPos(i * 0.5 * cellw, (j + (i % 2) * 0.5) * cellh);
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
                    hexagonItem->setPos((0.75 * i - 0.25) * cellw, (j - 0.5) * cellh + (i % 2 ? 0 : 0.5 * cellh));
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
                    triangleItem->setPos(cellw * (0.5 * i),
                                         cellh * j + cellh * ((j + i) % 2 ? 1.0 / 3.0 : 2.0 / 3.0));
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
                    QPointF pos = {cell  * i, cell  * j};

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
    case RANDOM_CIRCLE: {
        if (Item == nullptr) {
            Item = new QGraphicsEllipseItem();
            myscene->addItem(Item);
        }
        else {
            int diameter_dot = qMin(desktop.width(), desktop.height()) / 10;
            QRect r(qrand() % (desktop.width() - diameter_dot),
                    qrand() % (desktop.height() - diameter_dot), diameter_dot, diameter_dot);

            Item->setRect(r);
            Item->setBrush(QColor::fromHsvF(qrand() % 256 / 255.0, 1, 1, 0.5));
        }
        break;
    }
    case RANDOM_CIRCLES: {
        QGraphicsItemGroup *group =  new QGraphicsItemGroup();
        myscene->addItem(group);

        int count_item = desktop.height() * desktop.width() / (1920.0 * 1080.0) * 100;
        for (int i = 0; i < count_item; i++) {
            int diameter_dot = qrand() % 100 + 50;
            QRect r(qrand() % (desktop.width() - diameter_dot),
                    qrand() % (desktop.height() - diameter_dot), diameter_dot, diameter_dot);

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

            QGraphicsRectItem *clockItemRect = new QGraphicsRectItem();
            clockItemRect->setRect(clockItem->boundingRect());
            clockItemRect->setPos(clockItem->pos());
            clockItemRect->setZValue(2);
            clockItemRect->setPen(Qt::NoPen);
            clockItemRect->setBrush(Qt::black);
            QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
            blur->setBlurRadius(20);
            clockItemRect->setGraphicsEffect(blur);
            myscene->addItem(clockItemRect);
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

        const QVector<QVector<QVector<quint32>>> unicode_limits = {
            /* 0 IPA             */ {{0x0250, 0x02AF}},
            /*   Runic           */ {{0x16A0, 0x16F0}},
            /* 2 Greek lower     */ {{0x3B1, 0x3C9}, {0x3D9, 0x3D9}, {0x3DB, 0x3DB}, {0x3DD, 0x3DD}, {0x3DF, 0x3DF}, {0x3D1, 0x3D1}},
            /*   Glagolitic upper*/ {{0x2C00, 0x2C2E}},
            /* 4 Gothic alphabet */ {{0x10330, 0x1034A}},
            /*   Math.Blackletter*/ {{0x1D56C, 0x1D59F}},
            /* 6 Phoenician      */ {{0x10900, 0x10915}},

            /*   Cherokee        */ {{0x13A0, 0x13F4}},
            /* 8 Tengwar         */ {{0xE000, 0xE031}, {0xE036, 0xE03D}, {0xE062, 0xE06F}},
            /*   Klingon         */ {{0xF8D0, 0xF8E8}, {0xF8F1, 0xF8F9}},
            /*10 Hex. Numerals   */ {{0x30, 0x39}, {0x41, 0x46}},
            /*   Roman Numerals  */ {{0x2160, 0x216F}, {0x2180, 0x2182}},
            /*12 Currency        */ {{0x24, 0x24}, {0xA2, 0xA5}, {0x20A0, 0x20BF}, {0xFF04, 0xFF04}},
            /*   Cyrillic slavic */ {{0x0401, 0x040C}, {0x040E, 0x044F}, {0x0451, 0x045C}, {0x045E, 0x045F}},
            /*14 All Cyrillic    */{
                {0x0410, 0x044f}, {0x0410, 0x044f}, //   Russian
                {0x0400, 0x0481}, {0x048A, 0x04ff}, //   Cyrillic
                {0x0500, 0x0527},                   //   Cyrillic Supplement
                {0xA640, 0xA66E}, {0xA680, 0xA697}  //   Cyrillic Extended-B
            },
            /*   All Latin       */ {
                {0x0041, 0x005A}, {0x0041, 0x005A}, {0x0041, 0x005A}, {0x0041, 0x005A}, //   A-Z
                {0x0061, 0x007A}, {0x0061, 0x007A}, {0x0061, 0x007A}, {0x0061, 0x007A}, //   a-z
                {0x00C0, 0x00D6}, {0x00D8, 0x00F6}, {0x00F8, 0x00FF},                   //   Latin-1 Supplement
                {0x0100, 0x01BF}, {0x01C4, 0x01D4}, {0x01E2, 0x0229}, {0x0232, 0x024F}  //   Latin Extended-A+B
            },
            /*16 Latin Europe    */ {
                {0x0041, 0x005A}, {0x0041, 0x005A},      //   A-Z
                {0x0061, 0x007A}, {0x0061, 0x007A},      //   a-z
                // French Latin-1 Supplement
                {0x00C0, 0x00D6},       // À(Á)Â(ÃÄÅÆÇ)ÈÉÊË(ÌÍ)ÎÏ(ÐÑ)(Ò)ÓÔ(ÕÖ)
                {0x00E0, 0x00F6},       // à(á)â(ãäåæç)èéêë(ìí)îï(ðñ)(ò)óô(õö)
                {0x00D8, 0x00DF},       // ØÙ(Ú)ÛÜ(ÝÞß)
                {0x00F8, 0x00FF},       // øù(ú)ûü(ýþÿ)
                // Belarusian Extended-A
                {0x0100, 0x0107}, {0x010C, 0x010D}, // (ĂăĄą)Ćć   Čč
                {0x0139, 0x013A}, {0x0141, 0x0148}, // Ĺĺ   ŁłŃń(ŅņŇň)
                {0x014A, 0x014D}, {0x0158, 0x0161}, // (ŊŋŌō) (Řř)Śś(ŜŝŞş)Šš
                {0x016A, 0x0173}, {0x0179, 0x017E}, // (Ūū)Ŭŭ(ŮůŰűŲų)   Źź(Żż)Žž

                {0x0116, 0x011B},                   // (Ėė)Ęę(Ěě) // Polish     +ext-a: ĄĆŁŃÓŚŹŻ
                {0x012E, 0x012F},                   // Įį         // Lithuanian +ext-a: ĄČĖĘŠŪŲŽ
                {0x0112, 0x0113}, {0x012A, 0x012B}, {0x0122, 0x0123},// Ēē Īī Ģģ // Latvian +ext-a: +ĀČŠŪŽ
                {0x0136, 0x0137}, {0x013B, 0x013C}, {0x0145, 0x0146},// Ķķ Ļļ Ņņ
                {0x010E, 0x0111}, {0x0164, 0x0165}, // Ďď(Ðđ) Ťť // Czech   +sup: ÓÁÉÍÝÚ +ext-a: ČŠŮŽĚŇŘ
                {0x011E, 0x011F},                   // Ğğ        // Turkish +sup: ÖÜÇ    +ext-a: Ş
                {0x013D, 0x013E}, {0x0154, 0x0155}, // Ľľ Ŕŕ     // Slovak  +sup: ÁÄÉÍÓÔÚÝŽ +ext-a: ČĎŤĹŇŠ
                {0x0218, 0x021B}                    // Extended-B: ȘșȚț      // Romanian +sup: ÂÎ    +ext-a: Ă
                // German  +sup: ÄÖẞÜ            // Swedish          +sup: ÅÄÖ
                // Italian +sup: ÀÈÉÍÎÓÙÚÌÒ      // Danish Norwegian +sup: ÆØÅ
                // Slovene        +ext-a: ČŠŽ    // Estonian +sup: ÄÕÖÜ +ext-a: ŠŽ
                // Serbo-Croatian +ext-a: ČĆÐŠŽ  // Finnish  +sup: ÄÅÖ  +ext-a: ŠŽ
            }
        };
        QGraphicsItemGroup *group =  new QGraphicsItemGroup();
        myscene->addItem(group);

        //for (int unicode_set = 0; unicode_set < 1/*unicode_limits.size()*/; unicode_set++) {
        //    for (auto item : group->childItems())
        //        myscene->removeItem(item);
        int unicode_set = qrand() % unicode_limits.size();
        //unicode_set++;

        quint32 limit = 0;
        QVector<quint32> limits;
        limits.reserve(unicode_limits[unicode_set].size() + 1);
        limits.append(limit);
        for (int i = 0; i < unicode_limits[unicode_set].size(); i++) {
            limit += unicode_limits[unicode_set][i][1] - unicode_limits[unicode_set][i][0] + 1;
            limits.append(limit);
        }
        qDebug() << "unicode symbols ="  << limit;


        {
            // every item, FullHD 63 ms.
            for (int column_pos_x = basic_width; column_pos_x < desktop.width();) {
                int char_max = desktop.height() / basic_height + 1;
                int char_count = 1.0 / 4.0 * char_max + 3.0 / 4.0 * (qrand() % char_max + 1);
                for (int char_index = 0; char_index < char_count; char_index++) {
                    QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
                    group->addToGroup(item);
                    item->setBrush(QColor(0, 128, 0));
                    item->setPen(Qt::NoPen);
                    item->setFont(font_background);

                    quint32 unicode = qrand() % limits.last();
                    for (int i = 1; i < limits.size(); i++)
                        if (limits[i - 1] <= unicode && unicode < limits[i])
                            unicode = unicode - limits[i - 1] + unicode_limits[unicode_set][i - 1][0];

                    if (QChar::requiresSurrogates(unicode)) {
                        QChar charArray[2];
                        charArray[0] = QChar::highSurrogate(unicode);
                        charArray[1] = QChar::lowSurrogate(unicode);
                        item->setText(QString(charArray, 2));
                    }
                    else
                        item->setText(QChar(unicode));

                    item->setPos(column_pos_x - item->boundingRect().width() / 2, char_index * basic_height);
                }
                column_pos_x += basic_width * 3;
            }
        }
//          SaveSceneToFile("c:/Users/User/Pictures/Screenshots/EyesThanks");
//        }

        {

//            // for test
//            for (auto item: myscene->items())
//                myscene->removeItem(item);
//            QString text;
            //QFontDatabase::addApplicationFont("c:/Users/User/Repositories/eyes-thanks/repository/fonts/Constructium.ttf");

//            QVector<QVector<QVector<qint64>>> unicode_limits = {
//                {{0x0250, 0x02AF}},/* 0 IPA                  */
//                {{0x16A0, 0x16EF}},/* 1 Runic                */
//                {{0x2C00, 0x2C2E}},/* 2 Glagolitic uppercase */
//                {{0x3B1, 0x3C9},{0x3D9, 0x3D9},{0x3DB, 0x3DB},{0x3DD, 0x3DD},{0x3DF, 0x3DF},{0x3D1, 0x3D1}}, // greek
//                {{0x262D, 0x262E},{0x263D, 0x2653},{0x26B3, 0x26B8},{0x26E7, 0x26E7}},        // symbols
//                {{0x2160, 0x216B},{0x216C, 0x216F},{0x2180, 0x2182},{0x2185, 0x2186}},        // roman num
//                {{0xF8D0,0xF8E8}, {0xF8F1, 0xF8F9}}, /* 11 Klingon */
//                {{0x20A0, 0x20BF}}, /* Currency Symbols */
//                {{0x0400, 0x0481},{0x048A, 0x04ff}}, //   Cyrillic             = 82+75
//                {{0x0500, 0x052F}},                  //   Cyrillic Supplement  = 2F
//                {{0xA640, 0xA66E},{0xA680, 0xA69B}},  //   Cyrillic Extended-B  = 2E+1B
//                {{0x0041, 0x005A},{0x0061, 0x007A}}, // A-Za-z
//                {{0x00C0, 0x00D6},{0x00D8, 0x00F6},{0x00F8, 0x00FF}},                   //   Latin-1 Supplement
//                {{0x0100, 0x017F}},                                                     //   Latin Extended-A
//                {{0x0180, 0x01BF},{0x01C4, 0x01D4},{0x01E2, 0x0229},{0x0232, 0x024F}},   //   Latin Extended-B
//                };
//            int count = 0;

//            for (int unicode_set=0; unicode_set< unicode_limits.size(); unicode_set++) {
//                int count_fortxt = 0;

//                text+= QString::number(unicode_set) + ",    0x" +
//                        QString::number(unicode_limits[unicode_set][0][0],16).toUpper()+ "  ";
//                for (int t = 0; t < unicode_limits[unicode_set].size(); t++) {
//                for (quint32 unicode = unicode_limits[unicode_set][t][0]; unicode <= unicode_limits[unicode_set][t][1]; unicode++) {
//                    qDebug() << unicode_set << t << QString::number(unicode,16).toUpper();

//                    QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
//                    myscene->addItem(item);
//                    item->setBrush(QColor(0, 128, 0));
//                    item->setPen(Qt::NoPen);
//                    item->setFont(font_background);
//                    item->setPos(basic_width + (count%(16*3) * basic_width*2) ,
//                                 count/(16*3) * (line_height) + line_height);
//                    if (count_fortxt %(16*4) == 0) text += "\n";

//                    if(QChar::requiresSurrogates(unicode))
//                    {
//                        QChar charArray[2];
//                        charArray[0] = QChar::highSurrogate(unicode);
//                        charArray[1] = QChar::lowSurrogate(unicode);
//                        item->setText(QString(charArray, 2));
//                        text += QString(charArray, 2);
//                    }
//                    else {
//                        item->setText(QChar(unicode));
//                        text += QChar(unicode);
//                    }
//                    count++;
//                    count_fortxt++;
//                }
//                }
//                count =  (count-1) / (16*3) * (16*3) + (16*3);
//                myscene->addLine(0,count/(16*3) * (line_height) + line_height+1,
//                   desktop.width(),count/(16*3) * (line_height) + line_height+1,QPen(Qt::darkGray));
//                text+= "\n";
//            }
//            LogToFile("unicode.txt",text);
        }
        {
            // columnItem, FullHD 55ms.
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
        }

        {
            // textItem    18ms
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
        }
        myscene->setBackgroundBrush(Qt::black);
        break;
    }
    default: {
    }
    }

    return Method;

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
    QString methodName = QString(QMetaEnum::fromType<Methods>().valueToKey(Method));

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
        SetBackground(Hue_start + ratio);
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
