#pragma once

#include "constants.h"
#include <vector>
#include <cstdint>

namespace encoder {

class LayerEncoder{
private:
    std::vector<uint8_t> data;
    uint8_t last_value{0};
public:
    LayerEncoder() = default;

    void add_run(uint32_t length, uint8_t value);
    
    uint8_t get_checksum() const {
        return calculate_checksum(this->data.data(), this->data.size());
    }

    const std::vector<uint8_t>& get_data() const {return this->data;}

    std::vector<uint8_t> take_data() {return std::move(this->data);}
};
    
} //namespace encoder