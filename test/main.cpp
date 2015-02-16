#include "mainwindow.h"
#include <QApplication>
#include <httpdaemon.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    quint16 port = 8080;
    HttpDaemon * daemon = new HttpDaemon(port);
    daemon->setIndexPage("<h3>Quick cat files:</h3>");
    daemon->shareFile("file.png");
    daemon->shareFile("moc_httpdaemon.cpp");
    return a.exec();
}
