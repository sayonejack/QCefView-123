
#ifndef CEFRESPONSEFILTERWRAPPER_H
#define CEFRESPONSEFILTERWRAPPER_H

#include <include/cef_app.h>
#include <include/cef_base.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>

#include "IResponseFilter.h"

class CefResponseFilterWrapper : public CefResponseFilter
{
private:
  Filters::IResponseFilter* filter_;

public:
  CefResponseFilterWrapper(Filters::IResponseFilter* filter)
    : filter_(filter)
  {
  }

  ~CefResponseFilterWrapper()
  {
    //if (filter_)
    //  delete filter_;
  }

  bool InitFilter() override { return filter_->InitFilter(); }

  FilterStatus Filter(void* data_in,
                      size_t data_in_size,
                      size_t& data_in_read,
                      void* data_out,
                      size_t data_out_size,
                      size_t& data_out_written) override
  {
    Filters::FilterStatus status =
      filter_->Filter(data_in, data_in_size, data_in_read, data_out, data_out_size, data_out_written);
    return static_cast<FilterStatus>(status);
  }

private:
  IMPLEMENT_REFCOUNTING(CefResponseFilterWrapper);
};

#endif // CEFRESPONSEFILTERWRAPPER_H
