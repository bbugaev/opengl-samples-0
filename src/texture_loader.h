#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <FreeImage.h>


class TextureLoader
{
public:
    TextureLoader(char const *filename);
    ~TextureLoader();

    unsigned width() const
    { return width_; }

    unsigned height() const
    { return height_; }

    BYTE *bits() const
    { return bits_; }

private:
    unsigned width_;
    unsigned height_;
    BYTE *bits_;
    FIBITMAP *dib_;
};


#endif // TEXTURE_LOADER_H
