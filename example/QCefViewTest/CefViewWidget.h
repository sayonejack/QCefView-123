#ifndef CUSTOMCEFVIEW_H
#define CUSTOMCEFVIEW_H

#include <QScreen>

#include <QCefView.h>

#include <QElapsedTimer>
#include <QThread>
#include <QEventLoop>

#include "Filters/AppendResponseFilter.h"
#include "Filters/MemoryStreamResponseFilter.h"

/// <summary>
/// Represents the customized QCefView
/// </summary>
class CefViewWidget : public QCefView
{
    Q_OBJECT

  public:
    CefViewWidget(const QString url, const QCefSetting* setting, QWidget* parent = 0);

    ~CefViewWidget();

    enum CefMenuCommand
    {
        NotFound = -1,
        Back = 100,
        Forward = 101,
        Reload = 102,
        ReloadNoCache = 103,
        StopLoad = 104,
        Undo = 110,
        Redo = 111,
        Cut = 112,
        Copy = 113,
        Paste = 114,
        Delete = 115,
        SelectAll = 116,
        Find = 130,
        Print = 131,
        ViewSource = 132,
        SpellCheckSuggestion0 = 200,
        SpellCheckSuggestion1 = 201,
        SpellCheckSuggestion2 = 202,
        SpellCheckSuggestion3 = 203,
        SpellCheckSuggestion4 = 204,
        SpellCheckLastSuggestion = 204,
        SpellCheckNoSuggestions = 205,
        AddToDictionary = 206,
        //
        // 摘要:
        //     Custom menu items originating from the renderer process This is the first entry
        CustomFirst = 220,
        //
        // 摘要:
        //     Custom menu items originating from the renderer process This is the last entry
        CustomLast = 250,
        UserFirst = 26500,
        UserLast = 28500,

        ShowDevTools = 26501,
        LinkUrlCommand = 26503,
        newwindowlink = 26504,
        SelectionTextbaidu = 26505,
        SelectionTextgoogle = 26506,
        SelectionText = 26507,
        Save2PDFCommand = 26508,
        // private const int LoadIpCommand = 26509;
        // private const int GoHomeCommand = 26510;
        // private const int AddBookMarkCommand = 26511;
        CopyImageLinkCommand = 26512,
        SaveImageCommand = 26513,
        CopyImageCommand = 26514,
        SaveLink = 26515,

    };

    QString homeUrl;
    QList<QString> listResponseFilterWords;
    QMap<QString, QString> listResponseFilterAppend;
    QMap<QString, Filters::MemoryStreamResponseFilter*> listResponse;

    Q_INVOKABLE QString EvaluateJavascript(const QString& jsCode, const int& timeout = 5000);

    QList<QPair<QString, QString>> getResponses();

  protected slots:
    void onScreenChanged(QScreen* screen);

    void onNativeBrowserWindowCreated(QWindow* window);

    void onDraggableRegionChanged(const QRegion& draggableRegion, const QRegion& nonDraggableRegion);

    void onJavascriptResult(int browserId,
#if CEF_VERSION_MAJOR > 119
                            QString frameId,
#else
                            qint64 frameId,
#endif
                            const QString& context,
                            const QVariant& result);

  protected:
    bool onNewPopup(
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
      bool& disableJavascriptAccess) override;

    void onNewDownloadItem(const QSharedPointer<QCefDownloadItem>& item, const QString& suggestedName) override;

    void onUpdateDownloadItem(const QSharedPointer<QCefDownloadItem>& item) override;

    QList<QString> onFileDialog() override;

    bool onContextMenuCommand(int command_id, const QPoint& pos, const QString& LinkUrl, const QString& imgUrl) override;

    void onBeforeContextMenu(QList<QPair<QString, int>>& data,
                             const QString& LinkUrl,
                             const QString& linkText,
                             const QString& imgUrl) override;

    void onCefContextMenuDismissed() override;

    Filters::IResponseFilter* getResourceResponseFilter(const QString& url) override;

    int onBeforeResourceLoad(QString& url,
                             QString& postDataStr,
                             QString& referer,
                             const QString& method,
                             const int& ResourceType) override;

    void onResourceLoadComplete(const QString& url,
                                const QString& method,
                                const int& statusCode,
                                const QString& statusText,
                                const int& status,
                                const int& received_content_length) override;

  protected:
    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

  private:
    void updateMask();

  private:
    QWindow* m_pCefWindow = nullptr;

    int m_iCornerRadius = 50;

    QRegion m_draggableRegion;

    QRegion m_nonDraggableRegion;

    QMap<QString, QString> jsResults;
    QEventLoop eventLoop;

  signals:

    void OnJSDialog(bool& result);

    void OnGetResourceResponseFilter(QString& url, int& filter, QString& appendData);

    void OnResourceLoadComplete(const QString& url,
                                const QString& method,
                                const int& statusCode,
                                const QString& statusText,
                                const int& status,
                                const int& received_content_length);

    void BeforeResourceLoad(QString& url,
                            QString& postDataStr,
                            QString& referer,
                            const QString& method,
                            const int& ResourceType,
                            int& result);

    void OnFileDialog(QStringList& file);
};

#endif // CUSTOMCEFVIEW_H
