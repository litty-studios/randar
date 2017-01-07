#include <randar/Render/Texture.hpp>
#include <randar/Engine/Gpu.hpp>

randar::Texture::Texture(
    randar::Texture::Type initType,
    unsigned int initWidth,
    unsigned int initHeight,
    const std::string& initName
) :
  randar::Resource(initName),
  type(initType),
  width(initWidth),
  height(initHeight)
{

}

void randar::Texture::initialize(randar::Gpu& gpu)
{
    gpu.initialize(*this);
    this->initialized = true;
}

void randar::Texture::destroy(randar::Gpu& gpu)
{
    gpu.destroy(*this);
    this->initialized = false;
}
