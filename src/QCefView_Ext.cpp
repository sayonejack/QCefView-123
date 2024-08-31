#include <QCefView.h>

#pragma region qt_headers
#include <QPainter>
#include <QPoint>
#include <QResizeEvent>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QtDebug>
#pragma endregion qt_headers

#include <QCefContext.h>

#include "details/QCefEventPrivate.h"
#include "details/QCefViewPrivate.h"
#include "details/utils/CommonUtils.h"

void
QCefView::showDevTools(const QPoint& point)
{
    Q_D(QCefView);

    d->showDevTools(point);
}

void
QCefView::startDownload(const QString& url)
{
    Q_D(QCefView);

    d->startDownload(url);
}

void
QCefView::downloadImage(const QString& url)
{
    Q_D(QCefView);

    d->downloadImage(url);
}

bool
QCefView::setCookie(const QString& url,
                    const QString& name,
                    const QString& value,
                    const QString& domain,
                    const QString& path,
                    const bool& httpOnly,
                    const bool& secure,
                    const QDateTime& expirationDate)
{
    Q_D(QCefView);

    return d->setCookie(url, name, value, domain, path, httpOnly, secure, expirationDate);
}

QString
QCefView::exportCookies(const QString& url)
{
    Q_D(QCefView);

    auto temp = url;
    if (temp.isEmpty()) {
        temp = d->getMainUrl();
    }

    return d->exportCookies(temp);
}

QString
QCefView::getMainUrl()
{
    if (d_ptr) {
        return d_ptr->getMainUrl();
    }
    return QString();
}

bool
QCefView::LoadRequest(const QString& url,
                      const QString& postDataStr,
                      const QString& referer,
                      const QString& contentType,
                      const QString& origin)
{
    if (d_ptr) {
        return d_ptr->LoadRequest(url, postDataStr, referer, contentType, origin);
    }
    return false;
}

QString
QCefView::getHtml()
{
    if (d_ptr) {
        return d_ptr->getHtml();
    }

    return "";
}

void
QCefView::SendMouseClickEvent(int x, int y, Qt::MouseButton button, bool mouseUp, int clickCount)
{
    if (d_ptr) {
        d_ptr->SendMouseClickEvent(x, y, button, mouseUp, clickCount);
    }
}

void
QCefView::SendKeyEvent(int winkey, bool iskeydown, int modifiers, const QString& text)
{
    if (d_ptr) {
        d_ptr->SendKeyEvent(winkey, iskeydown, modifiers, text);
    }
}

void
QCefView::SendMouseMoveEvent(int x, int y, bool mouseLeave)
{
    if (d_ptr) {
        d_ptr->SendMouseMoveEvent(x, y, mouseLeave);
    }
}

#pragma region protected

Filters::IResponseFilter*
QCefView::getResourceResponseFilter(const QString& url)
{

    return nullptr;
}

int
QCefView::onBeforeResourceLoad(QString& url,
                               QString& postDataStr,
                               QString& referer,
                               const QString& method,
                               const int& ResourceType)
{
    return -1;
}

bool
QCefView::onContextMenuCommand(int command_id, const QPoint& pos, const QString& LinkUrl, const QString& imgUrl)
{
    return false;
}

void
QCefView::onBeforeContextMenu(QList<QPair<QString, int>>& data,
                              const QString& LinkUrl,
                              const QString& linkText,
                              const QString& imgUrl)
{
}

void
QCefView::onCefContextMenuDismissed()
{
}

void
QCefView::onResourceLoadComplete(const QString& url,
                                 const QString& method,
                                 const int& statusCode,
                                 const QString& statusText,
                                 const int& status,
                                 const int& received_content_length)
{
}

QList<QString>
QCefView::onFileDialog()
{
    return QList<QString>();
}

// bool
// QCefView::onBeforeUnloadDialog()
// {
//     return false;
// }

bool
QCefView::onJSDialog()
{
    return false;
}

#pragma endregion protected
