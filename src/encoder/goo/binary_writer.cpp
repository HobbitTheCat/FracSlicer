#include "encoder/goo/binary_writer.h"

#include <cstdint>
#include <ios>
#include <stdexcept>

namespace encoder {

    BinaryWriter::BinaryWriter(const std::string& file_path) {
        this->stream.open(file_path, std::ios::binary);
        if (!this->stream.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + file_path);
        }
    }

    BinaryWriter::~BinaryWriter() {
        if (this->stream.is_open()) this->stream.close();
    }

    void BinaryWriter::write_u8(uint8_t value) {
        this->stream.put(static_cast<char>(value));
    }

    void BinaryWriter::write_u16_be(uint16_t value) {
        uint8_t bytes[2] = {
            static_cast<uint8_t>((value >> 8) & 0xFF),
            static_cast<uint8_t>(value & 0xFF)
        };
        this->stream.write(reinterpret_cast<const char*>(bytes), 2);
    }

    void BinaryWriter::write_u32_be(uint32_t value) {
        uint8_t bytes[4] = {
            static_cast<uint8_t>((value >> 24) & 0xFF),
            static_cast<uint8_t>((value >> 16) & 0xFF),
            static_cast<uint8_t>((value >> 8) & 0xFF),
            static_cast<uint8_t>(value & 0xFF)
        };
        this->stream.write(reinterpret_cast<const char*>(bytes), 4);
    }

    void BinaryWriter::write_f32_be(float value) {
        uint32_t int_val;
        std::memcpy(&int_val, &value, sizeof(float));
        this->write_u32_be(int_val);
    }

    void BinaryWriter::write_bool(bool value) {
        this->write_u8(value ? 1 : 0);
    }

    void BinaryWriter::write_bytes(const uint8_t* data, std::size_t size) {
        this->stream.write(reinterpret_cast<const char*>(data), size);
    }

    void BinaryWriter::seek_to_start() {
        this->stream.seekp(0, std::ios::beg);
        if (this->stream.fail()) throw std::runtime_error("Goo Encoder: Failde to seek to the beginning of the file");
    }
}