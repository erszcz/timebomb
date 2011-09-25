#include <QtGui>

#include "trayicon.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(timebomb);

    QApplication app(argc, argv);

    if (! QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0, QObject::tr("Hourglass"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    TrayIcon ti;

    return app.exec();
}
