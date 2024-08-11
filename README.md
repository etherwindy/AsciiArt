# Ascii Art

Generate ascii character drawings from jpg or png images.

## Build

### Windows

```posh
mkdir build
cd .\build\
cmake ..
mingw32-make.exe
```

### Linux

```bash
mkdir build
cd ./build
cmake ..
make
```

## Usage

```bash
Usage: asciiArt -F filename [options]
Options:
 -F: jpg, png or jfif filename.
 -W: max width, default is terminal width.
 -H: max height, default is terminal height.
 -S: style, ascii or braille, default is ascii.
 -N: number of grayscale bits, default is 4, max is 4 (ascii style only).
 -T: threshold value, default is 128 (braille style only).
 -G: gamma value, default is 1.0.
 -c: enable color.
 -a: enable alpha.
 -r: enable gray reverse (color disabled only).
```
