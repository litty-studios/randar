#ifndef RANDAR_RENDER_FRAMEBUFFER_HPP
#define RANDAR_RENDER_FRAMEBUFFER_HPP

#include <randar/Render/Camera.hpp>
#include <randar/Render/Renderbuffer.hpp>
#include <randar/Render/Texture.hpp>

namespace randar
{
    class Framebuffer : virtual public GpuResource
    {
    protected:
        Texture* texture;

    public:
        Camera camera;

        /**
         * Constructs a framebuffer with no attachments.
         */
        Framebuffer(const Viewport& initViewport = Viewport());

        /**
         * Constructs a framebuffer with a single texture attachment.
         */
        Framebuffer(Texture *initTexture);

        /**
         * Retrieves the texture attachment.
         */
        Texture* getTexture();

        /**
         * Resource initialization and destruction.
         */
        virtual void initialize(Gpu& gpu) override;
        virtual void destroy(Gpu& gpu) override;
    };
}

#endif
