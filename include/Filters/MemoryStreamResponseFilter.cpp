
#include "Filters/MemoryStreamResponseFilter.h"

#include <algorithm>
#include <iostream>

Filters::MemoryStreamResponseFilter::MemoryStreamResponseFilter() {}

bool
Filters::MemoryStreamResponseFilter::InitFilter()
{
    return true;
}

Filters::FilterStatus
Filters::MemoryStreamResponseFilter::Filter(void* data_in,
                                            size_t data_in_size,
                                            size_t& data_in_read,
                                            void* data_out,
                                            size_t data_out_size,
                                            size_t& data_out_written)
{
    data_in_read = 0;
    data_out_written = 0;

    try {
        if (data_in == nullptr) {
            size_t max_write = std::min(data_out_buffer.size(), data_out_size);

            if (max_write > 0) {
                memcpy(data_out, data_out_buffer.data(), max_write);
                data_out_written += max_write;
            }

            if (max_write < data_out_buffer.size()) {
                data_out_buffer.erase(data_out_buffer.begin(), data_out_buffer.begin() + max_write);
                return RESPONSE_FILTER_NEED_MORE_DATA;
            }

            data_out_buffer.clear();
            return RESPONSE_FILTER_DONE;
        }

        data_in_read = data_in_size;
        const char* data_in_buffer = static_cast<const char*>(data_in);
        data_out_buffer.insert(data_out_buffer.end(), data_in_buffer, data_in_buffer + data_in_size);
        overflow.insert(overflow.end(), data_in_buffer, data_in_buffer + data_in_size);

        if (data_in_size < data_out_size) {
            size_t max_write = std::min(data_out_buffer.size(), data_out_size);

            if (max_write > 0) {
                memcpy(data_out, data_out_buffer.data(), max_write);
                data_out_written += max_write;
            }

            if (max_write < data_out_buffer.size()) {
                data_out_buffer.erase(data_out_buffer.begin(), data_out_buffer.begin() + max_write);
                return RESPONSE_FILTER_NEED_MORE_DATA;
            }

            data_out_buffer.clear();
            return RESPONSE_FILTER_DONE;
        } else {
            return RESPONSE_FILTER_NEED_MORE_DATA;
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return RESPONSE_FILTER_DONE;
}

std::vector<char>
Filters::MemoryStreamResponseFilter::ToArray() const
{
    return overflow;
}

std::string
Filters::MemoryStreamResponseFilter::ToString() const
{
    return std::string(overflow.begin(), overflow.end());
}
