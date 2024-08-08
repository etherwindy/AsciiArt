#include <iostream>
#include <cstring>
#include <cmath>

namespace ascii
{
    class AsciiGenerator
    {
    private:
        // 灰度级比特数
        int gray_bits = 4;
        // 是否启用颜色
        bool enable_color = false;
        // 是否启用 alpha 通道
        bool enable_alpha = false;
        // 按字符密度给 ASCII 字符排序
        const char* ascii_chars[5] = {"@", " @", " :#@", " :=xnHN@", " .:;!?vxnhVUHNW@"};
        // gamma 校正映射表
        unsigned char gamma_table[256];

        // 初始化 gamma 校正映射表
        void init_gamma_table(const float &gamma)
        {
            if (gamma <= 0.0)
            {
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
        void print_ascii_color(const unsigned char *data, const int &width, const int &height, const int &channels)
        {
            const int gray_levels = pow(2, gray_bits);
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    int index = i * width + j;
                    int r = data[index * channels + 0];
                    int g = data[index * channels + 1];
                    int b = data[index * channels + 2];
                    int a = (channels > 3) ? data[index * channels + 3] : 0xff;

                    // gamma 校正
                    r = gamma_table[r];
                    g = gamma_table[g];
                    b = gamma_table[b];

                    int grey = (r + g + b) / 3;
                    int ascii_index = (enable_alpha ? a : grey) * gray_levels / 256;
                    std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m" << ascii_chars[gray_bits][ascii_index] << "\033[0m";
                }
                std::cout << std::endl;
            }
        }

        // 输出灰度 ASCII 图片
        void print_ascii_gray(const unsigned char *data, const int &width, const int &height, const int &channels)
        {
            const int gray_levels = pow(2, gray_bits);
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    int index = i * width + j;
                    int r = data[index * channels + 0];
                    int g = data[index * channels + 1];
                    int b = data[index * channels + 2];

                    // gamma 校正
                    r = gamma_table[r];
                    g = gamma_table[g];
                    b = gamma_table[b];

                    int gray = (r + g + b) / 3;
                    int ascii_index = gray * gray_levels / 256;
                    std::cout << ascii_chars[gray_bits][ascii_index];
                }
                std::cout << std::endl;
            }
        }

    public:
        AsciiGenerator(const int &gray_bits, const float &gamma, const bool &enable_color, const bool &enable_alpha): enable_color(enable_color), gray_bits(gray_bits), enable_alpha(enable_alpha)
        {
            init_gamma_table(gamma);
        } 

        // 输出 ASCII 图片
        void print_ascii(const unsigned char *data, const int &width, const int &height, const int &channels)
        {
            std::ios::sync_with_stdio(false);
            if (enable_color)
                print_ascii_color(data, width, height, channels);
            else
                print_ascii_gray(data, width, height, channels);
            std::ios::sync_with_stdio(true);
        }
    };
}
