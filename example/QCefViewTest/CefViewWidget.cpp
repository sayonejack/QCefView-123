#include "CefViewWidget.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#elif defined(Q_OS_MAC)
#elif defined(Q_OS_LINUX)
#else
#endif

#include <QColor>
#include <QDebug>
#include <QPainterPath>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QWindow>
#include <QThread>
#include <QElapsedTimer>
#include <QApplication>
#include <QClipboard>
#include <QScreen>
#include <QMouseEvent>

#include "DownloadManager.h"

CefViewWidget::CefViewWidget(const QString url, const QCefSetting* setting, QWidget* parent /* = 0*/)
  : QCefView(url, setting, parent)
{
    setStyleSheet("background: blue;");

    connect(this, &CefViewWidget::draggableRegionChanged, this, &CefViewWidget::onDraggableRegionChanged);
    connect(this, &CefViewWidget::nativeBrowserCreated, this, &CefViewWidget::onNativeBrowserWindowCreated);

    connect(this, &QCefView::reportJavascriptResult, this, &CefViewWidget::onJavascriptResult);
}

CefViewWidget::~CefViewWidget() {}

void
CefViewWidget::onScreenChanged(QScreen* screen)
{
    if (!m_pCefWindow)
        return;

    updateMask();
}

void
CefViewWidget::onNativeBrowserWindowCreated(QWindow* window)
{
    m_pCefWindow = window;
    if (!m_pCefWindow)
        return;

    connect(this->window()->windowHandle(), SIGNAL(screenChanged(QScreen*)), this, SLOT(onScreenChanged(QScreen*)));

    updateMask();
}

void
CefViewWidget::onDraggableRegionChanged(const QRegion& draggableRegion, const QRegion& nonDraggableRegion)
{
    m_draggableRegion = draggableRegion;
    m_nonDraggableRegion = nonDraggableRegion;
}

QString
CefViewWidget::EvaluateJavascript(const QString& jsCode, const int& timeout)
{
    // 当前已在线程中
    QString contextName = "getJsContext" + QString::number(qHash(jsCode));
    QString result("");

    // 计算用时
    QElapsedTimer timer;
    timer.start();

    // 执行 JavaScript
    bool success = this->executeJavascriptWithResult(this->MainFrameID, jsCode, QString(), contextName);
    if (success) {
        while (true) {
            {
                if (jsResults.contains(contextName)) {
                    result = jsResults.value(contextName, "");
                    jsResults.remove(contextName);
                    break;
                }
            }

            if (timer.elapsed() > timeout) {
                break;
            }

            QThread::msleep(1);
        }
    }

    qDebug() << "success:" << success << "Result:" << result << "timeout:" << timeout << "time:" << timer.elapsed();

    return result;
}

QList<QPair<QString, QString>>
CefViewWidget::getResponses()
{
    QList<QPair<QString, QString>> lists;

    try {

        for (auto it = listResponse.constBegin(); it != listResponse.constEnd(); ++it) {
            auto url = it.key();
            auto item = listResponse.value(url);
            auto data = QString::fromStdString(item->ToString());
            lists.append({ url, data });
        }

    } catch (const std::exception& ex) {
        qDebug() << "getResponses:" << ex.what();
    }

    return lists;
}

void
CefViewWidget::onJavascriptResult(int browserId,
#if CEF_VERSION_MAJOR > 119
                                  QString frameId,
#else
                                  qint64 frameId,
#endif
                                  const QString& context,
                                  const QVariant& result)
{
    Q_UNUSED(browserId);
    Q_UNUSED(frameId);

    qDebug() << "Frame ID:" << frameId << "context:" << context << "Result:" << result << "isValid:" << result.isValid();

    if (context.startsWith("getJsContext")) {
        if (result.isValid()) {
            jsResults[context] = result.toString();
        }
    }

    if (eventLoop.isRunning()) {
        eventLoop.quit();
    }
}

bool
CefViewWidget::onNewPopup(
#if CEF_VERSION_MAJOR > 119
  QString sourceFrameId,
#else
  qint64 sourceFrameId,
#endif

  const QString& targetUrl,
  QString& targetFrameName,
  QCefView::CefWindowOpenDisposition targetDisposition,
  QRect& rect,
  QCefSetting& settings,
  bool& disableJavascriptAccess)
{
    // create new QCefView as popup browser
    settings.setBackgroundColor(Qt::red);
    return false;
}

void
CefViewWidget::onNewDownloadItem(const QSharedPointer<QCefDownloadItem>& item, const QString& suggestedName)
{
    // keep the item into list or map, and call item->start() to allow the download

    DownloadManager::getInstance().AddNewDownloadItem(item);
}

void
CefViewWidget::onUpdateDownloadItem(const QSharedPointer<QCefDownloadItem>& item)
{

    // control the download by invoking item->pause(), item->resume(), item->cancel()

    DownloadManager::getInstance().UpdateDownloadItem(item);
}

QList<QString>
CefViewWidget::onFileDialog()
{
    QStringList files;
    emit OnFileDialog(files);

    // qDebug() << "CefViewWidget onFileDialog:" << files;

    return files;
}

void
CefViewWidget::resizeEvent(QResizeEvent* event)
{
    // update mask first, because the new mask will be
    // used in the QCefView::resizeEvent
    updateMask();

    QCefView::resizeEvent(event);
}

void
CefViewWidget::mousePressEvent(QMouseEvent* event)
{
    QCefView::mousePressEvent(event);

#if defined(Q_OS_WIN)
    if (event->buttons().testFlag(Qt::LeftButton) && m_draggableRegion.contains(event->pos())) {
        HWND hWnd = ::GetAncestor((HWND)(window()->windowHandle()->winId()), GA_ROOT);
        POINT pt;
        ::GetCursorPos(&pt);
        ::ReleaseCapture();
        ::SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS(pt));
    }
#elif defined(Q_OS_MAC)
#elif defined(Q_OS_LINUX)
#else
#endif
}

void
CefViewWidget::updateMask()
{
    // create a rect with rounded corner (50px radius) as mask
    QPainterPath path;
    // path.addRoundedRect(rect(), 50, 50);
    QRegion mask = QRegion(path.toFillPolygon().toPolygon());

    // apply the mask
    setMask(mask);
}

bool
CefViewWidget::onContextMenuCommand(int command_id, const QPoint& pos, const QString& LinkUrl, const QString& imgUrl)
{
    qDebug() << "onContextMenuCommand: " << command_id;

    switch (command_id) {
        case 100:
            if (this->browserCanGoBack()) {
                this->browserGoBack();
            }

            return true;
        case 101:
            if (this->browserCanGoForward()) {
                this->browserGoForward();
            }

            return true;
        case CefMenuCommand::ShowDevTools: {

            this->showDevTools(pos);

            return true;
        }
        case CefMenuCommand::LinkUrlCommand:

            QApplication::clipboard()->setText(LinkUrl);

            return true;
        case CefMenuCommand::newwindowlink:

            if (!LinkUrl.isEmpty())
                this->navigateToUrl(LinkUrl);

            return true;
        case CefMenuCommand::SaveLink: {
            if (!LinkUrl.isEmpty())
                this->startDownload(LinkUrl);

            return true;
        }
        case CefMenuCommand::SelectionText: {

            // 获取鼠标点击位置的元素
            int x = pos.x();
            int y = pos.y();

            QString jsCode = QString("return (function() { "
                                     "const element = document.elementFromPoint(%1, %2); "
                                     "if (element && element.tagName.toLowerCase() === 'a') { "
                                     "    return element.textContent || element.innerText; "
                                     "} "
                                     "return null; "
                                     "})();")
                               .arg(x)
                               .arg(y);

            // 新建线程中调用 EvaluateJavascript
            QThread* thread = QThread::create([=]() {
                QString result = EvaluateJavascript(jsCode);

                // 在主线程中调用
                QMetaObject::invokeMethod(
                  this, [&]() { QApplication::clipboard()->setText(result); }, Qt::BlockingQueuedConnection);
            });
            thread->start();
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);

            return true;
        }
        case CefMenuCommand::CopyImageLinkCommand:

            if (!imgUrl.isEmpty())
                QApplication::clipboard()->setText(imgUrl);

            return true;
        case CefMenuCommand::SaveImageCommand: {
            if (!imgUrl.isEmpty())
                this->startDownload(imgUrl);

            return true;
        }
        case CefMenuCommand::CopyImageCommand: {

            ////this->downloadImage(imgUrl);

            QString url(imgUrl);
            if (!url.isEmpty()) {
                DownloadManager::getInstance().setDownImage2clipboard(url);
                this->startDownload(url);
            }

            return true;
        }

        default:
            break;
    }

    return false;
}

void
CefViewWidget::onBeforeContextMenu(QList<QPair<QString, int>>& data,
                                   const QString& LinkUrl,
                                   const QString& linkText,
                                   const QString& imgUrl)
{

    QList<QPair<QString, int>> dataNew;

    if (!LinkUrl.isEmpty()) {
        dataNew.append(QPair<QString, int>("&打开链接", CefMenuCommand::newwindowlink));
        dataNew.append(QPair<QString, int>("&复制链接", CefMenuCommand::LinkUrlCommand));

        if (imgUrl.isEmpty()) {
            dataNew.append(QPair<QString, int>("&复制链接文字 ", CefMenuCommand::SelectionText));
        }

        dataNew.append(QPair<QString, int>("&链接另存为", CefMenuCommand::SaveLink));
        dataNew.append(QPair<QString, int>("l1", -1));
    }

    if (!imgUrl.isEmpty()) {
        dataNew.append(QPair<QString, int>("&复制图片链接", CefMenuCommand::CopyImageLinkCommand));
        dataNew.append(QPair<QString, int>("&图片另存为", CefMenuCommand::SaveImageCommand));
        dataNew.append(QPair<QString, int>("&复制图片", CefMenuCommand::CopyImageCommand));
        dataNew.append(QPair<QString, int>("l2", -1));
    }

    dataNew.append(QPair<QString, int>("&后退", CefMenuCommand::Back));
    dataNew.append(QPair<QString, int>("&前进", CefMenuCommand::Forward));
    dataNew.append(QPair<QString, int>("&刷新", CefMenuCommand::Reload));
    // dataNew.append(QPair<QString, int>("&打印", 131));
    dataNew.append(QPair<QString, int>("&网页源码", CefMenuCommand::ViewSource));
    dataNew.append(QPair<QString, int>("&检查元素", CefMenuCommand::ShowDevTools));

    QList<QPair<QString, int>> dataEdit;
    // for data 倒序遍历
    for (auto& item : data) {
        if (item.second == CefMenuCommand::Copy) {
            item.first = "&复制";
            dataEdit.append(item);
        } else if (item.second == CefMenuCommand::Cut) {
            item.first = "&剪切";
            dataEdit.append(item);
        } else if (item.second == CefMenuCommand::Paste) {
            item.first = "&粘贴";
            dataEdit.append(item);
        } else if (item.second == CefMenuCommand::Undo) {
            item.first = "&撤销";
            dataEdit.append(item);
        } else if (item.second == CefMenuCommand::Redo) {
            item.first = "&重做";
            dataEdit.append(item);
        } else if (item.second == CefMenuCommand::SelectAll) {
            item.first = "&全选";
            dataEdit.append(item);
        }
    }

    data.clear();
    if (dataEdit.size() > 0) {
        dataEdit.append(QPair<QString, int>("l3", -1));
        data.append(dataEdit);
    }

    data.append(dataNew);
}

void
CefViewWidget::onCefContextMenuDismissed()
{
}

Filters::IResponseFilter*
CefViewWidget::getResourceResponseFilter(const QString& url)
{

    for (auto it = listResponseFilterAppend.constBegin(); it != listResponseFilterAppend.constEnd(); ++it) {
        const QString& word = it.key();
        if (url.contains(word, Qt::CaseInsensitive)) {

            qDebug() << "getResourceResponseFilterAppend:" << word << "url:" << url;

            auto data = listResponseFilterAppend.value(word);
            return new Filters::AppendResponseFilter(data.toStdString());
        }
    }

    // listResponseFilterWords
    for (const auto& word : qAsConst(listResponseFilterWords)) {
        if (url.contains(word, Qt::CaseInsensitive)) {

            auto* filter = new Filters::MemoryStreamResponseFilter();
            listResponse.insert(url, filter);

            qDebug() << "getResourceResponseFilterWords:" << word << "url:" << url << "listResponse:" << listResponse.count();

            return filter;
        }
    }

    {
        int filter = 0;
        QString resData;
        QString url2 = url;

        emit OnGetResourceResponseFilter(url2, filter, resData);

        // qDebug() << "url:" << url << "url2:" << url2 << "filter:" << filter;

        if (filter == 1) {
            auto* filter = new Filters::MemoryStreamResponseFilter();
            listResponse.insert(url, filter);
            return filter;
        } else if (filter == 2) {
            return new Filters::AppendResponseFilter(resData.toStdString());
        }
    }

    return nullptr;
}

int
CefViewWidget::onBeforeResourceLoad(QString& url,
                                    QString& postDataStr,
                                    QString& referer,
                                    const QString& method,
                                    const int& ResourceType)
{

    // RT_MAIN_FRAME
    if (ResourceType == 0) {
        // for (auto filter : listResponse) {
        for (auto it = listResponse.constBegin(); it != listResponse.constEnd(); ++it) {
            auto filter = it.value();
            delete filter;
        }
        listResponse.clear();
    }

    int result = -1;
    emit BeforeResourceLoad(url, postDataStr, referer, method, ResourceType, result);

    return result;
}

void
CefViewWidget::onResourceLoadComplete(const QString& url,
                                      const QString& method,
                                      const int& statusCode,
                                      const QString& statusText,
                                      const int& status,
                                      const int& received_content_length)
{

    emit OnResourceLoadComplete(url, method, statusCode, statusText, status, received_content_length);
}
