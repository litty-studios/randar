#ifndef RANDAR_RENDER_VERTEX_ARRAY_HPP
#define RANDAR_RENDER_VERTEX_ARRAY_HPP

#include <randar/Render/Vertex.hpp>
#include <randar/Engine/GpuResource.hpp>

namespace randar
{
    /**
     * An OpenGL "helper" object that helps bind a Vertex Buffer by explaining
     * its layout.
     *
     * It would be pointless to instantiate this outside a Vertex Buffer.
     */
    struct VertexArray : virtual public GpuResource
    {

    };

    /**
     * A GPU buffer of vertices.
     *
     * This reverses the OpenGL pattern of using Vertex Arrays as a
     * "container." Here, the Vertex Buffer is the "container" and the Vertex
     * Array is simply used as a "helper" object when binding.
     */
    struct VertexBuffer : virtual public GpuResource
    {
        const static Resource::Type type;
        Resource::Type getType() const;

        VertexArray vertexArray;
    };
}

#endif
