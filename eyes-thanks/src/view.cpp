//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "view.h"


#include "charactersets.h"
#include "transliteration-iso9a.h"
#include <QGraphicsBlurEffect>
#include <lm.h>
//#include <shellapi.h>
//#include <lmerr.h>
//#include <lmapibuf.h>


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QDesktopWidget>
#endif

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

View::View(QWidget *parent): QGraphicsView(parent)

{
    default_screen = QGuiApplication::primaryScreen()->geometry();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    desktop = QApplication::desktop()->geometry();
#else
    QRegion virtualGeometry;
    for (auto screen : QGuiApplication::screens())
    {
      virtualGeometry += screen->geometry();
    }
    desktop = virtualGeometry.boundingRect();
#endif
    //LogToFile("LoggingTest.txt", str_from(desktop));

    setWindowFlags(Qt::WindowStaysOnTopHint);

    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setAttribute(Qt::WA_DeleteOnClose);
    setRenderHint(QPainter::Antialiasing);
    //setRenderHints(QPainter::SmoothPixmapTransform);

    setFocusPolicy(Qt::StrongFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
    setStyleSheet("QGraphicsView { border-style: none; }");

    myscene = new QGraphicsScene();
    myscene->setBackgroundBrush(Qt::black);
    setScene(myscene);

    ProgressBarItem = new ViewItem(true,16);
    myscene->addItem(ProgressBarItem);


    ButtonItem = new ViewItem(false, 14);
    ButtonItem->setText(tr("Close"));
    myscene->addItem(ButtonItem);
}


struct Image{
    QPixmap pic;
    qreal ratio {};
    QString path;
    qsizetype count {};       // Count of image in the same folder;
};

void View::ShowRefreshment(const QString &clock, const QString &text, const  Setting &_setting, Timer *viewtimer)
{
    setting = _setting;

    // Skip break when running fullscreen apps
    if (setting.isSkipWhenFullScreen && CheckIsForegroundFullScreen())
        return;

    QElapsedTimer timer; timer.start();
    myscene->setSceneRect(desktop);
    const qreal ratio_desk = desktop.ratio();
    const qreal ratio_default_screen = default_screen.ratio();

    // ProgressBar and Button
    {
        const qreal REL_PBAR_WIDTH = 3/5.0;
        const qreal REL_PBAR_POS = 1/5.0;
        const int BUTTON_WIDTH = 100;

        ProgressBarItem->setText(viewtimer->remains_str());
        ProgressBarItem->setSize(default_screen.width() *REL_PBAR_WIDTH);
        ProgressBarItem->setPos(default_screen.left() + default_screen.width()*REL_PBAR_POS,
                                default_screen.height() - ProgressBarItem->rect().height() - 25);

        ButtonItem->setSize(BUTTON_WIDTH, ProgressBarItem->rect().height());
        ButtonItem->setPos(ProgressBarItem->sceneTransform().map(ProgressBarItem->rect().topRight()) + QPointF(25,0));
    }

    // ClockItem and TextItem
    if (setting.isClock || setting.isText) {
        const int FONT_SIZE = 30;
        const QFont font(setting.isPrettyFont ? "UKIJ Diwani Yantu Mod" : "FreeSerif Mod", FONT_SIZE, QFont::Bold);
        const QColor fill_color(Qt::white);
        const QColor outline_color(Qt::black);
        const qreal opacity = 0.5;
        const int TOP_MARGIN = 25;
        const int SIDE_MARGIN = 50;

        if (setting.isClock) {
            clockItem = new QGraphicsSimpleTextItem();
            clockItem->setText(clock);
            clockItem->setFont(font);
            clockItem->setPos(default_screen.topRight() + QPointF(-SIDE_MARGIN - clockItem->boundingRect().width(), TOP_MARGIN));
            clockItem->setZValue(3);
            clockItem->setBrush(fill_color);
            clockItem->setPen(outline_color);
            clockItem->setOpacity(opacity);
            myscene->addItem(clockItem);
        }

        if (setting.isText || MethodIndex == NEO) {
            textItem = new QGraphicsSimpleTextItem();
            textItem->setText(text);
            textItem->setFont(font);
            textItem->setPos(default_screen.topLeft() + QPointF(SIDE_MARGIN, TOP_MARGIN));
            textItem->setZValue(3);
            textItem->setBrush(fill_color);
            textItem->setPen(outline_color);
            textItem->setOpacity(opacity);
            myscene->addItem(textItem);
        }
    }


    // Picture and Background
    QList<Image> images;
    Image image;

    QString display_case;
    int variants = 0;

    const QList<QString> folders({setting.imagesPath, setting.imagesPathAlternative});


    for (const auto& folder : folders)
    {
        QStringList images_names = QDir(folder).entryList(QStringList() << "*.jpg" << "*.png");
        if (!images_names.isEmpty()) {
            QString rand_pic_path = folder + QDir::separator() + images_names[Random(images_names.size())];
            QPixmap pic = QPixmap(rand_pic_path);
            if (!pic.isNull()) {
                images.append(Image{pic, qreal(pic.width()) / pic.height(),
                                    rand_pic_path, images_names.count()});
            }
        }
    }

    if (!images.empty()) {
        // find out which one of pic apropriate, if "inside" - comparison to def screen, if outside or auto - comparison to desk
        qreal ratio = (setting.imageAspectMode == ImageAspectMode::Inside) ? ratio_default_screen : ratio_desk;
        qreal min_value = std::numeric_limits<qreal>::max();
        int min_index = 0;

        for (int i = 0; i < images.size(); i++) {
            if (qFabs(images[i].ratio - ratio) < min_value) {
                min_value = qFabs(images[i].ratio - ratio);
                min_index = i;
            }
        }

        image = images[min_index];
        variants += image.count;
    }


    if (setting.isSpectrum) MethodsEnabled.append(SPECTRUM);
    if (setting.isTiling  ) MethodsEnabled.append(TILING);
    if (setting.isStripes ) MethodsEnabled.append(STRIPES);
    if (setting.isCircle  ) MethodsEnabled.append(RANDOM_CIRCLE);
    if (setting.isCircles ) MethodsEnabled.append(RANDOM_CIRCLES);
    if (setting.isNeo     ) MethodsEnabled.append(NEO);

    variants += MethodsEnabled.size();

    if (variants) {
        int variant = Random(variants);
        qDebug() << QString("variant: %1th of %2").arg(variant).arg(variants);
        if (variant < image.count)
            display_case = SetImageBackground(image.ratio, image.pic);
        else{
            MethodIndex = MethodsEnabled.at(variant - image.count);
            SetPredeterminedBackground();
        }
    }
    else
    {
        myscene->setBackgroundBrush(Qt::black);
    }


    // Logging
    if (setting.isLogging) {
        QString Logging_str = "";

        if (MethodIndex != -1) {
            QMetaEnum metaEnum = QMetaEnum::fromType<Methods>();
            display_case = metaEnum.valueToKey(MethodIndex);
        }

        Logging_str += QString("%1 Display case = %2, %3 ms., items = %4\n")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(display_case).arg(timer.elapsed()).arg(myscene->items().size());

        for (int i = 0; i < images.size(); i++) {
            Logging_str += QString(" -   %4  %5    %1  %2 %3\n")
                    .arg(images[i].ratio, 4, 'f', 2).arg(str_from(images[i].pic.size())).arg(images[i].path)
                    .arg(image.path == images[i].path?"image showed ":"image        ")
                    .arg(display_case.split(' ')[0] == "Full_desktop"
                          ?images[i].ratio/desktop.ratio()
                          :images[i].ratio/default_screen.ratio(),4,'f',2);

        }


        Logging_str +=     QString(" -   %3          %1  %2\n")
                .arg(ratio_desk, 4, 'f', 2).arg(str_from(desktop.size()))
                .arg(default_screen == desktop?"single screen":"full desktop ");

        if (QGuiApplication::screens().count() > 1)
            for (int i = 0; i < QGuiApplication::screens().count(); i++) {
                QRect scr_i = QGuiApplication::screens()[i]->geometry();
                Logging_str += QString(" - - %4 screen #%1      %2  %3\n").arg(i)
                        .arg(qreal(scr_i.width()) / scr_i.height(), 4, 'f', 2)
                        .arg(str_from(scr_i))
                        .arg(QGuiApplication::primaryScreen() == QGuiApplication::screens()[i]?"default":"       ");
            }



        LogToFile("LoggingDisplay.txt", Logging_str);
    }


#ifndef DEPLOY
    // SaveSceneToFile
    //SaveSceneToFile("c:/Users/User/Pictures/Screenshots/EyesThanks");
    //qDebug().noquote() << "ShowRefreshment saving by" << timer.restart() << "ms.";
#endif

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

QString View::SetImageBackground(qreal ratio_pic, QPixmap pic)
{

    QString display_case = "No image";
    const qreal ratio_desk = desktop.ratio();
    const qreal ratio_default_screen = default_screen.ratio();

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
    return display_case;
}

void View::SetPredeterminedBackground()
{
    // HSV
    // S - from uncolor to color
    // V - from black to color or white
    // 0 0 - black   // 1 0 - black
    // 0 0.5 grey    // 1 1 - color
    // 0 1 - white

    myscene->setBackgroundBrush(Qt::black);

    qreal Hue_start = Random(1.0);

    QElapsedTimer timer;
    timer.start();


    if (MethodIndex == RANDOM_CIRCLE)
        IsBackgroundUpdate = true;

    switch (MethodIndex) {
    case SPECTRUM: {
        QLinearGradient rainbow(desktop.topLeft(), desktop.topRight());

        for (int hue_int = 0; hue_int <= 25; hue_int ++) {
            qreal hue = hue_int/25.0;
            QColor c = QColor::fromHsvF(hue*0.8, 1, 1);
            rainbow.setColorAt(hue, c);
        }
        myscene->setBackgroundBrush(QBrush(rainbow));

        QLinearGradient vertical(desktop.topLeft(), desktop.bottomLeft());
        const qreal more_opaque = 0.4;
        const qreal less_opaque = 0.6;
        const qreal transparent  = 1.0;

        int mode = Random(3);
        if (mode == 0) {
            vertical.setColorAt(0.35,  QColor::fromRgbF(0, 0, 0, transparent));
            vertical.setColorAt(0.5,  QColor::fromRgbF(0, 0, 0, more_opaque));
            vertical.setColorAt(0.65,  QColor::fromRgbF(0, 0, 0, transparent));

        }
        else if (mode == 1) {

            vertical.setColorAt(0.275, QColor::fromRgbF(0, 0, 0, transparent));
            vertical.setColorAt(0.375, QColor::fromRgbF(0, 0, 0, more_opaque));
            vertical.setColorAt(0.475, QColor::fromRgbF(0, 0, 0, transparent));

            vertical.setColorAt(0.525, QColor::fromRgbF(0, 0, 0, transparent));
            vertical.setColorAt(0.625, QColor::fromRgbF(0, 0, 0, more_opaque));
            vertical.setColorAt(0.725, QColor::fromRgbF(0, 0, 0, transparent));

        }
        else if (mode == 2) {
            vertical.setColorAt(0.25, QColor::fromRgbF(0, 0, 0, transparent));
            vertical.setColorAt(0.30, QColor::fromRgbF(0, 0, 0, less_opaque));
            vertical.setColorAt(0.35, QColor::fromRgbF(0, 0, 0, transparent));

            vertical.setColorAt(0.4,  QColor::fromRgbF(0, 0, 0, transparent));
            vertical.setColorAt(0.5,  QColor::fromRgbF(0, 0, 0, more_opaque));
            vertical.setColorAt(0.6,  QColor::fromRgbF(0, 0, 0, transparent));

            vertical.setColorAt(0.65, QColor::fromRgbF(0, 0, 0, transparent));
            vertical.setColorAt(0.70, QColor::fromRgbF(0, 0, 0, less_opaque));
            vertical.setColorAt(0.75, QColor::fromRgbF(0, 0, 0, transparent));
        }
        QGraphicsRectItem *item = new QGraphicsRectItem(desktop);
        item->setPen(Qt::NoPen);
        item->setBrush(vertical);
        myscene->addItem(item);

        break;
    }
    case TILING: {
        switch (Random(5)) {
        case 0: { // square
            int h = Random(20)+9;
            int w = qRound(h * desktop.ratio());
            qreal cellw = desktop.widthF() / w;
            qreal cellh = desktop.heightF() / h;

            for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                    QGraphicsRectItem *squareItem = new QGraphicsRectItem();
                    squareItem->setPen(QPen(Qt::black, 1));
                    squareItem->setPos(desktop.topLeft() + QPointF(i * cellw, j * cellh));
                    squareItem->setRect(0, 0, cellw, cellh);
                    qreal hue = Hue_start + qreal(i) * 0.8 / w ;
                    hue -= floor(hue);
                    qreal value = Random(0.5) + 0.25;
                    squareItem->setBrush(QColor::fromHsvF(hue, 1.0, value));
                    myscene->addItem(squareItem);
                }
            }
            break;
        }
        case 1: { // square transform
            int h =  Random(20)+7;
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
            int h = Random(30)+7;
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
            int h = 2 * (Random(10) + 3); // even
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
            int h = 2 * Random(10) + 5; // odd
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

        for (int hue_int = 0; hue_int <= 25; hue_int ++) {
            qreal hue = hue_int/25.0;
            QColor c = QColor::fromHsvF(fmod(Hue_start + hue * 0.8, 1), 1, 0.9);
            rainbow.setColorAt(hue, c);
        }

        myscene->setBackgroundBrush(QBrush(rainbow));

        qreal ratio = desktop.ratio();
        qreal standard_ratio = 16.0 / 9.0;
        qreal k = ratio / standard_ratio;
        int stripes_count = 10;

        int stripe_segment = desktop.width() / int(stripes_count * k);
        int stripe_width = int(stripe_segment * 0.8);

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

        break;
    }
    case RANDOM_CIRCLE: {
        if (Item == nullptr) {
            Item = new ViewRandomPolygon(desktop);
            myscene->addItem(Item);
        }
        Item->setRandomPos(desktop);
        Item->setRamdomColor();
        break;

    }
    case RANDOM_CIRCLES: {
        int count_item = qRound(desktop.height() * desktop.width() / (1920.0 * 1080.0) * 100);
        for (int i = 0; i < count_item; i++) {
            int diameter_dot = Random(100) + 50;
            QRect r(desktop.topLeft() +
                    QPoint(Random(desktop.width() - diameter_dot), Random(desktop.height() - diameter_dot)),
                    QSize(diameter_dot, diameter_dot));

            QGraphicsEllipseItem *item = new QGraphicsEllipseItem(r);
            item->setBrush(QColor::fromHsvF(fmod(Hue_start + qreal(i) / count_item / 2, 1), 1, 1));
            item->setPen(Qt::NoPen);
            item->setOpacity(qreal(i) / count_item / 2);
            myscene->addItem(item);
        }

        break;
    }
    case NEO: {

        // chance to show a username
        if (Random(5) == 0) {
            QString name;

            // Trying to get "Windows 10 Username"
            // or, presicely, name in the Computer Management->Local Users and Groups->Users->Properties
            if (name.isEmpty()) {
                WCHAR userName[UNLEN+1];
                DWORD userNameCount = UNLEN+1;
                LPUSER_INFO_2 pBuf = nullptr;

                GetUserNameW( userName, &userNameCount );
                name = QString::fromWCharArray(userName); // local username

                if (NetUserGetInfo(nullptr, userName, 2, reinterpret_cast<LPBYTE*>(&pBuf)) == NERR_Success && pBuf != nullptr){
                    QStringList full_name = QString::fromWCharArray( pBuf->usri2_full_name ).split(" ");
                    // take the longest from user names: James T.K. -> James
                    if (!full_name.isEmpty())
                        name = *std::max_element(full_name.begin(), full_name.end(),[](QString& a, QString& b) { return a.size() < b.size(); } );

                    NetApiBufferFree(pBuf);
                }
            }

            // in the case was some problems
            // with getting full name
            if (name.isEmpty()) name = qEnvironmentVariable("USER");
            if (name.isEmpty()) name = qEnvironmentVariable("USERNAME");

            name = transliteraction(name);

            QString title = QString("Wake up, %1...").arg(name);

            if (textItem)
            {
                textItem->setBrush(QColor(0, 128, 0));
                textItem->setPen(Qt::NoPen);
                textItem->setFont(QFont("FreeSerif Mod", textItem->font().pointSize()));
                textItem->setOpacity(1);
                textItem->setText(title);
                CreateBlurredBackgroundForItem(textItem, myscene);
            }
        } else {
            if (textItem)    myscene->removeItem(textItem);
        }

        if (clockItem) {
            clockItem->setBrush(QColor(0, 128, 0));
            clockItem->setPen(Qt::NoPen);
            clockItem->setFont(QFont("FreeSerif Mod", clockItem->font().pointSize()));
            clockItem->setOpacity(1);
            CreateBlurredBackgroundForItem(clockItem, myscene);
        }


        QFont font_background("FreeSerif Mod", 17);
        QFontMetrics fm(font_background);
        int basic_height =  fm.boundingRect("А").height();
        int basic_width =  fm.boundingRect("А").width();

        QGraphicsItemGroup *group =  new QGraphicsItemGroup();
        myscene->addItem(group);

        CharacterSets characterSets(":res/character_sets.xml");
        //              -------
        characterSets.ChooseRandomCurrentSet();
        //              -------
        auto characters = characterSets.currentCharacters;
        auto title = characterSets.currentTitle;

        //qDebug() << "unicode symbols ="  << characters.size();
        if (!title.isEmpty())
            for (int index = 0; index < title.size(); index++) {
                QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
                group->addToGroup(item);
                item->setBrush(Qt::darkGreen);
                item->setPen(Qt::NoPen);
                item->setFont(font_background);
                item->setText(QString(title.at(index)));
                item->setPos(default_screen.topLeft() + QPointF(basic_width - item->boundingRect().width() / 2, index * basic_height));
            }

        for (int pos_x = default_screen.left()+ basic_width * (title.isEmpty() ? 1 : 4); pos_x < desktop.right(); pos_x += basic_width * 3) {
            int max_pos_y = int(1.0 / 5.0 * desktop.height() + 3.5 / 5.0 * Random(desktop.height()));
            int min_pos_y = int(max_pos_y - desktop.height()/2.0);  // could be < 0
            for (int pos_y = qMax(0,min_pos_y); pos_y < max_pos_y; pos_y += basic_height) {
                qreal rel_pos =  qreal(pos_y-min_pos_y)/(max_pos_y-min_pos_y);
                QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
                group->addToGroup(item);
                if (abs(pos_y - max_pos_y) <= basic_height)
                    item->setBrush(QColor(128,255,128));
                else
                    item->setBrush(QColor(0,rel_pos*255,0));
                item->setPen(Qt::NoPen);
                item->setFont(font_background);

                int charIndex = Random(characters.size());
                QString character = characters.at(charIndex);

                item->setText(character);
                item->setPos(pos_x - item->boundingRect().width() / 2, pos_y);
            }

        }

        for (int pos_x = default_screen.left() -2 * basic_width; pos_x >  desktop.left(); pos_x -= basic_width * 3) {
            int max_pos_y = int(1.0 / 5.0 * desktop.height() + 3.5 / 5.0 * Random(desktop.height()));
            int min_pos_y = int(max_pos_y - desktop.height()/2.0);  // could be < 0
            for (int pos_y = qMax(0,min_pos_y); pos_y < max_pos_y; pos_y += basic_height) {
                qreal rel_pos =  qreal(pos_y-min_pos_y)/(max_pos_y-min_pos_y);
                QGraphicsSimpleTextItem *item = new  QGraphicsSimpleTextItem();
                group->addToGroup(item);
                if (abs(pos_y - max_pos_y) <= basic_height)
                    item->setBrush(QColor(128,255,128));
                else
                    item->setBrush(QColor(0,rel_pos*255,0));
                item->setPen(Qt::NoPen);
                item->setFont(font_background);

                int charIndex = Random(characters.size());
                QString character = characters.at(charIndex);

                item->setText(character);
                item->setPos(pos_x - item->boundingRect().width() / 2, pos_y);
            }

        }

        myscene->setBackgroundBrush(Qt::black);
        break;
    }
    default:{ // MethodIndex == -1 && methodsEnabled.size() == 0
        myscene->setBackgroundBrush(Qt::black);
    }
    }

    return;
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
                           .arg(methodName, QDateTime::currentDateTime().toString("yyyyMMdd-HHmmsszzz")));
    qDebug() << "SaveSceneToFile is" << done << "by" << timer.elapsed() << "ms.";
}

bool View::CheckIsForegroundFullScreen()
{
    QUERY_USER_NOTIFICATION_STATE pquns;
    HRESULT err = SHQueryUserNotificationState(&pquns);
    if (!err) {
        switch (pquns) {
        case QUNS_BUSY:
            if (setting.isLogging) {
                QString Logging_str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +
                        " FullScreen QUNS_BUSY activity is detected. The break is aborted.";
                LogToFile("LoggingDisplay.txt", Logging_str);
            }
            return true;
        case QUNS_RUNNING_D3D_FULL_SCREEN:
            if (setting.isLogging) {
                QString Logging_str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +
                        " FullScreen QUNS_RUNNING_D3D_FULL_SCREEN activity is detected. The break is aborted.";
                LogToFile("LoggingDisplay.txt", Logging_str);
            }
            return true;
        case QUNS_PRESENTATION_MODE: {
            if (setting.isLogging) {
                QString Logging_str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +
                        " FullScreen QUNS_PRESENTATION_MODE activity is detected. The break is aborted.";
                LogToFile("LoggingDisplay.txt", Logging_str);
            }
            return true;
        }
        default:
            // QUNS_NOT_PRESENT,
            // QUNS_ACCEPTS_NOTIFICATIONS,
            // QUNS_QUIET_TIME,
            // QUNS_APP
            return false;
        }
    }
    return false;
}

void View::UpdateValues(const QString &remains_str, const qreal &ratio)
{
    ProgressBarItem->setText(remains_str);
    ProgressBarItem->setProgress(ratio);

    if (clockItem != nullptr)
        clockItem->setText(QTime::currentTime().toString("hh:mm"));

    if (picture_path.isEmpty() && IsBackgroundUpdate)
    {

        SetPredeterminedBackground();
    }

}

void View::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        QFile pic(picture_path);

        if (pic.exists() && pic.remove()) {
            for (auto item : myscene->items())
                if (qFuzzyCompare(item->zValue(), -2)) {
                    myscene->removeItem(item);
                    delete item;
                }
            SetPredeterminedBackground();
        }
    }
    else if (event->key() == Qt::Key_Escape)
        close();
}

void View::mousePressEvent(QMouseEvent *event)
{
    for (auto i : items(event->pos())) {
        if (ButtonItem == i)
            ButtonItem->setOpacity(1);
    }
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    for (auto i : items(event->pos()))
        if (ButtonItem == i)
            close();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    bool IsButton = false;
    for (auto i : items(event->pos()))
        if (ButtonItem == i)
            IsButton = true;

    if (IsButton) {
        if (event->buttons() == Qt::NoButton)
            ButtonItem->setOpacity(0.8);
        else
            ButtonItem->setOpacity(1);
    }
    else
        ButtonItem->setOpacity(0.4);
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
