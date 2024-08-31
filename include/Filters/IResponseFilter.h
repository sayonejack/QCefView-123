
#ifndef IRESPONSEFILTER_H
#define IRESPONSEFILTER_H

namespace Filters {
enum FilterStatus
{
  ///
  /// Some or all of the pre-filter data was read successfully but more data is
  /// needed in order to continue filtering (filtered output is pending).
  ///
  RESPONSE_FILTER_NEED_MORE_DATA,

  ///
  /// Some or all of the pre-filter data was read successfully and all available
  /// filtered output has been written.
  ///
  RESPONSE_FILTER_DONE,

  ///
  /// An error occurred during filtering.
  ///
  RESPONSE_FILTER_ERROR
};

class IResponseFilter
{
public:
  virtual bool InitFilter() = 0;

  virtual FilterStatus Filter(void* data_in,
                     size_t data_in_size,
                     size_t& data_in_read,
                     void* data_out,
                     size_t data_out_size,
                     size_t& data_out_written) = 0;
};
} // namespace Filters
#endif // IRESPONSEFILTER_H
