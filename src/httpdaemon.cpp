#include <QFile>
#include <QDataStream>
#include <QTcpSocket>
#include <QDebug>

#include "httpdaemon.h"

HttpDaemon::HttpDaemon(quint16 port, QObject* parent)
    : QTcpServer(parent), disabled(false)
{
    qDebug() << "Start";
    contentTypes["default"] = "application/octet-stream";
    //image
    contentTypes["png"] = "image/png";
    contentTypes["jpeg"] = "image/jpeg";
    contentTypes["jpg"] = "image/jpeg";
    contentTypes["pjpeg"] = "image/pjpeg";
    contentTypes["svg"] = "image/svg+xml";
    contentTypes["tiff"] = "image/tiff";
    contentTypes["ico"] = "image/vnd.microsoft.icon";
    contentTypes["wbmp"] = "image/vnd.wap.wbmp";
    //text
    contentTypes["css"] = "text/css";
    contentTypes["csv"] = "text/csv";
    contentTypes["html"] = "text/html";
    contentTypes["htm"] = "text/html";
    contentTypes["js"] = "text/javascript";
    contentTypes["php"] = "text/php";
    contentTypes["xml"] = "text/xml";
    //video
    contentTypes["mpeg"] = "video/mpeg";
    contentTypes["mp4"] = "video/mp4";
    contentTypes["ogg"] = "video/ogg";
    contentTypes["mov"] = "video/quicktime";
    contentTypes["wmv"] = "video/x-ms-wmv";
    contentTypes["flv"] = "video/x-flv";
    //application
    contentTypes["atom"] = "application/atom+xml";
    contentTypes["json"] = "application/json";
    contentTypes["ogg"] = "application/ogg";
    contentTypes["pdf"] = "application/pdf";
    contentTypes["xhtml"] = "application/xhtml+xml";
    contentTypes["zip"] = "application/zip";
    contentTypes["gzip"] = "application/x-gzip";
    contentTypes["torrent"] = "application/x-bittorrent";
    contentTypes["pdf"] = "application/pdf";
    contentTypes["pdf"] = "application/pdf";
    listen(QHostAddress::Any, port);
    QObject::connect(this,SIGNAL(newConnection()),this,SLOT(incomingConnection()));
    qDebug() << isListening();
}

void HttpDaemon::incomingConnection()
{
    qDebug() << "in connection";
    if (disabled)
        return;

    // When a new client connects, the server constructs a QTcpSocket and all
    // communication with the client is done over this QTcpSocket. QTcpSocket
    // works asynchronously, this means that all the communication is done
    // in the two slots readClient() and discardClient().
    QTcpSocket* s = nextPendingConnection();
    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
}

void HttpDaemon::procIndexReq(QDataStream &os, QString request)
{
    Q_UNUSED(request)
    QString header;
    QString procIndexPage;
    procIndexPage = indexPage;
    header += "HTTP/1.0 200 Ok\r\n";
    header += "Content-Type: text/html;\r\n";
    header += "Content-Length: " + QString::number(procIndexPage.size()) + ";\r\n\r\n";

    QMap<QString,QString>::iterator i = sharedFiles.begin();
    for (; i != sharedFiles.end(); i++)
    {
        procIndexPage += "<a href=\"/f/" + i.key() + "\">" + i.key() + "</a><br>";
        qDebug() << i.key();
    }
    qDebug() << procIndexPage;
    os.writeRawData(header.toUtf8().data(),header.size());
    os.writeRawData(procIndexPage.toUtf8().data(),procIndexPage.size());
}

void HttpDaemon::procFileReq(QDataStream &os, QString request, QString fileName)
{
    if (sharedFiles.find(fileName) == sharedFiles.end())
        return;
    QFile file(sharedFiles[fileName]);
    if (!file.open(QIODevice::ReadOnly))
        return;
    Q_UNUSED(request)
    QString header;
    QString suffix = QFileInfo(fileName).completeSuffix();
    QString contentType;
    if (contentTypes.find(suffix) != contentTypes.end())
        contentType = contentTypes[suffix];
    else
        contentType = contentTypes["default"];
    qDebug() << "Content type: " << contentType;
    header += "HTTP/1.0 200 Ok\r\n";
    header += "Content-Type: " + contentType + ";\r\n";
    header += "Content-Length: " + QString::number(file.size()) + ";\r\n\r\n";
    os.writeRawData(header.toUtf8().data(),header.size());
    QByteArray tmp;
    while (!file.atEnd())
    {
        tmp = file.read(1000);
        os.writeRawData(tmp.data(),tmp.size());
    }
    file.close();
}

void HttpDaemon::readClient()
{
    if (disabled)
        return;
    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple HTML
    // document back.
    QTcpSocket* socket = (QTcpSocket*)sender();
    if (socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens[0] == "GET") {
            QDataStream os(socket);

            QString path = tokens[1];

            if (path[1] == 'i')
                procIndexReq(os,tokens[1]);
            else
            {
                if (path[1] == 'f')
                {
                    QString fileName = QFileInfo(path).fileName();
                    qDebug() << fileName;
                    procFileReq(os, tokens[1],fileName);
                }
            }

            socket->close();

            qDebug() << "Wrote to client";

            if (socket->state() == QTcpSocket::UnconnectedState) {
                delete socket;
                qDebug() << "Connection closed";
            }
        }
    }
}

void HttpDaemon::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();

    qDebug() << "Connection closed";
}
