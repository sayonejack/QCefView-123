#include "MyDownloadImageCallback.h"
#include <QApplication>
#include <QClipboard>
#include <QStandardPaths>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QMimeData>
#include <QDebug>

#include "QCefView.h"

//
//HBITMAP MyDownloadImageCallback::QImageToHBITMAP(const QImage& image)
//{
//
//	QImage img = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
//	// 修改透明部分为白色
//	for (int y = 0; y < img.height(); ++y) {
//		QRgb* line = reinterpret_cast<QRgb*>(img.scanLine(y));
//		for (int x = 0; x < img.width(); ++x) {
//			if (qAlpha(line[x]) < 255) {
//				line[x] = qRgba(255, 255, 255, 255);
//			}
//		}
//	}
//
//	HBITMAP hBitmap = CreateCompatibleBitmap(GetDC(0), img.width(), img.height());
//
//	if (hBitmap) {
//		BITMAPINFOHEADER bi;
//		ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
//		bi.biSize = sizeof(BITMAPINFOHEADER);
//		bi.biWidth = img.width();
//		bi.biHeight = -img.height(); // Top-down DIB
//		bi.biPlanes = 1;
//		bi.biBitCount = 32;
//		bi.biCompression = BI_RGB;
//
//		HDC hdc = CreateCompatibleDC(GetDC(0));
//		void* pBits = NULL;
//		HBITMAP hDIB = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &pBits, NULL, 0);
//		if (hDIB) {
//			memcpy(pBits, img.bits(), img.byteCount());
//
//			HDC hSrcDC = CreateCompatibleDC(GetDC(0));
//			HBITMAP hOldSrcBitmap = static_cast<HBITMAP>(SelectObject(hSrcDC, hDIB));
//			HBITMAP hOldDestBitmap = static_cast<HBITMAP>(SelectObject(hdc, hBitmap));
//			BitBlt(hdc, 0, 0, img.width(), img.height(), hSrcDC, 0, 0, SRCCOPY);
//			SelectObject(hSrcDC, hOldSrcBitmap);
//			SelectObject(hdc, hOldDestBitmap);
//			DeleteDC(hSrcDC);
//		}
//		DeleteDC(hdc);
//		if (hDIB)
//			DeleteObject(hDIB); // 删除临时的 DIB
//	}
//
//	return hBitmap;
//}
//
//void MyDownloadImageCallback::CopyHBITMAPToClipboard(HBITMAP hBitmap)
//{
//	if (OpenClipboard(NULL)) {
//		EmptyClipboard();
//		SetClipboardData(CF_BITMAP, hBitmap);
//		CloseClipboard();
//	}
//}
//
