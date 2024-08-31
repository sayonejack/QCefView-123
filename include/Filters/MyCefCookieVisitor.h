#pragma once
#ifndef MY_CEF_COOKIE_VISITOR_H
#define MY_CEF_COOKIE_VISITOR_H

#include "include/cef_cookie.h"

#include <functional>

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>

class MyCefCookieVisitor : public CefCookieVisitor
{
public:
  // 构造函数，接受一个回调函数并初始化成员变量
  MyCefCookieVisitor(std::function<void(const QString&)> callback)
    : callback_(callback) // 初始化回调函数
  {
    // 这里可以添加任何其他初始化代码
  }

  QString toQString(const cef_string_t& s) {
      return QString::fromUtf16(reinterpret_cast<const ushort*>(s.str), s.length);

  }

  // CefCookieVisitor 的实现方法，用于访问每个 cookie
  bool Visit(const CefCookie& cookie, int count, int total, bool& deleteCookie) override
  {
    try {

      QJsonObject jsonCookie;

      std::wstring domainWStr = toQString(cookie.domain).toStdWString();


      jsonCookie["name"] = toQString(cookie.name);
      jsonCookie["value"] = toQString(cookie.value);
      jsonCookie["path"] = toQString(cookie.path);
      jsonCookie["domain"] = QString::fromStdWString(domainWStr);

      qint64 microsecondsSince1601 = cookie.expires.val;
      qint64 secondsSince1970 = (microsecondsSince1601 - 11644473600000000LL) / 1000000;
      if (secondsSince1970 >0) {
        jsonCookie["expirationDate"] = secondsSince1970; // 转换为秒
      }

      //jsonCookie["expirationDate"] = QString::number(cookie.expires.val).left(10).toInt(); // 转换为秒
      jsonCookie["httpOnly"] =(bool) cookie.httponly;

      jsonCookie["hostOnly"] = !domainWStr.empty() && domainWStr[0] != L'.'; // 判断是否为 hostOnly
      jsonCookie["sameSite"] = QJsonValue::Null; // CEF 的 `CefCookie` 没有直接对应 `sameSite` 的字段
      jsonCookie["secure"] = (bool)cookie.secure;
      jsonCookie["session"] = (bool)(cookie.expires.val == 0); // 如果 `expires` 是0，意味着是 session cookie
      jsonCookie["storeId"] = QJsonValue::Null;        // CEF 的 `CefCookie` 没有 `storeId` 字段

      cookies_.append(jsonCookie);

      // 如果已经访问到最后一个 cookie，调用回调函数
      if (count + 1 >= total) {

        // 将 QVector 转换为 JSON 字符串
        QString cookiesString = QString::fromUtf8(QJsonDocument(cookies_).toJson(QJsonDocument::Compact));
        // 调用回调函数
        callback_(cookiesString);

        return false; // 返回 false 停止访问
      }

    } catch (const std::exception& ex) {
      qDebug() << "Error in Visit" << ex.what();
      return false; // 返回 false 停止访问
    }

    return true; // 返回 true 继续访问下一个 cookie，返回 false 停止访问
  }

  IMPLEMENT_REFCOUNTING(MyCefCookieVisitor); // 引用计数宏，管理对象生命周期

private:
  QJsonArray cookies_;                           // 用于存储 cookies 的 QVector
  std::function<void(const QString&)> callback_; // 回调函数
};

#endif // MY_CEF_COOKIE_VISITOR_H
