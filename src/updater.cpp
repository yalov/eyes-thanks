//----------------------------------------------------------------------------------//
//      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  //
//      This file is part of Eyes' Thanks.                                          //
//      GNU General Public License 3                                                //
//----------------------------------------------------------------------------------//

#include "updater.h"

#include "QDebug"
#include <QElapsedTimer>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QSpacerItem>
#include <QGridLayout>



UpdateAction::UpdateAction(const QIcon &icon, const QString &text, QObject *parent) :
    QAction(icon, text, parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(showmessagebox()));
    connect(this, SIGNAL(triggered()), this, SLOT(update()));


}

void UpdateAction::showmessagebox()
{
    downloadProcess = new QMessageBox(QMessageBox::Icon::NoIcon, qApp->translate("App", "Eyes' Thanks"), tr("Checking update..."), QMessageBox::StandardButton::Cancel);

    QSpacerItem *horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = (QGridLayout *)downloadProcess->layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    downloadProcess->show();
    QCoreApplication::processEvents();


}

void UpdateAction::update()
{
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(replyFinished(QNetworkReply *)));

    manager->get(QNetworkRequest(QUrl("https://raw.githubusercontent.com/yalov/eyes-thanks/master/eyes-thanks.pro")));

}



void UpdateAction::replyFinished(QNetworkReply *reply)
{
    QList<int> newVersion = {0, 0, 0};
    QList<int> currVersion = {0, 0, 0};

    QRegularExpression re("(\\d+)\\.(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = re.match(QString(APP_VERSION));

    if (match.hasMatch()) {
        currVersion[0] = match.captured(1).toInt();
        currVersion[1] = match.captured(2).toInt();
        currVersion[2] = match.captured(3).toInt();
    }


    if (reply->error() != QNetworkReply::NoError) {
        // qDebug() << reply->errorString();

        downloadProcess->setText(tr("There was an error connecting to <a href='%1'>%1</a>.")
                                 .arg(REPOSITORY_PATH));

        downloadProcess->setIcon(QMessageBox::Critical);
        downloadProcess->setStandardButtons(QMessageBox::Ok);

    } else {

        QString ProFileText = QString(reply->readAll());
        QRegularExpression re("VERSION.+(\\d+)\\.(\\d+)\\.(\\d+)");
        QRegularExpressionMatch match = re.match(ProFileText);

        if (match.hasMatch()) {
            newVersion[0] = match.captured(1).toInt();
            newVersion[1] = match.captured(2).toInt();
            newVersion[2] = match.captured(3).toInt();
        }


        downloadProcess->setIcon(QMessageBox::Information);
        downloadProcess->setStandardButtons(QMessageBox::Ok);

        if (currVersion == newVersion) {
            downloadProcess->setText(tr("You are already running the most recent version of <b>Eyes' Thanks</b>."));

        } else if ((currVersion[0] < newVersion[0]) ||
                   (currVersion[0] == newVersion[0] && currVersion[1] < newVersion[1]) ||
                   (currVersion[0] == newVersion[0] && currVersion[1] == newVersion[1] && currVersion[2] < newVersion[2])) {
            QString NewVersionPath = QString("https://github.com/yalov/eyes-thanks/releases/download/%1/EyesThanks_v%1.7z")
                                     .arg(QString::number(newVersion[0]) + "." + QString::number(newVersion[1]) + "." + QString::number(newVersion[2]));

            downloadProcess->setText(
                tr("A new version of <b>Eyes' Thanks</b> has been released! Version <b>%1</b> is available at <a href=%2>%2</a>.<br><br>"
                   "You can download this version using the link:<br>"
                   "<a href=%3>%3</a>")
                .arg(QString::number(newVersion[0]) + "." + QString::number(newVersion[1]) + "." + QString::number(newVersion[2]))
                .arg(REPOSITORY_PATH)
                .arg(NewVersionPath));
        }


        else {
            downloadProcess->setText(
                tr("Fantastic! You are have <b>Eyes' Thanks %1</b>, but last available version is <b>%2</b>.<br><br>"
                   "Please, upload new version to <a href=%3>%3</a>.")
                .arg(QString::number(currVersion[0]) + "." + QString::number(currVersion[1]) + "." + QString::number(currVersion[2]))
                .arg(QString::number(newVersion[0]) + "." + QString::number(newVersion[1]) + "." + QString::number(newVersion[2]))
                .arg(REPOSITORY_PATH)
            );
        }

    }

    reply->deleteLater();
}
