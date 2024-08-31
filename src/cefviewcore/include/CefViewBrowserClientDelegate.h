﻿//
//  CefViewBrowserHandlerDelegate.h
//  CefView
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewBrowserHandlerDelegate_h
#define CefViewBrowserHandlerDelegate_h
#pragma once

#include <memory>
#include <string>

#include <include/cef_version.h>
#include <include/cef_client.h>

/// <summary>
///
/// </summary>
class CefViewBrowserClientDelegateInterface
{
  public:
    typedef std::shared_ptr<CefViewBrowserClientDelegateInterface> RefPtr;
    typedef std::weak_ptr<CefViewBrowserClientDelegateInterface> WeakPtr;

    virtual ~CefViewBrowserClientDelegateInterface() {}

    virtual void processUrlRequest(const std::string& url) = 0;

    virtual void processQueryRequest(CefRefPtr<CefBrowser>& browser,
#if CEF_VERSION_MAJOR > 119
                                     std::string frameId,
#else
                                     int64_t frameId,
#endif
                                     const std::string& query,
                                     const int64_t query_id) = 0;

    virtual void focusedEditableNodeChanged(CefRefPtr<CefBrowser>& browser,
#if CEF_VERSION_MAJOR > 119
                                            std::string frameId,
#else
                                            int64_t frameId,
#endif
                                            bool focusOnEditableNode) = 0;

    virtual void invokeMethodNotify(CefRefPtr<CefBrowser>& browser,
#if CEF_VERSION_MAJOR > 119
                                    std::string frameId,
#else
                                    int64_t frameId,
#endif
                                    const std::string& method,
                                    const CefRefPtr<CefListValue>& arguments) = 0;

    virtual void reportJSResult(CefRefPtr<CefBrowser>& browser,
#if CEF_VERSION_MAJOR > 119
                                std::string frameId,
#else
                                int64_t frameId,
#endif
                                const std::string& context,
                                const CefRefPtr<CefValue>& result) = 0;

    // context menu handler
#pragma region ContextMenuHandler
    virtual void onBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model) = 0;
    virtual bool onRunContextMenu(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefContextMenuParams> params,
                                  CefRefPtr<CefMenuModel> model,
                                  CefRefPtr<CefRunContextMenuCallback> callback) = 0;
    virtual bool onContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params,
                                      int command_id,
                                      CefContextMenuHandler::EventFlags event_flags) = 0;
    virtual void onContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) = 0;
#pragma endregion

    // dialog handler
#pragma region DialogHandler
    virtual bool onFileDialog(CefRefPtr<CefBrowser> browser,
                              CefBrowserHost::FileDialogMode mode,
                              const CefString& title,
                              const CefString& default_file_path,
                              const std::vector<CefString>& accept_filters,
#if CEF_VERSION_MAJOR < 102
                              int selected_accept_filter,
#endif
                              CefRefPtr<CefFileDialogCallback> callback)
    {
        return false;
    };
#pragma endregion

    // display handler
#pragma region DisplayHandler
    virtual void addressChanged(CefRefPtr<CefBrowser>& browser,
#if CEF_VERSION_MAJOR > 119
                                std::string frameId,
#else
                                int64_t frameId,
#endif
                                const std::string& url) = 0;

    virtual void titleChanged(CefRefPtr<CefBrowser>& browser, const std::string& title) = 0;

    virtual void faviconURLChanged(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) = 0;

    virtual bool tooltipMessage(CefRefPtr<CefBrowser>& browser, const std::string& text) = 0;

    virtual void fullscreenModeChanged(CefRefPtr<CefBrowser>& browser, bool fullscreen) = 0;

    virtual void statusMessage(CefRefPtr<CefBrowser>& browser, const std::string& value) = 0;

    virtual void loadingProgressChanged(CefRefPtr<CefBrowser>& browser, double progress) = 0;

    virtual void consoleMessage(CefRefPtr<CefBrowser>& browser, const std::string& message, int level) = 0;

    virtual bool cursorChanged(CefRefPtr<CefBrowser> browser,
                               CefCursorHandle cursor,
                               cef_cursor_type_t type,
                               const CefCursorInfo& custom_cursor_info) = 0;
#pragma endregion

    // download hander
#pragma region DownloadHandler
    virtual void onBeforeDownload(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDownloadItem> download_item,
                                  const CefString& suggested_name,
                                  CefRefPtr<CefBeforeDownloadCallback> callback) = 0;

    virtual void onDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefDownloadItem> download_item,
                                   CefRefPtr<CefDownloadItemCallback> callback) = 0;
#pragma endregion

#pragma region GetDragHandler
    // drag hander
    virtual bool onDragEnter(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefDragData> dragData,
                             CefDragHandler::DragOperationsMask mask)
    {
        return true;
    }

    virtual void draggableRegionChanged(CefRefPtr<CefBrowser>& browser, const std::vector<CefDraggableRegion>& regions) = 0;
#pragma endregion

    // life span handler
#pragma region LifeSpanHandler
    virtual bool onBeforePopup(CefRefPtr<CefBrowser>& browser,
#if CEF_VERSION_MAJOR > 119
                               std::string frameId,
#else
                               int64_t frameId,
#endif
                               const std::string& targetUrl,
                               const std::string& targetFrameName,
                               CefLifeSpanHandler::WindowOpenDisposition targetDisposition,
                               CefWindowInfo& windowInfo,
                               CefBrowserSettings& settings,
                               bool& DisableJavascriptAccess) = 0;
    virtual void onAfterCreate(CefRefPtr<CefBrowser>& browser) = 0;

    virtual bool doClose(CefRefPtr<CefBrowser> browser) = 0;

    virtual bool requestClose(CefRefPtr<CefBrowser> browser) = 0;

    virtual void onBeforeClose(CefRefPtr<CefBrowser> browser) = 0;
#pragma endregion

    // focus handler
#pragma region FocusHandler
    virtual void takeFocus(CefRefPtr<CefBrowser>& browser, bool next) = 0;

    virtual bool setFocus(CefRefPtr<CefBrowser>& browser) = 0;

    virtual void gotFocus(CefRefPtr<CefBrowser>& browser) = 0;
#pragma endregion

    // load handler
#pragma region LoadHandler
    virtual void loadingStateChanged(CefRefPtr<CefBrowser>& browser, bool isLoading, bool canGoBack, bool canGoForward) = 0;

    virtual void loadStart(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefFrame>& frame, int transition_type) = 0;

    virtual void loadEnd(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefFrame>& frame, int httpStatusCode) = 0;

    virtual void loadError(CefRefPtr<CefBrowser>& browser,
                           CefRefPtr<CefFrame>& frame,
                           int errorCode,
                           const std::string& errorMsg,
                           const std::string& failedUrl,
                           bool& handled) = 0;
#pragma endregion

    // keyboard handler
#pragma region KeyboardHandler
    virtual bool onPreKeyEvent(CefRefPtr<CefBrowser> browser,
                               const CefKeyEvent& event,
                               CefEventHandle os_event,
                               bool* is_keyboard_shortcut) = 0;

    virtual bool onKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event) = 0;
#pragma endregion

    // Off screen rendering
#pragma region RenderHandler
    virtual bool getRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) { return false; }
    virtual void getViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {}
    virtual bool getScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY) { return false; }
    virtual bool getScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) { return false; }
    virtual void onPopupShow(CefRefPtr<CefBrowser> browser, bool show) {}
    virtual void onPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) {}
    virtual void onPaint(CefRefPtr<CefBrowser> browser,
                         CefRenderHandler::PaintElementType type,
                         const CefRenderHandler::RectList& dirtyRects,
                         const void* buffer,
                         int width,
                         int height)
    {
    }
    virtual void onAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                    CefRenderHandler::PaintElementType type,
                                    const CefRenderHandler::RectList& dirtyRects,
                                    void* shared_handle)
    {
    }
    virtual bool startDragging(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefDragData> drag_data,
                               CefRenderHandler::DragOperationsMask allowed_ops,
                               int x,
                               int y)
    {
        return false;
    }
    virtual void updateDragCursor(CefRefPtr<CefBrowser> browser, CefRenderHandler::DragOperation operation) {}
    virtual void onScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y) {}
    virtual void onImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                              const CefRange& selected_range,
                                              const CefRenderHandler::RectList& character_bounds)
    {
    }
    virtual void onTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                        const CefString& selected_text,
                                        const CefRange& selected_range)
    {
    }
    virtual void onVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, CefRenderHandler::TextInputMode input_mode) {}
#pragma endregion

#pragma region JSDialogHandler

    virtual bool onJSDialog(CefRefPtr<CefBrowser> browser,
                            const CefString& origin_url,
                            CefJSDialogHandler::JSDialogType dialog_type,
                            const CefString& message_text,
                            const CefString& default_prompt_text,
                            CefRefPtr<CefJSDialogCallback> callback,
                            bool& suppress_message)
    {
        return false;
    }

    virtual bool onBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                      const CefString& message_text,
                                      bool is_reload,
                                      CefRefPtr<CefJSDialogCallback> callback)
    {
        return false;
    }

#pragma endregion

#pragma region ResourceRequestHandler

    virtual CefRefPtr<CefResponseFilter> getResourceResponseFilter(CefRefPtr<CefBrowser> browser,
                                                                   CefRefPtr<CefFrame> frame,
                                                                   CefRefPtr<CefRequest> request,
                                                                   CefRefPtr<CefResponse> response)
    {
        return nullptr;
    }

    virtual CefResourceRequestHandler::ReturnValue onBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                                                        CefRefPtr<CefFrame> frame,
                                                                        CefRefPtr<CefRequest> request,
#if CEF_VERSION_MAJOR > 91
                                                                        CefRefPtr<CefCallback> callback
#else
                                                                        CefRefPtr<CefRequestCallback> callback
#endif
    )
    {
        return CefResourceRequestHandler::ReturnValue::RV_CONTINUE;
    }

    virtual void onResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        CefRefPtr<CefResponse> response,
                                        CefResourceRequestHandler::URLRequestStatus status,
                                        int64_t received_content_length)
    {
    }

#pragma endregion
};

#endif
