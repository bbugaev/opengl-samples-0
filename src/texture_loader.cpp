#include "texture_loader.h"


TextureLoader::TextureLoader(char const *filename):
    width_(0),
    height_(0),
    bits_(0),
    dib_(0)
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    fif = FreeImage_GetFileType(filename, 0);
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename);
    if(FreeImage_FIFSupportsReading(fif))
        dib_ = FreeImage_Load(fif, filename);

    if(!dib_)
        return;

    bits_ = FreeImage_GetBits(dib_);
    width_ = FreeImage_GetWidth(dib_);
    height_ = FreeImage_GetHeight(dib_);
}


TextureLoader::~TextureLoader()
{
    FreeImage_Unload(dib_);
}
