#include "decoder.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include "errors.h"
#include "fourier.h"
#include "tricks.h"

const std::unordered_map<int, SectionTitle> JpgDecoder::kSectionTitles = {
    {0xffe0, SectionTitle::APPn},
    {0xffe1, SectionTitle::APPn},
    {0xffe2, SectionTitle::APPn},
    {0xffe3, SectionTitle::APPn},
    {0xffe4, SectionTitle::APPn},
    {0xffe5, SectionTitle::APPn},
    {0xffe6, SectionTitle::APPn},
    {0xffe7, SectionTitle::APPn},
    {0xffe8, SectionTitle::APPn},
    {0xffe9, SectionTitle::APPn},
    {0xffea, SectionTitle::APPn},
    {0xffeb, SectionTitle::APPn},
    {0xffec, SectionTitle::APPn},
    {0xffed, SectionTitle::APPn},
    {0xffee, SectionTitle::APPn},
    {0xffef, SectionTitle::APPn},
    {0xffd8, SectionTitle::SOI},
    {0xfffe, SectionTitle::COM},
    {0xffc4, SectionTitle::DHT},
    {0xffdb, SectionTitle::DQT},
    {0xffc0, SectionTitle::SOF0},
    {0xffda, SectionTitle::SOS},
    {0xffd9, SectionTitle::EOI}};

uint8_t JpgDecoder::ReadOne(int* len) {
    Expect(in_stream_.peek() != EOF, "File suddenly ended");
    uint8_t byte = in_stream_.get();
    if (len) {
        --(*len);
    }
    return byte;
}

uint16_t JpgDecoder::ReadTwo(int* len) {
    uint16_t byte_pair = (static_cast<uint16_t>(ReadOne(len)) << 8) +
                         static_cast<uint16_t>(ReadOne(len));
    return byte_pair;
}

Section JpgDecoder::NextSection() {
    unsigned int code = in_stream_.get();
    code = (code << 8) + in_stream_.peek();

    SectionTitle title;
    try {
        title = kSectionTitles.at(code);
        in_stream_.get();
    } catch (const std::out_of_range&) {
        in_stream_.unget();
        return {SectionTitle::NOT_A_SECTION, 0};
    }

    if (title == SectionTitle::SOI || title == SectionTitle::EOI) {
        return {title, 0};
    }
    int len = ReadTwo();
    Expect(len > 2, "Section length is not valid");
    return {title, len - 2};
}

void JpgDecoder::ProcessImage() {
    Expect(NextSection().title == SectionTitle::SOI,
           "No SOI marker in the start of file");

    Section section;
    do {
        section = NextSection();
        switch (section.title) {
            case SectionTitle::APPn:
                in_stream_.ignore(section.len);
                break;
            case SectionTitle::COM:
                ParseCOM(section.len);
                break;
            case SectionTitle::DQT:
                ParseDQT(section.len);
                break;
            case SectionTitle::DHT:
                ParseDHT(section.len);
                break;
            case SectionTitle::SOF0:
                ParseSOF0(section.len);
                break;
            case SectionTitle::SOS:
                ParseSOSHeader(section.len);
                break;
            default:
                throw std::runtime_error("No section marker in expected place");
        }
    } while (section.title != SectionTitle::SOS);

    ValidCheck();

    ProcessSOS();

    Expect(NextSection().title == SectionTitle::EOI,
           "No EOI marker in the end of scan");
}

void JpgDecoder::ParseCOM(int len) {
    while (len != 0) {
        char buf = in_stream_.get();
        --len;
        comment_ += buf;
    }
}

void JpgDecoder::ParseDHT(int len) {
    while (len != 0) {
        uint8_t info_byte = ReadOne(&len);

        std::string type;
        switch (info_byte >> 4) {
            case 0:
                type = "DC";
                break;
            case 1:
                type = "AC";
                break;
            default:
                throw std::runtime_error("Wrong huffman table type");
        }

        int id = info_byte & 0xf;
        HuffmanTableInfo table_info{type, id};
        Expect(huffman_decoders_.find(table_info) == huffman_decoders_.end(),
               "Multiple definition of huffman table");

        std::vector<int> codes_number;
        codes_number.reserve(kHuffmanMaxLength);
        size_t total_values = 0;
        for (size_t ind = 0; ind < kHuffmanMaxLength; ++ind) {
            codes_number.push_back(ReadOne(&len));
            total_values += codes_number[ind];
        }
        std::vector<uint8_t> codes_values;
        codes_values.reserve(total_values);
        for (size_t ind = 0; ind < total_values; ++ind) {
            codes_values.push_back(ReadOne(&len));
        }
        huffman_decoders_[table_info].BuildTree(codes_number, codes_values);
    }
}

void JpgDecoder::ParseDQT(int len) {
    while (len != 0) {
        uint8_t info_byte = ReadOne(&len);

        int values_len;
        switch (info_byte >> 4) {
            case 0:
                values_len = 1;
                break;
            case 1:
                values_len = 2;
                break;
            default:
                throw std::runtime_error("Not valid DQT values size");
        }

        int table_id = info_byte & 0xf;

        std::vector<int> table_values;
        table_values.reserve(kBlockSize * kBlockSize);
        for (size_t ind = 0; ind < kBlockSize * kBlockSize; ++ind) {
            int value = (values_len == 1) ? ReadOne(&len) : ReadTwo(&len);
            table_values.push_back(value);
        }
        quantum_tables_[table_id] = ZigzagMatrix<int>(table_values, kBlockSize);
    }
}

void JpgDecoder::ParseSOF0(int len) {
    // pass the precision, because it's
    // always 8 bit in baseline
    ReadOne(&len);

    height_ = ReadTwo(&len);
    width_ = ReadTwo(&len);
    Expect(height_ > 0 && width_ > 0, "Image size is not valid");

    int components_num = ReadOne(&len);
    Expect(components_num == 3 || components_num == 1,
           "Wrong number of components");

    while (len != 0) {
        int component_id = ReadOne(&len);
        Expect(component_id >= 1 && component_id <= 3, "Wrong component id");
        Expect(components_.find(component_id) == components_.end(),
               "Duplicate of component info");

        uint8_t thin_byte = ReadOne(&len);
        int hor_thin = thin_byte >> 4;
        Expect(hor_thin == 1 || hor_thin == 2, "Not valid horizontal thinning");
        max_hor_thin_ = std::max(max_hor_thin_, hor_thin);
        int vert_thin = thin_byte & 0xf;
        Expect(vert_thin == 1 || vert_thin == 2, "Not valid vertical thinning");
        max_vert_thin_ = std::max(max_vert_thin_, vert_thin);
        int quantum_table_id = ReadOne(&len);
        components_[component_id] = {vert_thin, hor_thin, quantum_table_id};
        --components_num;
        Expect(components_num >= 0, "Wrong declared len of SOF0 section");
    }
    Expect(!components_num, "Wrong components number");
}

void JpgDecoder::ParseSOSHeader(int len) {
    components_num_ = ReadOne(&len);
    Expect(components_num_ == components_.size(),
           "Numbers of components doesn't match");

    for (int ind = 0; ind < components_num_; ++ind) {
        int component_id = ReadOne(&len);
        Expect(components_.find(component_id) != components_.end(),
               "Wrong component id in SOS section");
        uint8_t huffman_byte = ReadOne(&len);
        int dc_id = huffman_byte >> 4;
        Expect(huffman_decoders_.find({"DC", dc_id}) != huffman_decoders_.end(),
               "Wrong DC huffman id in SOS section");
        int ac_id = huffman_byte & 0xf;
        Expect(huffman_decoders_.find({"AC", ac_id}) != huffman_decoders_.end(),
               "Wrong AC huffman id in SOS section");
        components_.at(component_id).huffman_ac = {"AC", ac_id};
        components_.at(component_id).huffman_dc = {"DC", dc_id};
    }

    in_stream_.ignore(1);
    Expect(ReadOne() == 0x3f, "Yeah, very funny");
    in_stream_.ignore(1);
    len -= 3;
    Expect(!len, "Not valid len of SOS section");
}

int Signed(int number, int declared_len) {
    if ((number >> (declared_len - 1)) == 0) {
        number = -((~number) & ((1 << declared_len) - 1));
    }
    return number;
}

Matrix<double> JpgDecoder::GetOneBlock(BitReader* reader,
                                       const Component& component,
                                       int* prev_dc_coef) {
    std::vector<double> values(kBlockSize * kBlockSize);

    // first read adding to DC coefficient
    auto& dc_huffman = huffman_decoders_.at(component.huffman_dc);
    uint8_t dc_code = dc_huffman.DecodeSymbol(reader);
    values[0] = *prev_dc_coef;
    if (dc_code != 0) {
        int declared_len = dc_code & 0xf;
        int dc_add = Signed(reader->Get(declared_len), declared_len);
        values[0] += dc_add;
        *prev_dc_coef = values[0];
    }

    // now goes AC coefficients
    auto& ac_huffman = huffman_decoders_.at(component.huffman_ac);
    int ind = 1;
    while (ind < kBlockSize * kBlockSize) {
        uint8_t ac_code = ac_huffman.DecodeSymbol(reader);
        if (ac_code == 0) {
            break;
        }
        if (ac_code == 0xf0) {
            ind += 16;
        } else {
            int zeros_before = ac_code >> 4;
            Expect(zeros_before < 16, "Incorrect pair encoding");
            ind += zeros_before;  // shift on first-half-byte zeros
            Expect(ind < kBlockSize * kBlockSize,
                   "Too much coefficients for one block");
            int declared_len = ac_code & 0xf;
            values[ind] = Signed(reader->Get(declared_len), declared_len);
            ++ind;
        }
    }

    // zig-zag
    Matrix<double> block = ZigzagMatrix(values, kBlockSize);

    // quantum
    ElemMultiply<double>(&block,
                         quantum_tables_.at(component.quantum_table_id));

    // fourier
    return DCT(block, fourier_plan_.get(), in_buffer_.get(), out_buffer_.get());
}

void JpgDecoder::ValidCheck() const {
    for (const auto & [ id, component ] : components_) {
        Expect(quantum_tables_.find(component.quantum_table_id) !=
                   quantum_tables_.end(),
               "Wrong quantum table id");
        Expect(huffman_decoders_.find(component.huffman_dc) !=
                       huffman_decoders_.end() &&
                   huffman_decoders_.find(component.huffman_ac) !=
                       huffman_decoders_.end(),
               "Wrong huffman table info");
    }
}

void JpgDecoder::CreateFourierPlan() {
    size_t buffer_size = kBlockSize * kBlockSize * sizeof(double);
    in_buffer_.reset((double*)fftw_malloc(buffer_size));
    out_buffer_.reset((double*)fftw_malloc(buffer_size));
    auto kind = FFTW_REDFT01;
    fourier_plan_.reset(fftw_plan_r2r_2d(kBlockSize, kBlockSize,
                                         in_buffer_.get(), out_buffer_.get(),
                                         kind, kind, FFTW_MEASURE));
}

int IncreaseToDivisible(int what, int by) {
    while (what % by != 0) {
        ++what;
    }
    return what;
}

struct Corner {
    int row = 0;
    int col = 0;
};

void JpgDecoder::ProcessSOS() {
    int mcu_width = kBlockSize * max_hor_thin_;
    int mcu_height = kBlockSize * max_vert_thin_;
    int width_ext = IncreaseToDivisible(width_, mcu_width);
    int height_ext = IncreaseToDivisible(height_, mcu_height);

    int mcu_per_row = width_ext / mcu_width;
    int mcu_per_col = height_ext / mcu_height;

    // create 1 or 3 matrices for components
    for (auto & [ id, component ] : components_) {
        int cur_width = width_ext * component.hor_thin / max_hor_thin_;
        int cur_height = height_ext * component.vert_thin / max_vert_thin_;
        component.matrix.Resize(cur_height, cur_width);
    }

    // create fourier plan
    CreateFourierPlan();

    // now start scan
    BitReader reader(in_stream_);
    reader.SetSkipRule(0xff, 0x00);

    // last dc coef to remember for each component
    std::vector<int> last_dc(components_num_);
    std::vector<Corner> start_corners(components_num_);

    // you don't have to understand it
    for (int mcu_row = 0; mcu_row < mcu_per_col; ++mcu_row) {
        for (auto& corner : start_corners) {
            corner.col = 0;
        }
        for (int mcu_col = 0; mcu_col < mcu_per_row; ++mcu_col) {
            for (int comp_id = 1; comp_id <= components_num_; ++comp_id) {
                auto& cur_component = components_.at(comp_id);
                auto& cur_corner = start_corners[comp_id - 1];
                int row_shift = 0, col_shift = 0;
                for (int magic_var :
                     {2, cur_component.hor_thin, cur_component.vert_thin,
                      cur_component.hor_thin * cur_component.vert_thin / 2}) {
                    if (magic_var == 2) {
                        auto block = GetOneBlock(&reader, cur_component,
                                                 &last_dc[comp_id - 1]);
                        PutIn<double>(block, &cur_component.matrix,
                                      cur_corner.row + row_shift,
                                      cur_corner.col + col_shift);
                    }
                    col_shift = (col_shift + kBlockSize) % (2 * kBlockSize);
                    row_shift = std::max(kBlockSize - col_shift, row_shift);
                }
                cur_corner.col += kBlockSize * cur_component.hor_thin;
            }
        }
        for (int comp_id = 1; comp_id <= components_num_; ++comp_id) {
            start_corners[comp_id - 1].row +=
                components_.at(comp_id).vert_thin * kBlockSize;
        }
    }
}

int Bound(int number) { return std::min(std::max(0, number), 255); }

RGB ToRGB(double y_component, double cb_component, double cr_component) {
    return {Bound(y_component + 1.402 * cr_component + 128),
            Bound(y_component - 0.34414 * cb_component -
                  0.71414 * cr_component + 128),
            Bound(y_component + 1.772 * cb_component + 128)};
}

RGB ToRGB(double y_component) { return ToRGB(y_component, 0, 0); }

Image JpgDecoder::RGBImage() const {
    Image image(width_, height_);
    for (int row = 0; row < height_; ++row) {
        for (int col = 0; col < width_; ++col) {
            RGB pixel;
            if (components_num_ == 1) {
                pixel = ToRGB(components_.at(1).matrix(row, col));
            } else {
                pixel = ToRGB(components_.at(1).matrix(row, col),
                              components_.at(2).matrix(row / max_vert_thin_,
                                                       col / max_hor_thin_),
                              components_.at(3).matrix(row / max_vert_thin_,
                                                       col / max_hor_thin_));
            }
            image.SetPixel(row, col, pixel);
        }
    }
    image.SetComment(comment_);
    return image;
}

Image Decode(const std::string& filename) {
    std::ifstream stream(filename, std::ios_base::binary);
    JpgDecoder decoder(stream);
    decoder.ProcessImage();
    return decoder.RGBImage();
}
