#include "decoder.h"
#include "png_encoder.h"
#include <string>


void DecodeAndWriteToPng(const std::string& filename) {
    auto image = Decode(filename);
    auto dot_pos = filename.find("jpg") - 1;
    std::string png_filename(filename.substr(0, dot_pos) + ".png");
    WritePng(png_filename, image);
}


int main() {

    for (const std::string& image_name : {"colors.jpg", "bad_quality.jpg", "chroma_halfed.jpg",
                                   "grayscale.jpg", "lenna.jpg", "save_for_web.jpg", "small.jpg",
                                   "test.jpg", "tiny.jpg"}) {

        DecodeAndWriteToPng("../jpeg-examples/" + image_name);
    }

    return 0;
}

