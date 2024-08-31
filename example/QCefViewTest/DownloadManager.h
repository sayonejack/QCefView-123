#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H
#pragma once

#include <QMap>
#include <QSharedPointer>
#include <QDebug>

#include <QCefDownloadItem.h>

class DownloadManager
{
  public:
    static DownloadManager& getInstance();

    void AddNewDownloadItem(const QSharedPointer<QCefDownloadItem>& item);

    void UpdateDownloadItem(const QSharedPointer<QCefDownloadItem>& item);

    QMap<QString, QString> downImage2clipboard;
    void setDownImage2clipboard(const QString& url, const QString& path = "");

  private:
    DownloadManager();
    ~DownloadManager();

    QMap<qint32, QSharedPointer<QCefDownloadItem>> m_mapDownloadingItem;
};

#endif
