#ifndef HTTPDEAMON_H
#define HTTPDEAMON_H

#include <QObject>
#include <QTcpServer>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QFileInfo>

// HttpDaemon is the the class that implements the simple HTTP server.
class HttpDaemon : public QTcpServer
{
    Q_OBJECT
public:
    HttpDaemon(quint16 port, QObject* parent = 0);

    void pause(){ disabled = true; }
    void resume(){ disabled = false; }

    void setIndexPage(QString text){ indexPage = text;}
    void setErrorPage(QString text){ errorPage = text;}
    void shareFile(QString file){ sharedFiles[QFileInfo(file).fileName()] = file; }
    void cleanFiles();

private slots:
    void incomingConnection();
    void readClient();
    void discardClient();

private:
    bool disabled;
    QString indexPage;
    QString errorPage;
    QMap<QString,QString> sharedFiles;
    QMap<QString,QString> contentTypes;

    void procIndexReq(QDataStream &os, QString request);
    void procFileReq(QDataStream &os, QString request, QString fileName);
};

#endif // HTTPDEAMON_H
