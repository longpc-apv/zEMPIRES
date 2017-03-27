PAL file format description
===========================

JASC-PAL Paint Shop Pro (.PAL) describes a text file format that contains the colour palette table saved from a 16 or 256 bit image. It can be loaded for another image and this provides a means of copying colour palettes from one image to another. These files are text editable. The format of the file is as follows:

1. line contains the header "JASC-PAL" (without the quotes).
2. line contains the palette file version. For example "0100" (without the quotes).
3. line contains the number of colours, either 16 or 256.

Subsequent lines contain the RGB components of each palette entry, with a space between each of the Red, Green and Blue numbers. These numbers are in decimal, so each attribute should be in the range 0 to 255.
