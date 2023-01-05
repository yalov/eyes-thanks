#include <QCoreApplication>

#include <stdio.h>
#include <windows.h>
#include <lmaccess.h>
#include <lm.h>
#include <shellapi.h>
#include <tchar.h>

#include <lmaccess.h>
#include <windows.h>
#include <stdio.h>
#include <assert.h>

#pragma comment(lib,"user32")
#pragma comment(lib,"kernel32")


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString name;

    if (name.isEmpty()){  // Trying to get Windows 10 Username
        TCHAR  infoBuf[32767];
        DWORD  bufCharCount = 32767;

        GetUserName( infoBuf, &bufCharCount );
        LPUSER_INFO_0 pBuf = nullptr;
        NetUserGetInfo(nullptr,infoBuf,2, reinterpret_cast<LPBYTE*>(&pBuf));   // infoBuf - username
        if (pBuf != nullptr)
        {
            LPUSER_INFO_2 pBuf2 = LPUSER_INFO_2(pBuf);
            QStringList full_name = QString::fromWCharArray( pBuf2->usri2_full_name ).split(" ");
            // take the largest from user names: James T.K. -> James
            std::sort(full_name.begin(), full_name.end(),[](QString& a, QString& b) { return a.size() < b.size(); } );
            name = full_name.last();
            qDebug() << "full_name" << full_name;
        }
    }

    // trying to get at least username
    if (name.isEmpty()) name = qgetenv("USER");
    if (name.isEmpty()) name = qgetenv("USERNAME");
    qDebug() << "name" << name;

    if (name.isEmpty() || name == "User" || name.size() > 50)
        name = QString("Neo");

    qDebug() << "name" << name;

    return a.exec();
}
