#ifndef MY_DOWNLOAD_IMAGE_CALLBACK_H
#define MY_DOWNLOAD_IMAGE_CALLBACK_H

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include <vector>
#include <QObject>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QPixmap>

// #include "MyDownloadImageHandler.h"

class MyDownloadImageCallback : public CefDownloadImageCallback
{
  public:
    MyDownloadImageCallback(QObject* q_object) { this->q_object = q_object; }

    void OnDownloadImageFinished(const CefString& image_url, int http_status_code, CefRefPtr<CefImage> image) override
    {
        CEF_REQUIRE_UI_THREAD();
        if (image) {
            int pixel_width, pixel_height;

            CefRefPtr<CefBinaryValue> binary_value;
            binary_value = image->GetAsPNG(1.0, true, pixel_width, pixel_height);
            // binary_value = image->GetAsBitmap(1.0, CEF_COLOR_TYPE_RGBA_8888, CEF_ALPHA_TYPE_PREMULTIPLIED, pixel_width,
            // pixel_height);

            if (binary_value) {
                size_t data_size = binary_value->GetSize();
                std::vector<unsigned char> image_data(data_size);
                binary_value->GetData(image_data.data(), data_size, 0);

                // 转换 std::vector<unsigned char> 到 QByteArray
                QByteArray byteArray(reinterpret_cast<const char*>(image_data.data()), static_cast<int>(image_data.size()));

                QMetaObject::invokeMethod(q_object, [=]() {
                    QPixmap pixmap;
                    // pixmap.loadFromData(byteArray);
                    if (pixmap.loadFromData(byteArray)) {
                        QClipboard* clipboard = QApplication::clipboard();
                        QMimeData* mimeData = new QMimeData;
                        mimeData->setImageData(pixmap.toImage());
                        clipboard->setMimeData(mimeData);
                    }
                });

                //// 调用槽函数
                // QMetaObject::invokeMethod(q_object, "handleDownloadedImage", Qt::QueuedConnection,
                //	Q_ARG(QByteArray, byteArray), Q_ARG(int, pixel_width), Q_ARG(int, pixel_height));

                ///*	HBITMAP hBitmap = QImageToHBITMAP(image);
                //	if (hBitmap) {
                //		CopyHBITMAPToClipboard(hBitmap);
                //		DeleteObject(hBitmap);
                //	}*/

                ///*	if (!CopyPNGToClipboard(image))
                //	{
                //		qDebug() << "Failed to copy image to clipboard.";
                //	}*/

                //}
            }
        }
    }

  private:
    QObject* q_object;
    // HBITMAP QImageToHBITMAP(const QImage& image);
    // void CopyHBITMAPToClipboard(HBITMAP hBitmap);

    IMPLEMENT_REFCOUNTING(MyDownloadImageCallback);
};

#endif // MY_DOWNLOAD_IMAGE_CALLBACK_H
