#pragma once

#include <fstream>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include "bit_reader.h"
#include "fftw3.h"
#include "huffman.h"
#include "image.h"
#include "matrix.h"

Image Decode(const std::string& filename);

struct HuffmanTableInfo {
    std::string type;  // AC / DC
    int id;

    bool operator<(const HuffmanTableInfo& other) const {
        return type + std::to_string(id) <
               other.type + std::to_string(other.id);
    }

    bool operator==(const HuffmanTableInfo& other) const {
        return id == other.id && type == other.type;
    }
};

struct Component {
    int vert_thin;
    int hor_thin;
    int quantum_table_id;
    HuffmanTableInfo huffman_dc, huffman_ac;
    Matrix<double> matrix;
};

enum class SectionTitle {
    SOI,
    COM,
    DHT,
    DQT,
    SOF0,
    SOS,
    EOI,
    APPn,
    NOT_A_SECTION
};

struct Section {
    SectionTitle title;
    int len;
};

class JpgDecoder {
public:
    explicit JpgDecoder(std::istream& in_stream)
        : in_stream_(in_stream),
          in_buffer_(nullptr, fftw_free),
          out_buffer_(nullptr, fftw_free),
          fourier_plan_(nullptr, fftw_destroy_plan) {}

    void ProcessImage();

    Image RGBImage() const;

    static const std::unordered_map<int, SectionTitle> kSectionTitles;
    static const int kBlockSize = 8;
    static const int kHuffmanMaxLength = 16;

private:
    std::istream& in_stream_;
    int height_, width_;
    std::unordered_map<int, Matrix<int>> quantum_tables_;
    std::map<HuffmanTableInfo, HuffmanDecoder> huffman_decoders_;
    int components_num_;
    int max_hor_thin_ = 1, max_vert_thin_ = 1;
    std::unordered_map<int, Component> components_;
    std::string comment_;

    std::unique_ptr<double, decltype(&fftw_free)> in_buffer_, out_buffer_;
    std::unique_ptr<fftw_plan_s, decltype(&fftw_destroy_plan)> fourier_plan_;

    Matrix<double> GetOneBlock(BitReader* reader, const Component& component,
                               int* prev_dc_coef);

    uint8_t ReadOne(int* len = nullptr);
    uint16_t ReadTwo(int* len = nullptr);

    Section NextSection();

    void ParseCOM(int len);
    void ParseDHT(int len);
    void ParseDQT(int len);
    void ParseSOF0(int len);

    void ParseSOSHeader(int len);

    void ValidCheck() const;

    void ProcessSOS();

    void CreateFourierPlan();
};
