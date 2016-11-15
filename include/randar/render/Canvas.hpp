#ifndef RANDAR_RENDER_CANVAS_HPP
#define RANDAR_RENDER_CANVAS_HPP

#include <randar/render/Vertices.hpp>

namespace randar
{
    /**
     * An object that may be drawn on.
     */
    class Canvas
    {
    public:
        virtual ~Canvas();
        virtual void draw(randar::Vertices &vertices) const = 0;
    };
}

#endif
