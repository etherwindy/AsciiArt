#pragma once
#include <iostream>
#include <cstring>
#include <cmath>

namespace ascii {
    // 按字符密度给 ASCII 字符排序
    const char* ascii_chars[5] = {"@", " @", " :#@", " :=xnHN@", " .:;!?vxnhVUHNW@"};

    class AsciiGenerator {
    private:
        // 灰度级比特数
        int gray_bits = 4;
        // 是否启用颜色
        bool enable_color = false;
        // 是否启用 alpha 通道
        bool enable_alpha = false;
        // gamma 校正映射表
        unsigned char gamma_table[256];
        // 是否反转灰度
        bool gray_reverse = false;

        // 初始化 gamma 校正映射表
        void init_gamma_table(const float &gamma) {
            if (gamma <= 0.0) {
                std::cerr << "Invalid gamma value" << std::endl;
                exit(1);
            }
            else if (gamma == 1.0)
                for (int i = 0; i < 256; i++)
                    gamma_table[i] = i;
            else
                for (int i = 0; i < 256; i++)
                    gamma_table[i] = (unsigned char)(pow(i / 255.0, gamma) * 255.0);
        }
        
        // 输出带颜色的 ASCII 图片
        void print_ascii_color(const unsigned char *data, const int &width, const int &height, const int &channels) {
            const int gray_levels = pow(2, gray_bits);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int index = i * width + j;
                    int r = data[index * channels + 0];
                    int g = data[index * channels + 1];
                    int b = data[index * channels + 2];
                    int a = (channels > 3) ? data[index * channels + 3] : 0xff;

                    // gamma 校正
                    r = gamma_table[r];
                    g = gamma_table[g];
                    b = gamma_table[b];

                    int gray = (r + g + b) / 3;
                    int ascii_index = (enable_alpha ? a : gray) * gray_levels / 256;
                    std::cout << "\e[38;2;" << r << ";" << g << ";" << b << "m" << ascii_chars[gray_bits][ascii_index];
                }
                std::cout << "\e[0m" << std::endl;
            }
        }

        // 输出灰度 ASCII 图片
        void print_ascii_gray(const unsigned char *data, const int &width, const int &height, const int &channels) {
            const int gray_levels = pow(2, gray_bits);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int index = i * width + j;
                    int r = data[index * channels + 0];
                    int g = data[index * channels + 1];
                    int b = data[index * channels + 2];
                    int a = (channels > 3) ? data[index * channels + 3] : 0xff;

                    r = gamma_table[r];
                    g = gamma_table[g];
                    b = gamma_table[b];

                    int gray = (r + g + b) / 3;
                    int ascii_index = (enable_alpha ? a : gray) * gray_levels / 256;
                    if (gray_reverse)
                        ascii_index = gray_levels - 1 - ascii_index;
                    std::cout << ascii_chars[gray_bits][ascii_index];
                }
                std::cout << std::endl;
            }
        }

        // 通道数小于 3 时输出灰度 ASCII 图片
        void print_ascii_gray_from_gray(const unsigned char *data, const int &width, const int &height, const int &channels) {
            const int gray_levels = pow(2, gray_bits);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int index = i * width + j;
                    int gray = data[index];
                    int a = (channels == 2) ? data[index + 1] : 0xff;
                    int ascii_index = (enable_alpha ? a : gray) * gray_levels / 256;
                    if (gray_reverse)
                        ascii_index = gray_levels - 1 - ascii_index;
                    std::cout << ascii_chars[gray_bits][ascii_index];
                }
                std::cout << std::endl;
            }
        }

    public:
        AsciiGenerator(const int &gray_bits, const float &gamma = 1.0, const bool &enable_color = false, const bool &enable_alpha = false, const bool &gray_reverse = false):
                        enable_color(enable_color), gray_bits(gray_bits), enable_alpha(enable_alpha), gray_reverse(gray_reverse) { init_gamma_table(gamma); } 

        // 输出 ASCII 图片
        void print_ascii(const unsigned char *data, const int &width, const int &height, const int &channels) {
            if (channels < 1 || channels > 4) {
                std::cerr << "Invalid channels" << std::endl;
                exit(1);
            }
            if (width < 1 || height < 1) {
                std::cerr << "Invalid width or height" << std::endl;
                exit(1);
            }
            if (data == nullptr) {
                std::cerr << "Invalid data" << std::endl;
                exit(1);
            }
            std::ios::sync_with_stdio(false);
            if (channels < 3) {
                print_ascii_gray_from_gray(data, width, height, channels);
            }
            else if (enable_color)
                print_ascii_color(data, width, height, channels);
            else
                print_ascii_gray(data, width, height, channels);
            std::ios::sync_with_stdio(true);
        }
    };
}
