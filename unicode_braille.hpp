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

namespace unicode_braille
{
    class unicode_braille_status {
    private:
        bool unicode_braille_enabled = false;
        friend void enable_unicode_braille();
        friend void disable_unicode_braille();
    public:
        bool is_unicode_braille_enabled() { return unicode_braille_enabled; }
    } unicode_braille_status;

    // 启用 unicdoe 盲文字符输出
    void enable_unicode_braille() {
        #ifdef _WIN32
        system("CHCP 65001");
        #elif __linux__
        setlocale(LC_ALL, "");
        #endif
        unicode_braille_status.unicode_braille_enabled = true;
    }

    // 禁用 unicode 盲文字符输出
    void disable_unicode_braille() {
        #ifdef _WIN32
        system("CHCP 936");
        #elif __linux__
        setlocale(LC_ALL, "C");
        #endif
        unicode_braille_status.unicode_braille_enabled = false;
    }

    // Unicode 盲文字符集
    const std::wstring unicode_braille = L""
        L"⠀⠁⠂⠃⠄⠅⠆⠇⠈⠉⠊⠋⠌⠍⠎⠏"
        L"⠐⠑⠒⠓⠔⠕⠖⠗⠘⠙⠚⠛⠜⠝⠞⠟"
        L"⠠⠡⠢⠣⠤⠥⠦⠧⠨⠩⠪⠫⠬⠭⠮⠯"
        L"⠰⠱⠲⠳⠴⠵⠶⠷⠸⠹⠺⠻⠼⠽⠾⠿"
        L"⡀⡁⡂⡃⡄⡅⡆⡇⡈⡉⡊⡋⡌⡍⡎⡏"
        L"⡐⡑⡒⡓⡔⡕⡖⡗⡘⡙⡚⡛⡜⡝⡞⡟"
        L"⡠⡡⡢⡣⡤⡥⡦⡧⡨⡩⡪⡫⡬⡭⡮⡯"
        L"⡰⡱⡲⡳⡴⡵⡶⡷⡸⡹⡺⡻⡼⡽⡾⡿"
        L"⢀⢁⢂⢃⢄⢅⢆⢇⢈⢉⢊⢋⢌⢍⢎⢏"
        L"⢐⢑⢒⢓⢔⢕⢖⢗⢘⢙⢚⢛⢜⢝⢞⢟"
        L"⢠⢡⢢⢣⢤⢥⢦⢧⢨⢩⢪⢫⢬⢭⢮⢯"
        L"⢰⢱⢲⢳⢴⢵⢶⢷⢸⢹⢺⢻⢼⢽⢾⢿"
        L"⣀⣁⣂⣃⣄⣅⣆⣇⣈⣉⣊⣋⣌⣍⣎⣏"
        L"⣐⣑⣒⣓⣔⣕⣖⣗⣘⣙⣚⣛⣜⣝⣞⣟"
        L"⣠⣡⣢⣣⣤⣥⣦⣧⣨⣩⣪⣫⣬⣭⣮⣯"
        L"⣰⣱⣲⣳⣴⣵⣶⣷⣸⣹⣺⣻⣼⣽⣾⣿";

    wchar_t get_braille_char(const int &index) { return unicode_braille[index]; }

    class UnicodeBrailleGenerator {
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

        // 将 4x2 区域的像素根据阈值转为 8 位二进制编码
        inline int get_braille_code(const unsigned char *data, const int &width, const int &height, const int &channels,
                                    const int &i, const int &j) {
            int braille_code = 0;
            for (int k = 1; k >= 0; k--) {
                int index = (i * 4 + 3) * width + j * 2 + k;
                int r = data[index * channels + 0];
                int g = data[index * channels + 1];
                int b = data[index * channels + 2];
                int a = (channels > 3) ? data[index * channels + 3] : 0xff;
                int gray = enable_alpha ? a : (r + g + b) / 3;
                braille_code = (braille_code << 1) | (gray > threshold);
            }
            for (int k = 1; k >= 0; k--) {
                for (int l = 2; l >= 0; l--) {
                    int index = (i * 4 + l) * width + j * 2 + k;
                    int r = data[index * channels + 0];
                    int g = data[index * channels + 1];
                    int b = data[index * channels + 2];
                    int a = (channels > 3) ? data[index * channels + 3] : 0xff;
                    int gray = enable_alpha ? a : (r + g + b) / 3;
                    braille_code = (braille_code << 1) | (gray > threshold);
                }
            }
            if (gray_reverse)
                braille_code = ~braille_code & 0xff;
            return braille_code;
        }

        inline int get_braille_code_from_gray(const unsigned char *data, const int &width, const int &height, const int &channels,
                                    const int &i, const int &j) {
            int braille_code = 0;
            for (int k = 1; k >= 0; k--) {
                int index = (i * 4 + 3) * width + j * 2 + k;
                int a = (channels > 3) ? data[index * channels + 3] : 0xff;
                int gray = enable_alpha ? a : data[index * channels + 0];
                braille_code = (braille_code << 1) | (gray > threshold);
            }
            for (int k = 1; k >= 0; k--) {
                for (int l = 2; l >= 0; l--) {
                    int index = (i * 4 + l) * width + j * 2 + k;
                    int a = (channels > 3) ? data[index * channels + 3] : 0xff;
                    int gray = enable_alpha ? a : data[index * channels + 0];
                    braille_code = (braille_code << 1) | (gray > threshold);
                }
            }
            if (gray_reverse)
                braille_code = ~braille_code & 0xff;
            return braille_code;
        }

        // 获取 4x2 区域的像素平均 RGB 值
        inline void get_braille_rgb(const unsigned char *data, const int &width, const int &height, const int &channels,
                                    const int &i, const int &j, int &r, int &g, int &b) {
            r = g = b = 0;
            for (int k = 0; k < 2; k++) {
                for (int l = 0; l < 4; l++) {
                    int index = (i * 4 + l) * width + j * 2 + k;
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
        void print_unicode_braille_gray(const unsigned char *data, const int &width, const int &height, const int &channels) {
            const int braille_width = width / 2;
            const int braille_height = height / 4;
            
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

            for (int i = 0; i < braille_height; i++) {
                for (int j = 0; j < braille_width; j++) {
                    int braille_code = get_braille_code(data, width, height, channels, i, j);
                    wchar_t braille_char = get_braille_char(braille_code);

                    std::cout << converter.to_bytes(braille_char);
                }
                std::cout << std::endl;
            }
        }

        // 输出带颜色的 Unicode 盲文字符
        void print_unicode_braille_color(const unsigned char *data, const int &width, const int &height, const int &channels) {
            const int braille_width = width / 2;
            const int braille_height = height / 4;

            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            // 同时保存到txt文件，保存时带ansi编码，可以用记事本打开
            std::ofstream out("output.txt");

            for (int i = 0; i < braille_height; i++) {
                for (int j = 0; j < braille_width; j++) {
                    int r, g, b;
                    get_braille_rgb(data, width, height, channels, i, j, r, g, b);
                    r = gamma_table[r];
                    g = gamma_table[g];
                    b = gamma_table[b];

                    int braille_code = get_braille_code(data, width, height, channels, i, j);
                    wchar_t braille_char = get_braille_char(braille_code);
                    std::cout << "\e[38;2;" << r << ";" << g << ";" << b << "m" << converter.to_bytes(braille_char);
                    // 同时保存到txt文件，保存时带ansi编码，可以用记事本打开
                    out << converter.to_bytes(braille_char);
                    
                }
                std::cout << "\e[0m" << std::endl;
            }
        }

        // 输出不带颜色的 Unicode 盲文字符
        void print_unicode_braille_gray_from_gray(const unsigned char *data, const int &width, const int &height, const int &channels) {
            const int braille_width = width / 2;
            const int braille_height = height / 4;
            
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

            for (int i = 0; i < braille_height; i++) {
                for (int j = 0; j < braille_width; j++) {
                    int braille_code = get_braille_code_from_gray(data, width, height, channels, i, j);
                    wchar_t braille_char = get_braille_char(braille_code);

                    std::cout << converter.to_bytes(braille_char);
                }
                std::cout << std::endl;
            }
        }

    public:
        UnicodeBrailleGenerator(const int &threshold, const float &gamma = 1.0, const bool &enable_color = false, const bool &enable_alpha = false, const bool &gray_reverse = false):
                                threshold(threshold), enable_color(enable_color), enable_alpha(enable_alpha), gray_reverse(gray_reverse) { init_gamma_table(gamma); }
        
        // 输出 Unicode 盲文字符
        void print_unicode_braille(const unsigned char *data, const int &width, const int &height, const int &channels) {
            if (!unicode_braille_status.is_unicode_braille_enabled()) {
                std::cout << "Unicode braille is not enabled." << std::endl;
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
                print_unicode_braille_gray(data, width, height, channels);
            else if (enable_color)
                print_unicode_braille_color(data, width, height, channels);
            else
                print_unicode_braille_gray(data, width, height, channels);
            std::ios::sync_with_stdio(true);
        }
    };
}