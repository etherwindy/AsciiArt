#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <cmath>
#include <fstream>
#include <codecvt>
#include <locale>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#elif __linux__
#include <locale.h>
#endif

namespace unicode_block {

// 终端对 Unicode 盲文字符的支持情况
class unicode_block_status {
private:
    bool unicode_block_enabled = false;
    friend void enable_unicode_block();
    friend void disable_unicode_block();
public:
    bool is_unicode_block_enabled() { return unicode_block_enabled; }
} unicode_block_status;

// 启用 unicdoe 盲文字符输出
void enable_unicode_block() {
    #ifdef _WIN32
    system("CHCP 65001");
    #elif __linux__
    setlocale(LC_ALL, "");
    #endif
    unicode_block_status.unicode_block_enabled = true;
}

// 禁用 unicode 盲文字符输出
void disable_unicode_block() {
    #ifdef _WIN32
    system("CHCP 936");
    #elif __linux__
    setlocale(LC_ALL, "C");
    #endif
    unicode_block_status.unicode_block_enabled = false;
}

// Unicode 盲文字符集
const std::wstring unicode_block = L""
    L"⠀▘▖▌"
    L"▝▀▞▛"
    L"▗▚▄▙"
    L"▐▜▟█";

wchar_t get_block_char(const int &index) { return unicode_block[index]; }

class UnicodeBlockGenerator {
private:
    // 灰度阈值
    int threshold = 256;
    // 是否启用颜色
    bool enable_color = false;
    // 是否启用 alpha 通道
    bool enable_alpha = false;
    // 是否反转灰度
    bool gray_reverse = false;
    // gamma 校正映射表
    unsigned char gamma_table[256];

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

    // 将 2x2 区域的像素根据阈值转为 4 位二进制编码
    inline int get_block_code(const unsigned char *data, const int &width, const int &height, const int &channels,
                                const int &i, const int &j) {
        int block_code = 0;
        for (int k = 1; k >= 0; k--) {
            for (int l = 1; l >= 0; l--) {
                int index = (i * 2 + l) * width + j * 2 + k;
                int r = data[index * channels + 0];
                int g = data[index * channels + 1];
                int b = data[index * channels + 2];
                int a = (channels > 3) ? data[index * channels + 3] : 0xff;
                int gray = enable_alpha ? a : (r + g + b) / 3;
                block_code = (block_code << 1) | (gray > threshold);
            }
        }
        if (gray_reverse)
            block_code = ~block_code & 0xf;
        return block_code;
    }

    inline int get_block_code_from_gray(const unsigned char *data, const int &width, const int &height, const int &channels,
                                const int &i, const int &j) {
        int block_code = 0;
        for (int k = 1; k >= 0; k--) {
            for (int l = 1; l >= 0; l--) {
                int index = (i * 2 + l) * width + j * 2 + k;
                int a = (channels > 3) ? data[index * channels + 3] : 0xff;
                int gray = enable_alpha ? a : data[index * channels + 0];
                block_code = (block_code << 1) | (gray > threshold);
            }
        }
        if (gray_reverse)
            block_code = ~block_code & 0xf;
        return block_code;
    }

    // 获取 4x2 区域的像素平均 RGB 值
    inline void get_block_rgb(const unsigned char *data, const int &width, const int &height, const int &channels,
                                const int &i, const int &j, int &r, int &g, int &b) {
        r = g = b = 0;
        for (int k = 0; k < 2; k++) {
            for (int l = 0; l < 2; l++) {
                int index = (i * 2 + l) * width + j * 2 + k;
                r += data[index * channels + 0];
                g += data[index * channels + 1];
                b += data[index * channels + 2];
            }
        }
        r /= 8;
        g /= 8;
        b /= 8;
    } 

    // 输出不带颜色的 Unicode 盲文字符
    void print_unicode_block_gray(const unsigned char *data, const int &width, const int &height, const int &channels) {
        const int block_width = width / 2;
        const int block_height = height / 2;
        
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        for (int i = 0; i < block_height; i++) {
            for (int j = 0; j < block_width; j++) {
                int block_code = get_block_code(data, width, height, channels, i, j);
                wchar_t block_char = get_block_char(block_code);

                std::cout << converter.to_bytes(block_char);
            }
            std::cout << std::endl;
        }
    }

    // 输出带颜色的 Unicode 盲文字符
    void print_unicode_block_color(const unsigned char *data, const int &width, const int &height, const int &channels) {
        const int block_width = width / 2;
        const int block_height = height / 2;

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        // 同时保存到txt文件，保存时带ansi编码，可以用记事本打开
        // std::ofstream out("output.txt");

        for (int i = 0; i < block_height; i++) {
            for (int j = 0; j < block_width; j++) {
                int r, g, b;
                get_block_rgb(data, width, height, channels, i, j, r, g, b);
                r = gamma_table[r];
                g = gamma_table[g];
                b = gamma_table[b];

                int block_code = get_block_code(data, width, height, channels, i, j);
                wchar_t block_char = get_block_char(block_code);
                std::cout << "\e[38;2;" << r << ";" << g << ";" << b << "m" << converter.to_bytes(block_char);
                // 同时保存到txt文件，保存时带ansi编码，可以用记事本打开
                // out << converter.to_bytes(block_char);
                
            }
            std::cout << "\e[0m" << std::endl;
        }
    }

    // 输出不带颜色的 Unicode 盲文字符
    void print_unicode_block_gray_from_gray(const unsigned char *data, const int &width, const int &height, const int &channels) {
        const int block_width = width / 2;
        const int block_height = height / 2;
        
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        for (int i = 0; i < block_height; i++) {
            for (int j = 0; j < block_width; j++) {
                int block_code = get_block_code_from_gray(data, width, height, channels, i, j);
                wchar_t block_char = get_block_char(block_code);

                std::cout << converter.to_bytes(block_char);
            }
            std::cout << std::endl;
        }
    }

public:
    UnicodeBlockGenerator(const int &threshold, const float &gamma = 1.0, const bool &enable_color = false, const bool &enable_alpha = false, const bool &gray_reverse = false):
                            threshold(threshold), enable_color(enable_color), enable_alpha(enable_alpha), gray_reverse(gray_reverse) { init_gamma_table(gamma); }
    
    // 输出 Unicode 盲文字符
    void print_unicode_block(const unsigned char *data, const int &width, const int &height, const int &channels) {
        if (!unicode_block_status.is_unicode_block_enabled()) {
            std::cout << "Unicode block is not enabled." << std::endl;
            return;
        }
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
        if (channels < 3)
            print_unicode_block_gray(data, width, height, channels);
        else if (enable_color)
            print_unicode_block_color(data, width, height, channels);
        else
            print_unicode_block_gray(data, width, height, channels);
        std::ios::sync_with_stdio(true);
    }
};
}