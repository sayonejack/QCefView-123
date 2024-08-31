#ifndef MY_CEF_STRING_VISITOR_H
#define MY_CEF_STRING_VISITOR_H


#include <QString>
#include <include/cef_string_visitor.h>
#include <functional>


class MyCefStringVisitor : public CefStringVisitor
{
public:
  explicit MyCefStringVisitor(std::function<void(const QString&)> callback)
    : callback_(callback)
  {
  }

  virtual void Visit(const CefString& string) override
  {
    QString html = QString::fromStdWString(string.ToWString());
    callback_(html);
  }

private:
  std::function<void(const QString&)> callback_;

  IMPLEMENT_REFCOUNTING(MyCefStringVisitor);
};

#endif // MY_CEF_STRING_VISITOR_H
