#include "MainWindow.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonValue>
#include <QMessageBox>
#include <QRandomGenerator>

#include <QCefContext.h>

#define URL_ROOT "http://QCefViewDoc"
#define INDEX_URL URL_ROOT "/index.html"
#define TUTORIAL_URL URL_ROOT "/tutorial.html"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent /*, Qt::FramelessWindowHint*/)
{
    m_ui.setupUi(this);

    // auto policy = m_ui.verticalSpacer->sizePolicy();
    // policy.setVerticalPolicy(QSizePolicy::Expanding);

#ifdef Q_OS_MACOS
    this->m_ui.nativeContainer->setContentsMargins(0, 28, 0, 0);
#endif

    setupWindow();

    // setWindowFlags(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground);

    connect(m_ui.btn_showDevTools, &QPushButton::clicked, this, &MainWindow::onBtnShowDevToolsClicked);
    connect(m_ui.btn_reloadRight, &QPushButton::clicked, this, &MainWindow::onBtnReloadRightViewClicked);
    connect(m_ui.btn_recreateRight, &QPushButton::clicked, this, &MainWindow::onBtnRecreateRightViewClicked);
    connect(m_ui.btn_changeColor, &QPushButton::clicked, this, &MainWindow::onBtnChangeColorClicked);
    connect(m_ui.btn_setFocus, &QPushButton::clicked, this, &MainWindow::onBtnSetFocusClicked);
    connect(m_ui.btn_callJSCode, &QPushButton::clicked, this, &MainWindow::onBtnCallJSCodeClicked);
    connect(m_ui.btn_newBrowser, &QPushButton::clicked, this, &MainWindow::onBtnNewBrowserClicked);
    connect(m_ui.btn_quitApp, &QPushButton::clicked, qApp, &QCoreApplication::quit);

    // build the path to the web resource
    QDir dir = QCoreApplication::applicationDirPath();
#if defined(Q_OS_MACOS)
    QString webResourceDir = /*QString("file://") +*/ QDir::toNativeSeparators(dir.filePath("../Resources/webres"));
#else
    QString webResourceDir = /*QString("file://") +*/ QDir::toNativeSeparators(dir.filePath("webres"));
#endif

    // add a local folder to URL map (global)
    QCefContext::instance()->addLocalFolderResource(webResourceDir, URL_ROOT);

    createLeftCefView();
    createRightCefView();
}

MainWindow::~MainWindow() {}

void
MainWindow::createLeftCefView()
{
    if (m_pLeftCefViewWidget) {
        m_pLeftCefViewWidget->deleteLater();
        m_pLeftCefViewWidget = nullptr;
    }

    m_pLeftCefViewWidget = new CefViewWidget(INDEX_URL, nullptr, this);
    // connect the invokeMethod to the slot
    connect(m_pLeftCefViewWidget, &QCefView::invokeMethod, this, &MainWindow::onInvokeMethod);

    // connect the cefQueryRequest to the slot
    connect(m_pLeftCefViewWidget, &QCefView::cefQueryRequest, this, &MainWindow::onQCefQueryRequest);

    // connect(m_pLeftCefViewWidget, &QCefView::reportJavascriptResult, this, &MainWindow::onJavascriptResult);

    connect(m_pLeftCefViewWidget, &QCefView::loadStart, this, &MainWindow::onLoadStart);
    connect(m_pLeftCefViewWidget, &QCefView::loadEnd, this, &MainWindow::onLoadEnd);
    connect(m_pLeftCefViewWidget, &QCefView::loadError, this, &MainWindow::onLoadError);

    connect(m_pLeftCefViewWidget, SIGNAL(OnFileDialog(QStringList&)), this, SLOT(OnFileDialog(QStringList&)));

    m_ui.leftCefViewContainer->layout()->addWidget(m_pLeftCefViewWidget);
}

void
MainWindow::OnFileDialog(QStringList& files)
{
    try {

        // 获取系统下载路径
        QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        // 获取三张图片
        QDir dir(path);
        QStringList filters;
        filters << "*.jpg" << "*.png";
        dir.setNameFilters(filters);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            files.append(list.at(i).absoluteFilePath());
            if (files.size() >= 3) {
                break;
            }
        }

        // path = path + "/1.jpg";
        // path = QDir::toNativeSeparators(path);
        // files.append(path);

    } catch (const std::exception& ex) {
        qDebug() << ex.what();
    }
}

void
MainWindow::createRightCefView()
{
    if (m_pRightCefViewWidget) {
        m_pRightCefViewWidget->deleteLater();
        m_pRightCefViewWidget = nullptr;
    }

    ///*
    // build settings for per QCefView
    QCefSetting setting;

#if CEF_VERSION_MAJOR < 100
    setting.setPlugins(false);
#endif

    setting.setWindowlessFrameRate(60);
    setting.setBackgroundColor(QColor::fromRgba(qRgba(255, 255, 220, 255)));
    // setting.setBackgroundColor(Qt::blue);

    // create the QCefView widget and add it to the layout container
    // m_pRightCefViewWidget = new CefViewWidget("https://cefview.github.io/QCefView/", &setting, this);

    //
    m_pRightCefViewWidget = new CefViewWidget("https://www.baidu.com", &setting, this);
    m_pRightCefViewWidget->listResponseFilterWords.append("/sugrec?");

    // m_pRightCefViewWidget = new CefViewWidget("https://fastest.fish/test-files", &setting, this);

    //
    // m_pRightCefViewWidget = new CefViewWidget("https://mdn.dev/", &setting, this);

    // this site is for test OSR performance
    // m_pRightCefViewWidget = new CefViewWidget("https://www.testufo.com", &setting, this);

    // this site is test for input devices
    // m_pRightCefViewWidget = new CefViewWidget("https://devicetests.com", &setting);

    m_ui.rightCefViewContainer->layout()->addWidget(m_pRightCefViewWidget);

    // allow show context menu for both OSR and NCW mode
    m_pRightCefViewWidget->setContextMenuPolicy(Qt::DefaultContextMenu);

    // all the following values will disable the context menu for both NCW and OSR mode
    // m_pRightCefViewWidget->setContextMenuPolicy(Qt::NoContextMenu);
    // m_pRightCefViewWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    // m_pRightCefViewWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    // m_pRightCefViewWidget->setContextMenuPolicy(Qt::PreventContextMenu);
}

void
MainWindow::onInvokeMethod(int browserId,
#if CEF_VERSION_MAJOR > 119
                           QString frameId,
#else
                           int64_t frameId,
#endif
                           const QString& method,
                           const QVariantList& arguments)
{
    // extract the arguments and dispatch the invocation to corresponding handler
    if (0 == method.compare("TestMethod")) {
        QString title("QCef InvokeMethod Notify");
        QString text = QString("================== Current Thread: QT_UI ==================\r\n"
                               "Frame: %1\r\n"
                               "Method: %2\r\n"
                               "Arguments:\r\n");

#if CEF_VERSION_MAJOR > 119
        text = text.arg(frameId, method);
#else
        text = text.arg(QString::number(frameId), method);
#endif

        for (int i = 0; i < arguments.size(); i++) {
            auto jv = QJsonValue::fromVariant(arguments[i]);

            // clang-format off
          text.append(
              QString("%1 Type:%2, Value:%3\r\n")
              .arg(i).arg(arguments[i].typeName()).arg(arguments[i].toString())
          );
            // clang-format on
        }

        auto jsonValue = QJsonDocument::fromVariant(arguments);
        auto jsonString = QString(jsonValue.toJson());
        text.append(QString("\r\nArguments List in JSON format:\r\n%1").arg(jsonString));

        QMessageBox::information(this->window(), title, text);
    } else {
    }
}

void
MainWindow::onQCefQueryRequest(int browserId,
#if CEF_VERSION_MAJOR > 119
                               QString frameId,
#else
                               int64_t frameId,
#endif
                               const QCefQuery& query)
{
    QString title("QCef Query Request");
    QString text = QString("Current Thread: QT_UI\r\n"
                           "Query: %1")
                     .arg(query.request());

    QMessageBox::information(this->window(), title, text);

    QString response = query.request().toUpper();
    query.setResponseResult(true, response);
    m_pLeftCefViewWidget->responseQCefQuery(query);
}

void
MainWindow::onJavascriptResult(int browserId,
#if CEF_VERSION_MAJOR > 119
                               QString frameId,
#else
                               int64_t frameId,
#endif
                               const QString& context,
                               const QVariant& result)
{
    auto jsonValue = QJsonDocument::fromVariant(result);
    auto jsonString = QString(jsonValue.toVariant().toString());

    QString title("Javascript result notification");
    QString text = QString("Context: %1\r\nResult in JSON format:\r\n%2").arg(context).arg(jsonString);

    QMessageBox::information(this->window(), title, text);
}

void
MainWindow::onLoadingStateChanged(int browserId, bool isLoading, bool canGoBack, bool canGoForward)
{
    qDebug() << "onLoadingStateChanged, browserId:" << browserId << ", isLoading:" << isLoading << ", canGoBack:" << canGoBack
             << ", canGoForward:" << canGoForward;
}

void
MainWindow::onLoadStart(int browserId,
#if CEF_VERSION_MAJOR > 119
                        QString frameId,
#else
                        int64_t frameId,
#endif
                        bool isMainFrame,
                        int transitionType)
{

    qDebug() << "onLoadStart, browserId:" << browserId << ", frameId:" << frameId << ", isMainFrame:" << isMainFrame
             << ", transitionType:" << transitionType;
}

void
MainWindow::onLoadEnd(int browserId,
#if CEF_VERSION_MAJOR > 119
                      QString frameId,
#else
                      int64_t frameId,
#endif
                      bool isMainFrame,
                      int httpStatusCode)
{

    qDebug() << "onLoadEnd, browserId:" << browserId << ", frameId:" << frameId << ", isMainFrame:" << isMainFrame
             << ", httpStatusCode:" << httpStatusCode;
}

void
MainWindow::onLoadError(int browserId,
#if CEF_VERSION_MAJOR > 119
                        QString frameId,
#else
                        int64_t frameId,
#endif
                        bool isMainFrame,
                        int errorCode,
                        const QString& errorMsg,
                        const QString& failedUrl)
{

    qDebug() << "onLoadError, browserId:" << browserId << ", frameId:" << frameId << ", isMainFrame:" << isMainFrame
             << ", errorCode:" << errorCode;
}

void
MainWindow::onBtnShowDevToolsClicked()
{
    if (m_pLeftCefViewWidget) {
        m_pLeftCefViewWidget->showDevTools();
    }
}

void
MainWindow::onBtnReloadRightViewClicked()
{
    if (m_pRightCefViewWidget) {
        m_pRightCefViewWidget->navigateToUrl("https://www.google.com");
    }
}

void
MainWindow::onBtnRecreateRightViewClicked()
{
    createRightCefView();
}

void
MainWindow::onBtnChangeColorClicked()
{
    if (m_pLeftCefViewWidget) {
        // create a random color
        QColor color(QRandomGenerator::global()->generate());

        // create the cef event and set the arguments
        QCefEvent event("colorChange");
        event.arguments().append(QVariant::fromValue(color.name(QColor::HexArgb)));

        // broadcast the event to all frames in all browsers created by this QCefView widget
        m_pLeftCefViewWidget->broadcastEvent(event);
    }
}

void
MainWindow::onBtnCallJSCodeClicked()
{
    QString context = "helloQCefView";
    QString code = "alert('hello QCefView'); return {k1: 'str', k2: true, k3: 100};";
    m_pLeftCefViewWidget->executeJavascriptWithResult(QCefView::MainFrameID, code, "", context);
}

void
MainWindow::onBtnSetFocusClicked()
{
    if (m_pLeftCefViewWidget) {
        m_pLeftCefViewWidget->setFocus();
    }
}

void
MainWindow::onBtnNewBrowserClicked()
{
    QMainWindow* w = new QMainWindow(nullptr);
    w->setAttribute(Qt::WA_DeleteOnClose);

    QCefSetting settings;
    QCefView* view = new QCefView("https://cefview.github.io/QCefView/", &settings, w);

    w->setCentralWidget(view);
    w->resize(1024, 768);
    w->show();
}

#ifndef Q_OS_MACOS
void
MainWindow::setupWindow()
{
}
#endif

void
MainWindow::on_pushButton_clicked()
{

    m_ui.pushButton->setEnabled(false);

    auto td = QThread::create([=]() {
        QString jstr = R"(
var data=[];
var es=document.getElementsByTagName('input');
for(var i=0;i<es.length;i++){
  var e=es[i];
 if(e.type=='file'){
  e.scrollIntoView(true);
  // e.click();
  var r = e.getBoundingClientRect();
  data = [r.left, r.top, e.clientWidth, e.clientHeight, e.innerText];
  break;
 }
}
return JSON.stringify(data);
)";

        jstr = m_pLeftCefViewWidget->EvaluateJavascript(jstr.trimmed());
        QJsonDocument doc = QJsonDocument::fromJson(jstr.toUtf8());

        QMetaObject::invokeMethod(this, [=]() {
            if (doc.isArray()) {
                auto arr = doc.array();
                if (arr.size() > 2) {
                    auto x = arr.at(0).toDouble();
                    auto y = arr.at(1).toDouble();
                    auto w = arr.at(2).toDouble();
                    auto h = arr.at(3).toDouble();

                    m_pLeftCefViewWidget->SendMouseClickEvent(x + w / 2, y + h / 2, Qt::LeftButton, false, 1);
                    m_pLeftCefViewWidget->SendMouseClickEvent(x + w / 2, y + h / 2, Qt::LeftButton, true, 1);

                    qDebug() << "click at:" << (x + w / 2) << (y + h / 2) << "x:" << x << "y:" << y << "w:" << w << "h:" << h;
                }

                // QMessageBox::information(this, "Result", jstr);
            }

            m_ui.pushButton->setEnabled(true);
        });
    });

    connect(td, &QThread::finished, td, &QThread::deleteLater);
    td->start();
}

void
MainWindow::on_pushButton_2_clicked()
{
    auto res = m_pRightCefViewWidget->getResponses();

    QString rs;
    for (auto& item : res) {
        rs += item.first + " : " + QString::number(item.second.length()) + "\n";
    }

    QMessageBox::information(
      this, QString::number(rs.count()) + "-" + QString::number(m_pLeftCefViewWidget->listResponse.count()), rs);
}

void
MainWindow::on_pushButton_4_clicked()
{
    auto rs = m_pRightCefViewWidget->getHtml();

    QMessageBox::information(this, "html", QString::number(rs.length()));
}

#include <windows.h>

void
MainWindow::on_pushButton_5_clicked()
{
    auto x = 250;
    auto y = 250;

    auto cw = m_pLeftCefViewWidget;

    cw->SendMouseMoveEvent(x, y, false);

    cw->SendMouseClickEvent(x, y, Qt::LeftButton, false, 1);
    cw->SendMouseClickEvent(x, y, Qt::LeftButton, true, 1);

    cw->setFocus();

    cw->SendKeyEvent(VK_BACK, true);
    cw->SendKeyEvent(VK_BACK, false);

    cw->SendKeyEvent(0x30, true);
    cw->SendKeyEvent(0x30, false);

    cw->SendKeyEvent(0x31, true);
    cw->SendKeyEvent(0x31, false);

    cw->SendMouseMoveEvent(x, y, true);
}

void
MainWindow::on_pushButton_7_clicked()
{
    auto rs = m_pRightCefViewWidget->exportCookies();

    QMessageBox::information(this, "cookie", rs);
}
