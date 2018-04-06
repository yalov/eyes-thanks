//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "updater.h"

#include <QRegularExpression>
#include <QCoreApplication>
#include <QGridLayout>

UpdateAction::UpdateAction(const QIcon &icon, const QString &text, QObject *parent) :
    QAction(icon, text, parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(showmessagebox()));
    connect(this, SIGNAL(triggered()), this, SLOT(update()));
}

void UpdateAction::showmessagebox()
{
    mbx = new QMessageBox(QMessageBox::Icon::NoIcon, APP_NAME, tr("Checking update..."),
                                      QMessageBox::StandardButton::Cancel);
    QFontMetrics fm(mbx->font());
    int height =  fm.boundingRect("A").height() * 6;
    mbx->setStyleSheet(QString("QLabel{min-height: %1px;}").arg(height));

    QSpacerItem *horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = qobject_cast<QGridLayout *>(mbx->layout());
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    mbx->show();
    QCoreApplication::processEvents();
}

void UpdateAction::update()
{
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(replyFinished(QNetworkReply *)));

    manager->get(QNetworkRequest(QUrl("https://api.github.com/repos/yalov/eyes-thanks/releases")));
}

void UpdateAction::replyFinished(QNetworkReply *reply)
{
    mbx->setStandardButtons(QMessageBox::Ok);

    if (reply->error() != QNetworkReply::NoError) {
        mbx->setIcon(QMessageBox::Critical);
        mbx->setText(tr("There was an error connecting to <a href='%1'>%1</a>.").arg(REPO_URL));
    }
    else {
        QVector<int> newVersion = {0, 0, 0};
        QVector<int> currVersion = {0, 0, 0};

        QRegularExpression re1("(\\d+)\\.(\\d+)\\.(\\d+)");
        QRegularExpressionMatch match1 = re1.match(QString(APP_VERSION));
        for (int i = 0; i < match1.lastCapturedIndex(); ++i) // 0..2
            currVersion[i] = match1.captured(i+1).toInt();

        QRegularExpression re2("tag_name.+?(\\d+)\\.(\\d+)\\.(\\d+)");
        QRegularExpressionMatch match2 = re2.match(QString(reply->readAll()));
        for (int i = 0; i < match2.lastCapturedIndex(); ++i) // 0..2
            newVersion[i] = match2.captured(i+1).toInt();

        QString NewVersionString  = QString("%1.%2.%3").arg(newVersion[0]).arg(newVersion[1]).arg(newVersion[2]);
        QString NewVersionUrl =
            QString("https://github.com/yalov/eyes-thanks/releases/download/%1/EyesThanks_v%1.7z").arg(NewVersionString);

        QString text;
        if (currVersion == newVersion)
            text = tr("You are already running the most recent version <b>Eyes' Thanks %1</b>.").arg(NewVersionString);
        else if (currVersion < newVersion)
            text = tr("A new version of <b>Eyes' Thanks</b> has been released! "
                      "Version <b>%1</b> is available at <a href=%2>%2</a>.<br><br>"
                      "You can download this version using the link:<br>"
                      "<a href=%3>%3</a>").arg(NewVersionString).arg(REPO_URL).arg(NewVersionUrl);
        else
            text = tr("Fantastic! You have <b>Eyes' Thanks %1</b>, "
                      "but last available version is <b>%2</b>.<br><br>"
                      "Please, upload new version to <a href=%3>%3</a>.").arg(APP_VERSION).arg(NewVersionString).arg(REPO_URL);

        mbx->setText(text);
    }
    reply->deleteLater();
}
