#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <array>

using namespace std;

struct Nucleotide {
    char symbol;
    size_t position;
    int chromosome_num;
    int gene_num;
    bool is_marked;
    char service_info;
};

const static int N = 4;
const std::array<char, N> Letters = { 'A', 'T', 'G', 'C' };

struct CompactNucleotide {
    // напишите вашу реализацию здесь
    uint32_t position;
    uint16_t gene_num : 15;
    uint16_t is_marked : 1;
    uint16_t service_info : 8;
    uint16_t symbol : 2;
    uint16_t chromosome_num : 6;
};

CompactNucleotide Compress(const Nucleotide& n) {
    // напишите вашу реализацию здесь
    CompactNucleotide result;
    result.position = static_cast<uint32_t>(n.position);
    result.gene_num = n.gene_num;
    result.is_marked = n.is_marked;
    result.service_info = n.service_info;
    for (size_t i = 0; i < N; ++i) {
        if (n.symbol == Letters[i]) {
            result.symbol = i;
            continue;
        }
    }
    result.chromosome_num = n.chromosome_num;

    return result;
}

Nucleotide Decompress(const CompactNucleotide& cn) {
    // напишите вашу реализацию здесь
    Nucleotide result;
    result.symbol = Letters[cn.symbol];
    result.position = cn.position;
    result.is_marked = cn.is_marked;
    result.gene_num = cn.gene_num;
    result.chromosome_num = cn.chromosome_num;
    result.service_info = cn.service_info;

    return result;
}

static_assert(sizeof(CompactNucleotide) <= 8, "Your CompactNucleotide is not compact enough"s);
static_assert(alignof(CompactNucleotide) == 4, "Don't use '#pragma pack'!"s);
bool operator==(const Nucleotide& lhs, const Nucleotide& rhs) {
    return (lhs.symbol == rhs.symbol) && (lhs.position == rhs.position) && (lhs.chromosome_num == rhs.chromosome_num)
        && (lhs.gene_num == rhs.gene_num) && (lhs.is_marked == rhs.is_marked) && (lhs.service_info == rhs.service_info);
}
void TestSize() {
    assert(sizeof(CompactNucleotide) <= 8);
}
void TestCompressDecompress() {
    Nucleotide source;
    source.symbol = 'T';
    source.position = 1'000'000'000;
    source.chromosome_num = 48;
    source.gene_num = 1'000;
    source.is_marked = true;
    source.service_info = '!';

    CompactNucleotide compressedSource = Compress(source);
    Nucleotide decompressedSource = Decompress(compressedSource);

    assert(source == decompressedSource);
}

int main() {
    TestSize();
    TestCompressDecompress();
}
