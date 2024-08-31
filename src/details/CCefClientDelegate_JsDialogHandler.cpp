#include "CCefClientDelegate.h"

#include <QDebug>
#include <QThread>

// #include "utils/CommonUtils.h"
// #include "utils/MenuBuilder.h"

#pragma region cef_headers
#include <include/cef_client.h>
#include <include/cef_jsdialog_handler.h>
#include <include/cef_version.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#pragma endregion cef_headers

#include "QCefViewPrivate.h"

// bool
// CCefClientDelegate::onBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
//                                          const CefString& message_text,
//                                          bool is_reload,
//                                          CefRefPtr<CefJSDialogCallback> callback)
// {

//   if (pCefViewPrivate_->isOSRModeEnabled()) {
//     bool result = false;
//     QMetaObject::invokeMethod(
//       pCefViewPrivate_,
//       [=, &result]() { result = pCefViewPrivate_->onBeforeUnloadDialog(); },
//       Qt::BlockingQueuedConnection);

//     if (result) {
//       callback->Continue(true, "");
//       return true;
//     }
//   }

//   return false;
// }

bool
CCefClientDelegate::onJSDialog(CefRefPtr<CefBrowser> browser,
                               const CefString& origin_url,
                               CefJSDialogHandler::JSDialogType dialog_type,
                               const CefString& message_text,
                               const CefString& default_prompt_text,
                               CefRefPtr<CefJSDialogCallback> callback,
                               bool& suppress_message)
{
    qDebug() << "onJSDialog" << message_text.ToString().c_str();

    if (pCefViewPrivate_->isOSRModeEnabled()) {
        bool result = false;
        QMetaObject::invokeMethod(
          pCefViewPrivate_, [=, &result]() { result = pCefViewPrivate_->onJSDialog(); }, Qt::BlockingQueuedConnection);

        qDebug() << "CCefClientDelegate onJSDialog result" << result;

        if (result) {
            callback->Continue(true, "");
            return true;
        }
    }

    return false;
}
