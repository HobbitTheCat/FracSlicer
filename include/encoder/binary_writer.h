#pragma once

#include <cstddef>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstring>

namespace encoder {
    class BinaryWriter {
    private:
        std::ofstream stream;

    public:
        explicit BinaryWriter(const std::string& file_path);
        ~BinaryWriter();

        BinaryWriter(const BinaryWriter&) = delete;
        BinaryWriter& operator=(const BinaryWriter&) = delete;

        /**
         * @brief Writes data of type uint8_t (1 byte) to a file 
         */        
        void write_u8(uint8_t value);

        /**
         * @brief Writes data of type uint16_t (2 bytes, short int) to a file in Big-Endian order
         */
        void write_u16_be(uint16_t value);

        /**
         * @brief Writes data of type uint32_t (4 bytes, int) to a file in Big-Endian order
         */
        void write_u32_be(uint32_t value);

        /**
         * @brief Writes data of type float (4 bytes) to a file in Big-Endian order
         */
        void write_f32_be(float value);

        /**
         * @brief Writes data of type bool (1 bytes) to a file in Big-Endian order
         */
        void write_bool(bool value);

        /**
         * @brief Writes a raw byte array to a file
         */
        void write_bytes(const uint8_t* data, std::size_t size);

        /**
         * @brief Overloading for std::array and std::vector
         */
        template <typename Container>
        void write_bytes(const Container& container) {
            this->stream.write(reinterpret_cast<const char*>(container.data()), container.size() * sizeof(typename Container::value_type));
        }

        void seek_to_start();
    };
}