#include <CefViewBrowserClient.h>

// #pragma region std_headers
// #include <sstream>
// #include <string>
// #include <algorithm>
// #pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/cef_base.h>
#pragma endregion cef_headers

#include <Common/CefViewCoreLog.h>

// #include "Filters/MemoryStreamResponseFilter.h"
// #include "Filters/AppendResponseFilter.h"

CefRefPtr<CefResourceRequestHandler>
CefViewBrowserClient::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                CefRefPtr<CefRequest> request,
                                                bool is_navigation,
                                                bool is_download,
                                                const CefString& request_initiator,
                                                bool& disable_default_handling)
{
    return this;
}

CefRefPtr<CefResourceHandler>
CefViewBrowserClient::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
    return resource_manager_->GetResourceHandler(browser, frame, request);
}

void
CefViewBrowserClient::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefRequest> request,
                                          bool& allow_os_execution)
{
}

CefResourceRequestHandler::ReturnValue
CefViewBrowserClient::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
#if CEF_VERSION_MAJOR > 91
                                           CefRefPtr<CefCallback> callback
#else
                                           CefRefPtr<CefRequestCallback> callback
#endif
)
{

    // auto url = request->GetURL().ToString();
    //  logD("require resource: %s", url.c_str());

    auto delegate = client_delegate_.lock();
    if (delegate) {
        if (delegate->onBeforeResourceLoad(browser, frame, request, callback) ==
            CefResourceRequestHandler::ReturnValue::RV_CANCEL) {
            callback->Cancel();
            return CefResourceRequestHandler::ReturnValue::RV_CONTINUE_ASYNC;
        }
    }

    return resource_manager_->OnBeforeResourceLoad(browser, frame, request, callback);
}

// 实现 GetResourceResponseFilter 方法
CefRefPtr<CefResponseFilter>
CefViewBrowserClient::GetResourceResponseFilter(CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                CefRefPtr<CefRequest> request,
                                                CefRefPtr<CefResponse> response)
{

    auto delegate = client_delegate_.lock();
    if (delegate)
        return delegate->getResourceResponseFilter(browser, frame, request, response);

    return nullptr;
}

// OnResourceLoadComplete

void
CefViewBrowserClient::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefRequest> request,
                                             CefRefPtr<CefResponse> response,
                                             CefResourceRequestHandler::URLRequestStatus status,
                                             int64_t received_content_length)
{

    auto delegate = client_delegate_.lock();
    if (delegate)
        delegate->onResourceLoadComplete(browser, frame, request, response, status, received_content_length);
}
