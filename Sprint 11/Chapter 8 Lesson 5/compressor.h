#pragma once

// Этот файл содержит компрессор RLE, не меняйте его.
// Можете изучить устройство компрессора, чтобы лучше понять формат сжатого файла.
// Сжатие и разжатие должно давать исходный файл

#include <fstream>
#include <iostream>
#include <string>

class CompressorRLE {
public:
    static const int max_block_size = 128;
    static const int min_repeats_for_special_block = 3;

    CompressorRLE(std::ostream& dst)
        : dst_(dst) {
    }

    void PutChar(char c) {
        if (block_size_ > 0 && c == last_char_) {
            ++repeat_count_;
            if (repeat_count_ >= max_block_size - 1) {
                FinalizeRepeats();
            }
            return;
        }

        FinalizeRepeats();
        AddCharToBlock(c);
    }

    void Finalize() {
        FinalizeRepeats();
        WriteBlock0(block, block_size_);
    }

    size_t GetCompressedSize() const {
        return compressed_size_;
    }

private:
    void FinalizeRepeats() {
        if (repeat_count_ == 0) {
            return;
        }

        // если недостаточно повторов для специального блока
        if (repeat_count_ < min_repeats_for_special_block) {
            for (; repeat_count_ > 0; --repeat_count_) {
                AddCharToBlock(last_char_);
            }
            return;
        }

        // если достаточно повторов
        WriteBlock0(block, block_size_ - 1);
        WriteBlock1(last_char_, repeat_count_ + 1);
        block_size_ = 0;
        repeat_count_ = 0;
    };

    void AddCharToBlock(char c) {
        block[block_size_++] = c;
        if (block_size_ >= max_block_size) {
            WriteBlock0(block, block_size_);
            block_size_ = 0;
        }
        last_char_ = c;
    }

    void WriteBlock0(char* data, int size) {
        if (size == 0) {
            return;
        }

        unsigned char zero = static_cast<unsigned char>(((size - 1) << 1) + 0);
        dst_.put(zero);
        dst_.write(data, size);

        compressed_size_ += 1 + static_cast<size_t>(size);
    }

    void WriteBlock1(char data, int size) {
        if (size == 0) {
            return;
        }

        unsigned char zero = static_cast<unsigned char>(((size - 1) << 1) + 1);
        dst_.put(zero);
        dst_.put(data);

        compressed_size_ += 2;
    }

private:
    std::ostream& dst_;

    size_t compressed_size_ = 0;

    int block_size_ = 0;
    int repeat_count_ = 0;
    char last_char_;

    char block[max_block_size];
};

struct EncodingResult {
    bool opened;
    size_t src_size;
    size_t dst_size;
};

inline EncodingResult EncodeRLE(std::string src_name, std::string dst_name) {
    using namespace std;

    ifstream in(src_name, ios::binary);
    if (!in) {
        return {false, 0, 0};
    }

    ofstream out(dst_name, ios::binary);

    CompressorRLE compressor(out);
    size_t source_size = 0;

    do {
        char buff[1024];
        in.read(buff, sizeof buff);
        size_t read = in.gcount();
        source_size += read;

        for (size_t i = 0; i < read; ++i) {
            compressor.PutChar(buff[i]);
        }
    } while (in);

    compressor.Finalize();

    return {true, source_size, compressor.GetCompressedSize()};
}
