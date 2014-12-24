#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H


class TextureLoader
{
public:
    TextureLoader(char const *filename);
    ~TextureLoader();

    unsigned width() const
    { return width_; }

    unsigned height() const
    { return height_; }

    unsigned char *bits() const
    { return bits_; }

private:
    unsigned width_;
    unsigned height_;
    unsigned char *bits_;
    void *dib_;
};


#endif // TEXTURE_LOADER_H
