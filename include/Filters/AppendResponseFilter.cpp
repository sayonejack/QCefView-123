
#include "Filters/AppendResponseFilter.h"

#include <iostream>

Filters::AppendResponseFilter::AppendResponseFilter(const std::string& stringToAppend)
{
    // 将要追加的字符串编码为字节并添加到缓冲区
    std::vector<char> encodedString(stringToAppend.begin(), stringToAppend.end());
    dataOutBuffer.insert(dataOutBuffer.end(), encodedString.begin(), encodedString.end());
}

Filters::AppendResponseFilter::AppendResponseFilter(const std::vector<char>& bytes)
{
    // 将字节数组添加到缓冲区
    dataOutBuffer.insert(dataOutBuffer.end(), bytes.begin(), bytes.end());
}

bool
Filters::AppendResponseFilter::InitFilter()
{
    return true;
}

Filters::FilterStatus
Filters::AppendResponseFilter::Filter(void* data_in,
                                      size_t data_in_size,
                                      size_t& data_in_read,
                                      void* data_out,
                                      size_t data_out_size,
                                      size_t& data_out_written)
{
    data_in_read = 0;
    data_out_written = 0;

    try {
        if (data_in != nullptr) {
            data_in_read = data_in_size;
        }

        size_t maxWrite = std::min(dataOutBuffer.size(), data_out_size);

        if (maxWrite > 0) {
            std::memcpy(data_out, dataOutBuffer.data(), maxWrite);
            data_out_written += maxWrite;
            dataOutBuffer.erase(dataOutBuffer.begin(), dataOutBuffer.begin() + maxWrite);
        }

        if (!dataOutBuffer.empty() && maxWrite > 0) {
            return FilterStatus::RESPONSE_FILTER_NEED_MORE_DATA;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return FilterStatus::RESPONSE_FILTER_DONE;
}

void
Filters::AppendResponseFilter::Dispose()
{
    // 资源释放（如果有）
}
