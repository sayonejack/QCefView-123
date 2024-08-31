#include "CCefClientDelegate.h"

#include <QDebug>
#include <QThread>

#include "utils/CommonUtils.h"
// #include "utils/MenuBuilder.h"

#include "QCefViewPrivate.h"

#include "Filters/CefResponseFilterWrapper.h"

CefResourceRequestHandler::ReturnValue
CCefClientDelegate::onBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefRequest> request,
#if CEF_VERSION_MAJOR > 91
                                         CefRefPtr<CefCallback> callback
#else
                                         CefRefPtr<CefRequestCallback> callback
#endif
)
{

    // FLog();

    try {

        if (pCefViewPrivate_->isOSRModeEnabled()) {
            QString url = QString::fromStdString(request->GetURL().ToString());
            QString method = QString::fromStdString(request->GetMethod().ToString());
            QString referer = QString::fromStdString(request->GetReferrerURL().ToString());

            QString postDataStr;
            if (method.toUpper() == "POST") {
                // 获取 POST 数据
                CefRefPtr<CefPostData> postData = request->GetPostData();
                if (postData) {
                    CefPostData::ElementVector elements;
                    postData->GetElements(elements);

                    for (auto& element : elements) {
                        if (element->GetType() == PDE_TYPE_BYTES) {
                            size_t size = element->GetBytesCount();
                            std::vector<char> bytes(size);
                            element->GetBytes(size, bytes.data());

                            postDataStr = QString::fromStdString(std::string(bytes.begin(), bytes.end()));
                            break;
                        }
                    }
                }
            }

            // QString postDataStr2(postDataStr.constData());

            int resourceType = request->GetResourceType();
            int result = -1;

            QMetaObject::invokeMethod(
              pCefViewPrivate_,
              [&]() { result = pCefViewPrivate_->onBeforeResourceLoad(url, postDataStr, referer, method, resourceType); },
              Qt::BlockingQueuedConnection);

            // result
            if (result == -1) {
                return CefResourceRequestHandler::ReturnValue::RV_CONTINUE;
            } else if (url != QString::fromStdString(request->GetURL().ToString())) {
                request->SetURL(url.toStdString());
            }

            if (!referer.isEmpty()) {
                request->SetReferrer(referer.toStdString(), REFERRER_POLICY_DEFAULT);
            }

            if (result > 0 && method.toUpper() == "POST" && !postDataStr.isEmpty()) {
                // 设置 POST 数据
                CefRefPtr<CefPostData> postData = request->GetPostData();
                if (postData) {
                    auto str = postDataStr.toStdString();
                    CefRefPtr<CefPostDataElement> newElement = CefPostDataElement::Create();
                    newElement->SetToBytes(str.size(), str.c_str());
                    postData->RemoveElements();
                    postData->AddElement(newElement);

                    request->SetPostData(postData);
                }
            }

            if (result == 0) {
                return CefResourceRequestHandler::ReturnValue::RV_CANCEL;
            } else if (result == 1) {
                return CefResourceRequestHandler::ReturnValue::RV_CONTINUE;
            } else if (result == 2) {
                return CefResourceRequestHandler::ReturnValue::RV_CONTINUE_ASYNC;
            }
        }

    } catch (const std::exception& ex) {
        qDebug() << "onBeforeResourceLoad:" << ex.what();
    }

    return CefResourceRequestHandler::ReturnValue::RV_CONTINUE;
}

// 实现 GetResourceResponseFilter 方法
CefRefPtr<CefResponseFilter>
CCefClientDelegate::getResourceResponseFilter(CefRefPtr<CefBrowser> browser,
                                              CefRefPtr<CefFrame> frame,
                                              CefRefPtr<CefRequest> request,
                                              CefRefPtr<CefResponse> response)
{

    // FLog();

    if (pCefViewPrivate_->isOSRModeEnabled()) {

        QString url = QString::fromStdString(request->GetURL().ToString());

        Filters::IResponseFilter* result;

        QMetaObject::invokeMethod(
          pCefViewPrivate_,
          [=, &result]() { result = pCefViewPrivate_->getResourceResponseFilter(url); },
          Qt::BlockingQueuedConnection);

        if (result) {
            return new CefResponseFilterWrapper(result);
        }
    }

    return nullptr;
}

// 实现 OnResourceLoadComplete 方法
void
CCefClientDelegate::onResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
                                           CefRefPtr<CefResponse> response,
                                           CefResourceRequestHandler::URLRequestStatus status,
                                           int64_t received_content_length)
{

    // FLog();

    if (pCefViewPrivate_->isOSRModeEnabled()) {
        QString url = QString::fromStdString(request->GetURL().ToString());
        QString method = QString::fromStdString(request->GetMethod().ToString());
        int statusCode = response->GetStatus();
        QString statusText = QString::fromStdString(response->GetStatusText().ToString());

        QMetaObject::invokeMethod(
          pCefViewPrivate_,
          [=]() {
              pCefViewPrivate_->onResourceLoadComplete(url, method, statusCode, statusText, status, received_content_length);
          },
          Qt::BlockingQueuedConnection);
    }
}
