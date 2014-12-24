#include "texture_loader.h"

#include <FreeImage.h>


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

    FIBITMAP *dib;
    if(FreeImage_FIFSupportsReading(fif))
        dib_ = dib = FreeImage_Load(fif, filename);

    if(!dib)
        return;

    bits_ = FreeImage_GetBits(dib);
    width_ = FreeImage_GetWidth(dib);
    height_ = FreeImage_GetHeight(dib);
}


TextureLoader::~TextureLoader()
{
    FreeImage_Unload(reinterpret_cast<FIBITMAP *>(dib_));
}
