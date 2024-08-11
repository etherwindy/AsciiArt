#include <iostream>
#include <fstream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION  //必须加上
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize2.h"

namespace img_utils {
    // 读取jpg图片转为RGB数组
    void read_img(const char *filename, unsigned char *&data, int &width, int &height, int &channels) {
        // 读取图片
        data = stbi_load(filename, &width, &height, &channels, 0);
        if (data == nullptr) {
            std::cerr << "Read image failed." << std::endl;
            exit(1);
        }

    }

    // RGB数组等比例压缩
    void compress(const unsigned char *data, int width, int height, 
                unsigned char *&compressed_data, const int &max_width, const int &max_height,
                const int &channels, int &compressed_width, int &compressed_height,
                bool half_width = false) {
        // 计算压缩比例
        if (max_width <= 0 || max_height <= 0) {
            std::cerr << "Invalid max width or max height." << std::endl;
            exit(1);
        }

        if (half_width) {
            double scale = std::min(0.5 * max_width / width, 1.0 * max_height / height);
            compressed_width = width * scale * 2;
            compressed_height = height * scale;
        } else {
            double scale = std::min(1.0 * max_width / width, 1.0 * max_height / height);
            compressed_width = width * scale;
            compressed_height = height * scale;
        }

        std::cout << "compressed_width: " << compressed_width << std::endl;
        std::cout << "compressed_height: " << compressed_height << std::endl;

        // 申请压缩后的RGB数组内存
        compressed_data = new unsigned char[compressed_width * compressed_height * channels];

        // 压缩RGB数组
        if (stbir_resize_uint8_linear(data, width, height, 0, compressed_data, compressed_width, compressed_height, 0, stbir_pixel_layout(channels)) == 0) {
            std::cerr << "Compress failed." << std::endl;
            exit(1);
        }
    }
}
