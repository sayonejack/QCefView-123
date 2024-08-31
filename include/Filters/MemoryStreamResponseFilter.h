#ifndef MEMORYSTREAMRESPONSEFILTER_H
#define MEMORYSTREAMRESPONSEFILTER_H

#include <string>
#include <vector>

#include "Filters/IResponseFilter.h"

namespace Filters {
class MemoryStreamResponseFilter : public IResponseFilter
{
  public:
    MemoryStreamResponseFilter();

    bool InitFilter() override;

    FilterStatus Filter(void* data_in,
                        size_t data_in_size,
                        size_t& data_in_read,
                        void* data_out,
                        size_t data_out_size,
                        size_t& data_out_written) override;

    std::vector<char> ToArray() const;

    std::string ToString() const;

  private:
    std::vector<char> data_out_buffer;
    std::vector<char> overflow;
};
} // namespace Filters
#endif // MEMORYSTREAMRESPONSEFILTER_H
