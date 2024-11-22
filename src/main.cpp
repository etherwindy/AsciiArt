#include <cstring>
#include <getopt.h>
#include <iostream>

#include "ascii.hpp"
#include "img_utils.hpp"
#include "terminal_utils.hpp"
#include "unicode_block.hpp"
#include "unicode_braille.hpp"

const char *help_str =
    "Usage: asciiArt -F filename [options]\n"
    "Options:\n"
    " -F: jpg, png or jfif filename.\n"
    " -W: max width, default is terminal width.\n"
    " -H: max height, default is terminal height.\n"
    " -S: style, 'ascii', 'braille' or 'block', default is 'ascii'.\n"
    " -N: number of grayscale bits, default is 4, max is 4 (ascii style "
    "only).\n"
    " -C: threshold value of gray level, default is 128 (braille and block "
    "style only).\n"
    " -A: threshold value of alpha, default is 128 (braille and block style "
    "only).\n"
    " -G: gamma value, default is 1.0.\n"
    " -c: enable color.\n"
    " -a: enable alpha (transparency).\n"
    " -r: enable gray reverse.\n";

int main(int argc, char *argv[]) {
    // 解析命令行参数
    const char *filename = nullptr;
    const char *style    = "ascii";
    int grayscale        = 4;
    float gamma          = 1.0;
    int threshold_gray   = 128;
    int threshold_alpha  = 128;
    bool color           = false;
    bool alpha           = false;
    bool color_decode    = false;
    bool gray_reverse    = false;

    bool set_size = false;
    int max_width = 0xfff, max_height = 0xfff;

    int opt;
    const char *optstring = "hF:W:H:S:N:C:A:G:caer";
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h':
            std::cout << help_str << std::endl;
            return 0;
        case 'F':
            filename = optarg;
            break;
        case 'W':
            max_width = atoi(optarg);
            set_size  = true;
            break;
        case 'H':
            max_height = atoi(optarg);
            set_size   = true;
            break;
        case 'S':
            style = optarg;
            break;
        case 'N':
            grayscale = atoi(optarg);
            if (grayscale < 0 || grayscale > 4) {
                std::cerr << "Invalid number." << std::endl;
                return 1;
            }
            break;
        case 'C':
            threshold_gray = atoi(optarg);
            break;
        case 'A':
            threshold_alpha = atoi(optarg);
            break;
        case 'G':
            gamma = atof(optarg);
            break;
        case 'c':
            color = true;
            break;
        case 'a':
            alpha = true;
            break;
        case 'e':
            color_decode = true;
            break;
        case 'r':
            gray_reverse = true;
            break;
        default:
            std::cerr << "Use -h to see help." << std::endl;
            return 1;
        }
    }

    // 读取jpg图片转为RGB数组
    unsigned char *data;
    int width, height, channels;
    img_utils::read_img(filename, data, width, height, channels);

    // 读取终端模拟器的长宽
    if (!set_size)
        terminal_utils::get_size(max_width, max_height);

    unsigned char *compressed_data;
    int compressed_width, compressed_height;

    if (strcmp(style, "ascii") == 0) {
        // 创建 ASCII 图片生成对象
        ascii::AsciiGenerator ascii_generator(grayscale, gamma, color, alpha,
                                              gray_reverse);

        // RGB数组等比例压缩
        img_utils::compress(data, width, height, compressed_data, max_width,
                            max_height, channels, compressed_width,
                            compressed_height, true);

        // 输出 ASCII 图片
        ascii_generator.print_ascii(compressed_data, compressed_width,
                                    compressed_height, channels);

    } else if (strcmp(style, "braille") == 0) {
        // 创建 Unicode 盲文图片生成对象
        unicode_braille::UnicodeBrailleGenerator unicode_braille_generator(
            threshold_gray, threshold_alpha, gamma, color, alpha, gray_reverse);

        // RGB数组等比例压缩
        img_utils::compress(data, width, height, compressed_data, max_width * 2,
                            max_height * 4, channels, compressed_width,
                            compressed_height);

        // 输出 Unicode 盲文图片
        unicode_braille::enable_unicode_braille();
        unicode_braille_generator.print_unicode_braille(
            compressed_data, compressed_width, compressed_height, channels);

    } else if (strcmp(style, "block") == 0) {
        // 创建 Unicode 方块图片生成对象
        unicode_block::UnicodeBlockGenerator unicode_block_generator(
            threshold_gray, threshold_alpha, gamma, color, alpha, gray_reverse);

        // RGB数组等比例压缩
        img_utils::compress(data, width, height, compressed_data, max_width * 2,
                            max_height * 2, channels, compressed_width,
                            compressed_height, true);

        // 输出 Unicode 方块图片
        unicode_block::enable_unicode_block();
        unicode_block_generator.print_unicode_block(
            compressed_data, compressed_width, compressed_height, channels);

    } else {
        std::cerr << "Invalid style." << std::endl;
        return 1;
    }

    // 释放内存
    delete[] compressed_data;
    delete[] data;

    return 0;
}
