#include <windows.h>
#include <lmaccess.h>
#include <lmerr.h>
#include <lmapibuf.h>

#include <QLabel>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString name;
    WCHAR userName[UNLEN+1];
    DWORD userNameCount = UNLEN+1;
    LPUSER_INFO_2 pBuf = nullptr;

    GetUserNameW( userName, &userNameCount ); // local username
    if (NetUserGetInfo(nullptr, userName, 2, reinterpret_cast<LPBYTE*>(&pBuf)) == NERR_Success && pBuf != nullptr)
    {
        name = QString::fromWCharArray( pBuf->usri2_full_name );
        NetApiBufferFree(pBuf);
    }

    qDebug() << "name: " << name;
    QLabel * label = new QLabel("|" + name + "|");
    label->show();

    return a.exec();
}
