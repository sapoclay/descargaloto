#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>

class DownloadManager : public QObject
{
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);
    ~DownloadManager();
    
    void startDownload(const QString &url, const QString &filePath);
    void cancelDownload();
    bool isDownloading() const;

signals:
    void downloadProgress(int percentage);
    void downloadFinished(const QString &filePath, bool success);
    void downloadError(const QString &error);

private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onDownloadError(QNetworkReply::NetworkError error);
    void onReadyRead();

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QFile *m_currentFile;
    QString m_currentFilePath;
    qint64 m_totalBytes;
    qint64 m_receivedBytes;
    bool m_isDownloading;
};

#endif // DOWNLOADMANAGER_H
