#ifndef RANDAR_RENDER_TEXTURE_HPP
#define RANDAR_RENDER_TEXTURE_HPP

#define RANDAR_TEXTURE_MAX_WIDTH  4096
#define RANDAR_TEXTURE_MAX_HEIGHT 4096

#include <randar/Engine/GpuResource.hpp>
#include <randar/Render/Image.hpp>
#include <randar/System/GraphicsContext.hpp>

namespace randar
{
    class Texture :
        virtual public GpuResource,
        virtual public Dimensional2<uint32_t>
    {
    public:
        /**
         * Help swig identify inherited methods.
         */
        using Dimensional2<uint32_t>::getWidth;
        using Dimensional2<uint32_t>::getHeight;
        using Dimensional2<uint32_t>::setWidth;
        using Dimensional2<uint32_t>::setHeight;
        using Dimensional2<uint32_t>::hasDimensions;
        using Dimensional2<uint32_t>::isWithinDimensions;

        /**
         * Type of texture.
         *
         * Valid values are "rgba" and "depth".
         */
        const std::string type;

        /**
         * New texture constructor.
         */
        Texture(
            GraphicsContext& context,
            uint32_t initWidth,
            uint32_t initHeight,
            const std::string& initType = "rgba");

        /**
         * Destructor.
         */
        ~Texture();

        /**
         * Binds the texture for further operations.
         */
        void bind();

        /**
         * Resets the texture with arbitrary data.
         *
         * Implicitly lets the GPU know the current size of the texture.
         */
        void reset();

        /**
         * Resizes this texture.
         */
        virtual void resize(uint32_t newWidth, uint32_t newHeight) override;

        /**
         * Reads the contents of the texture.
         */
        void read(Image& image);
    };
}

#endif
