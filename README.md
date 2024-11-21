
# AsciiArt -- An ASCII Art Generator

🎨 Generate ASCII character drawings from JPG or PNG images.

## Table of Contents

- [AsciiArt -- An ASCII Art Generator](#asciiart----an-ascii-art-generator)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [Build Instructions](#build-instructions)
    - [Windows](#windows)
    - [Linux](#linux)
  - [Usage](#usage)
    - [Options](#options)
  - [License](#license)

## Features

- Convert images to ASCII art or Braille representation.
- Adjustable width and height for output.
- Support for color and alpha blending.
- Customizable grayscale and threshold settings.

## Build Instructions

### Windows

To build the project on Windows, follow these steps:

```sh
mkdir build
cd .\build\
cmake ..
mingw32-make.exe
```

### Linux

To build the project on Linux, use the following commands:

```sh
mkdir build
cd ./build
cmake ..
make
```

## Usage

Run the ASCII Art generator with the following commands.

Linux:

```sh
asciiart -F filename [options]
```

Windows:

```sh
asciiart.exe -F filename [option]
```

### Options

| Option | Description |
|--------|-------------|
| `-F`   | Input filename (JPEG, PNG, or JFIF). |
| `-W`   | Max width (default is terminal width). |
| `-H`   | Max height (default is terminal height). |
| `-S`   | Output style: `ascii`, `braille` or 'block' (default is `ascii`). |
| `-N`   | Number of grayscale bits (default is 4, max is 4 for ASCII style only). |
| `-T`   | Threshold value (default is 128 for Braille and Block style only). |
| `-G`   | Gamma value (default is 1.0). |
| `-c`   | Enable color output. |
| `-a`   | Enable alpha blending. |
| `-r`   | Enable gray reverse. |

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.