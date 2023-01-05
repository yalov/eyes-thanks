#include <windows.h>
#include <lmaccess.h>
#include <lmapibuf.h>
#include <lmerr.h>
#include <QLabel>
#include <QApplication>
#include <winuser.h>

int main3(int argc, char *argv[])
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
        MessageBoxW(NULL, pBuf->usri2_full_name, NULL, 0x00000000L);

        NetApiBufferFree(pBuf);
    }

    qDebug() << "name: " << name << "-- " << userName;

    MessageBoxW(NULL, userName, NULL, 0x00000000L);

    QLabel * label = new QLabel("|" + name + "|");
    label->show();

    return a.exec();
}
