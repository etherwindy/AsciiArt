#include <iostream>
#include <getopt.h>

#include "ascii.hpp"
#include "read_img.hpp"
#include "terminal_utils.hpp"

const char *help_str = 
                        "Usage: asciiArt -f filename -d density -g gamma -c -a\n"
                        "Options:\n"
                        "  -f: jpg or png filename.\n"
                        "  -d: number of grayscale bits, default is 4, max is 4.\n"
                        "  -g: gamma value, default is 1.0.\n"
                        "  -c: enable color.\n"
                        "  -a: enable alpha.\n";

int main(int argc, char *argv[])
{
    // 解析命令行参数
    int opt;
    const char *optstring = "hf:d:g:ca";
    const char *filename = nullptr;
    bool color = false;
    bool alpha = false;
    int density = 4;
    float gamma = 1.0;

    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch (opt)
        {
        case 'h':
            std::cout << help_str << std::endl;
            return 0;
        case 'f':
            filename = optarg;
            break;
        case 'd':
            density = atoi(optarg);
            if (density < 0 || density > 4)
            {
                std::cerr << "Invalid number." << std::endl;
                return 1;
            }
            break;
        case 'g':
            gamma = atof(optarg);
            break;
        case 'c':
            color = true;
            break;
        case 'a':
            alpha = true;
            break;
        default:
            std::cerr << "Use -h to see help." << std::endl;
            return 1;
        }
    }

    // 创建 ASCII 图片生成对象
    ascii::AsciiGenerator ascii_generator(density, gamma, color, alpha);

    // 读取jpg图片转为RGB数组
    unsigned char *data;
    int width, height, channels;
    read::read_img(filename, data, width, height, channels);
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "channels: " << channels << std::endl;

    // 获取终端模拟器的长宽
    int terminal_width, terminal_height;
    terminal_utils::get_size(terminal_width, terminal_height);
    std::cout << "terminal_width: " << terminal_width << std::endl;
    std::cout << "terminal_height: " << terminal_height << std::endl;

    // RGB数组等比例压缩
    unsigned char *compressed_data;
    int compressed_width, compressed_height;
    read::compress(data, width, height, compressed_data, terminal_width, terminal_height,
                    channels, compressed_width, compressed_height);

    // 输出 ASCII 图片
    ascii_generator.print_ascii(compressed_data, compressed_width, compressed_height, channels);
    
    // 等待用户输入
    std::cin.get();

    // 释放内存
    delete[] data;
    delete[] compressed_data;

    return 0;
}