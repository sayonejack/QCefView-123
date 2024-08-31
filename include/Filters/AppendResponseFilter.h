#ifndef APPENDRESPONSEFILTER_H
#define APPENDRESPONSEFILTER_H

#include <string>
#include <vector>

#include "Filters/IResponseFilter.h"
namespace Filters {
class AppendResponseFilter : public IResponseFilter
{
  public:
    AppendResponseFilter(const std::string& stringToAppend);

    AppendResponseFilter(const std::vector<char>& bytes);

    bool InitFilter() override;

    FilterStatus Filter(void* data_in,
                        size_t data_in_size,
                        size_t& data_in_read,
                        void* data_out,
                        size_t data_out_size,
                        size_t& data_out_written) override;

    void Dispose();

  private:
    std::vector<char> dataOutBuffer;
};
} // namespace Filters
#endif // APPENDRESPONSEFILTER_H
