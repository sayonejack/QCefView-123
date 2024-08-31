#include "QCefViewPrivate.h"

#include <QEventLoop>
#include <QTimer>
#include <QDebug>

#include "Filters/MyCefCookieVisitor.h"
#include "Filters/MyCefStringVisitor.h"
#include "Filters/MyDownloadImageCallback.h"

// #include "include/cef_task.h"
#include "include/wrapper/cef_closure_task.h"

QString
QCefViewPrivate::getMainUrl()
{

    if (pCefBrowser_) {
        CefRefPtr<CefFrame> frame = pCefBrowser_->GetMainFrame();
        if (frame != nullptr) {
            return QString::fromStdString(frame->GetURL().ToString());
        }
    }

    return QString();
}

bool
QCefViewPrivate::LoadRequest(const QString& url,
                             const QString& postDataStr,
                             const QString& referer,
                             const QString& contentType,
                             const QString& origin)
{
    if (pCefBrowser_) {

        CefRefPtr<CefFrame> frame = pCefBrowser_->GetMainFrame();
        if (frame != nullptr) {
            CefString method = "GET";
            CefRefPtr<CefRequest> request = CefRequest::Create();

            if (!postDataStr.isEmpty()) {
                method = "POST";

                CefRefPtr<CefPostData> postData = CefPostData::Create();
                CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
                // element->SetToBytes(postDataStr.toUtf8().constData(), postDataStr.length());
                element->SetToBytes(postDataStr.toStdString().size(), postDataStr.toStdString().c_str());

                postData->AddElement(element);
                request->SetPostData(postData);
            }

            request->SetMethod(method);
            request->SetURL(url.toStdString());

            if (!referer.isEmpty()) {
                request->SetReferrer(referer.toStdString(), REFERRER_POLICY_DEFAULT);
            }

            CefRequest::HeaderMap headerMap;
            request->GetHeaderMap(headerMap);

            if (!contentType.isEmpty()) {
                headerMap.insert(std::make_pair("Content-Type", contentType.toStdString()));
                request->SetHeaderMap(headerMap);
            }

            if (!origin.isEmpty()) {
                headerMap.insert(std::make_pair("Origin", origin.toStdString()));
                request->SetHeaderMap(headerMap);
            }

            frame->LoadRequest(request);
            return true;
        }
    }

    return false;
}

QString
QCefViewPrivate::getHtml()
{
    try {

        if (pCefBrowser_) {

            CefRefPtr<CefFrame> frame = pCefBrowser_->GetMainFrame();
            if (frame != nullptr) {

                int timeoutMs = 5000;

                QString result;
                QEventLoop loop;
                QTimer timer;

                // 设置超时
                timer.setSingleShot(true);
                connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

                // 在 CEF UI 线程上执行获取 HTML 的操作
                CefPostTask(TID_UI,
                            base::BindOnce(
                              [](CefRefPtr<CefBrowser> browser, QString* result, QEventLoop* loop) {
                                  CefRefPtr<CefFrame> frame = browser->GetMainFrame();
                                  if (frame) {
                                      frame->GetSource(new MyCefStringVisitor([result, loop](const QString& html) {
                                          *result = html;
                                          QMetaObject::invokeMethod(loop, "quit", Qt::QueuedConnection);
                                      }));
                                  } else {
                                      QMetaObject::invokeMethod(loop, "quit", Qt::QueuedConnection);
                                  }
                              },
                              pCefBrowser_,
                              &result,
                              &loop));

                // 启动计时器和事件循环
                timer.start(timeoutMs);
                loop.exec();

                return result;
            }
        }

    } catch (const std::exception& ex) {
        qDebug() << "getHtml:" << ex.what();
    }
    return QString();
}

void
QCefViewPrivate::SendMouseClickEvent(int x, int y, Qt::MouseButton button, bool mouseUp, int clickCount)
{
    try {
        if (pCefBrowser_) {

            CefMouseEvent mouse_event;
            mouse_event.x = x;
            mouse_event.y = y;
            mouse_event.modifiers = 0;
            CefBrowserHost::MouseButtonType type = MBT_LEFT;
            switch (button) {
                case Qt::RightButton:
                    type = MBT_RIGHT;
                    break;
                case Qt::MiddleButton:
                    type = MBT_MIDDLE;
                    break;
                default:
                    break;
            }
            pCefBrowser_->GetHost()->SendMouseClickEvent(mouse_event, type, mouseUp, clickCount);
        }

    } catch (const std::exception& ex) {
        qDebug() << "SendMouseClickEvent:" << ex.what();
    }
}

void
QCefViewPrivate::SendKeyEvent(int key, bool iskeydown, int modifiers, const QString& text)
{
    try {
        if (pCefBrowser_) {
            CefKeyEvent key_event;
            key_event.windows_key_code = key;
            key_event.native_key_code = key;
            key_event.modifiers = modifiers;
            key_event.type = iskeydown ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP;
            key_event.character = text.toStdString().c_str()[0];
            key_event.unmodified_character = text.toStdString().c_str()[0];

            pCefBrowser_->GetHost()->SendKeyEvent(key_event);

            // qDebug() << "SendKeyEvent:" << key_event.windows_key_code << key_event.native_key_code << key_event.modifiers
            //          << key_event.is_system_key << key_event.type;
        }

    } catch (const std::exception& ex) {
        qDebug() << "SendKeyEvent:" << ex.what();
    }
}

void
QCefViewPrivate::SendMouseMoveEvent(int x, int y, bool mouseLeave)
{
    try {
        if (pCefBrowser_) {
            CefMouseEvent mouse_event;
            mouse_event.x = x;
            mouse_event.y = y;
            mouse_event.modifiers = 0;
            pCefBrowser_->GetHost()->SendMouseMoveEvent(mouse_event, mouseLeave);
        }

    } catch (const std::exception& ex) {
        qDebug() << "SendMouseMoveEvent:" << ex.what();
    }
}

bool
QCefViewPrivate::setCookie(const QString& url,
                           const QString& name,
                           const QString& value,
                           const QString& domain,
                           const QString& path,
                           const bool& httpOnly,
                           const bool& secure,
                           const QDateTime& expirationDate)
{
    try {
        if (pCefBrowser_) {
            CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
            if (manager) {
                CefCookie cookie;
                CefString(&cookie.name).FromString(name.toStdString());
                CefString(&cookie.value).FromString(value.toStdString());
                CefString(&cookie.domain).FromString(domain.toStdString());
                CefString(&cookie.path).FromString(path.toStdString());
                cookie.httponly = httpOnly;
                cookie.secure = secure;

                // 将 QDateTime 转换为 CEF 时间格式
                // CEF 时间是从 1601-01-01 00:00:00 UTC 开始的微秒数
                qint64 microsecondsSinceEpoch = expirationDate.toMSecsSinceEpoch() * 1000;
                qint64 microsecondsSince1601 = microsecondsSinceEpoch + 11644473600000000LL;
                if (microsecondsSince1601 > 0) {
                    cookie.has_expires = true;
                    cookie.expires.val = microsecondsSince1601;
                }

                return manager->SetCookie(url.toStdString(), cookie, nullptr);
            }
        }

    } catch (const std::exception& ex) {
        qDebug() << "setCookie:" << ex.what();
    }
    return false;
}

QString
QCefViewPrivate::exportCookies(const QString& url)
{
    try {
        if (pCefBrowser_) {

            int timeoutMs = 5000;

            QString result;
            QEventLoop loop;
            QTimer timer;

            // 设置超时
            timer.setSingleShot(true);
            connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

            // 在 CEF UI 线程上执行获取 HTML 的操作
            CefPostTask(TID_UI,
                        base::BindOnce(
                          [](const CefString& cefUrl, QString* result, QEventLoop* loop) {
                              CefRefPtr<CefCookieManager> cookieManager = CefCookieManager::GetGlobalManager(nullptr);
                              if (cookieManager) {
                                  CefRefPtr<MyCefCookieVisitor> visitor =
                                    new MyCefCookieVisitor([result, loop](const QString& data) {
                                        *result = data;
                                        QMetaObject::invokeMethod(loop, "quit", Qt::QueuedConnection);
                                    });
                                  cookieManager->VisitUrlCookies(cefUrl, true, visitor);
                              } else {

                                  qDebug() << "cookieManager is null";

                                  QMetaObject::invokeMethod(loop, "quit", Qt::QueuedConnection);
                              }
                          },
                          url.toStdString(),
                          &result,
                          &loop));

            // 启动计时器和事件循环
            timer.start(timeoutMs);
            loop.exec();

            return result;
        }

    } catch (const std::exception& ex) {
        qDebug() << "exportCookies:" << ex.what();
    }
    return QString();
}

void
QCefViewPrivate::showDevTools(const QPoint& point)
{
    if (pCefBrowser_) {
        CefRefPtr<CefBrowserHost> host = pCefBrowser_->GetHost();
        if (host) {
            CefWindowInfo info;
            info.SetAsPopup(nullptr, "DevTools");

            CefBrowserSettings settings;
            CefPoint cpoint;
            if (!point.isNull()) {
                cpoint.x = point.x();
                cpoint.y = point.y();
            }

            host->ShowDevTools(info, host->GetClient(), settings, cpoint);
        }
    }
}

void
QCefViewPrivate::startDownload(const QString& url)
{
    if (pCefBrowser_) {
        CefRefPtr<CefBrowserHost> host = pCefBrowser_->GetHost();
        if (host) {
            host->StartDownload(url.toStdString());
        }
    }
}

void
QCefViewPrivate::downloadImage(const QString& url)
{
    Q_Q(QCefView);

    if (pCefBrowser_) {
        CefRefPtr<CefBrowserHost> host = pCefBrowser_->GetHost();
        if (host) {

            // MyDownloadImageHandler* handler = new MyDownloadImageHandler();
            //  创建并设置下载图片的回调
            CefRefPtr<MyDownloadImageCallback> callback = new MyDownloadImageCallback(q);

            host->DownloadImage(url.toStdString(), false, 1024 * 1024 * 10, true, callback);
        }
    }
}

bool
QCefViewPrivate::onContextMenuCommand(int command_id, const QPoint& pos, const QString& LinkUrl, const QString& imgUrl)
{
    Q_Q(QCefView);

    return q->onContextMenuCommand(command_id, pos, LinkUrl, imgUrl);
}

void
QCefViewPrivate::onBeforeCefContextMenu(const MenuBuilder::MenuData& data,
                                        const QString& LinkUrl,
                                        const QString& linkText,
                                        const QString& imgUrl)
{
    Q_Q(QCefView);

    // clear previous context menu
    if (osr.contextMenu_) {
        osr.contextMenu_->close();
        osr.contextMenu_->deleteLater();
        osr.contextMenu_ = nullptr;
    }

    // create context menu
    osr.contextMenu_ = new QMenu(q);
    osr.contextMenu_->setAttribute(Qt::WA_DeleteOnClose);
    // 选中项背景色
    osr.contextMenu_->setStyleSheet("QMenu::item:selected {background-color: #0078d7; color: white;}");
    osr.contextMenu_->setFocusPolicy(Qt::NoFocus);

    // connect context menu signals
    connect(osr.contextMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onContextMenuTriggered(QAction*)));
    connect(osr.contextMenu_, SIGNAL(destroyed(QObject*)), this, SLOT(onContextMenuDestroyed(QObject*)));

    QList<QPair<QString, int>> dlist;
    MenuBuilder::MenuData dataCopy;
    for (const auto& item : data) {
        if (item.commandId == 131) {
            continue;
        }
        dataCopy.push_back(item);
        dlist.append(QPair<QString, int>(item.label, item.commandId));
    }

    q->onBeforeContextMenu(dlist, LinkUrl, linkText, imgUrl);

    // dlist to dataCopy
    for (auto it : dlist) {

        MenuBuilder::MenuItem item;
        if (it.second == -1) {
            item.type = MenuBuilder::MenuItemType::kMeueItemTypeSeparator;
        } else {
            item.type = MenuBuilder::MenuItemType::kMeueItemTypeCommand;
        }

        item.label = it.first;
        item.commandId = it.second;
        item.enable = true;
        item.visible = true;

        for (size_t i = 0; i < dataCopy.size(); i++) {
            int commandId = dataCopy[i].commandId;
            if (commandId == -1) {
                continue;
            }
            if (commandId == it.second) {
                item.enable = dataCopy[i].enable;
                item.visible = dataCopy[i].visible;
                item.subMenuData = dataCopy[i].subMenuData;
                item.groupId = dataCopy[i].groupId;
                item.accelerator = dataCopy[i].accelerator;
                item.checked = dataCopy[i].checked;
                dataCopy.removeAt(i);
                break;
            }
        }

        dataCopy.push_back(item);
    }

    // build menu from data
    MenuBuilder::BuildQtMenuFromMenuData(osr.contextMenu_, dataCopy);
}

// bool
// QCefViewPrivate::onBeforeUnloadDialog()
// {
//     Q_Q(QCefView);

//     return q->onBeforeUnloadDialog();
// }

bool
QCefViewPrivate::onJSDialog()
{
    Q_Q(QCefView);

    bool result = q->onJSDialog();
    return result;
}

Filters::IResponseFilter*
QCefViewPrivate::getResourceResponseFilter(const QString& url)
{

    Q_Q(QCefView);

    return q->getResourceResponseFilter(url);
}

int
QCefViewPrivate::onBeforeResourceLoad(QString& url,
                                      QString& postDataStr,
                                      QString& referer,
                                      const QString& method,
                                      const int& ResourceType)
{

    Q_Q(QCefView);

    return q->onBeforeResourceLoad(url, postDataStr, referer, method, ResourceType);
}

void
QCefViewPrivate::onResourceLoadComplete(const QString& url,
                                        const QString& method,
                                        const int& statusCode,
                                        const QString& statusText,
                                        const int& status,
                                        const int& received_content_length)
{
    Q_Q(QCefView);

    q->onResourceLoadComplete(url, method, statusCode, statusText, status, received_content_length);
}
