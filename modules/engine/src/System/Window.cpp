#include <randar/Render/Framebuffer.hpp>
#include <randar/System/GraphicsContext.hpp>
#include <randar/System/Window.hpp>

// Constructor.
randar::Window::Window(
    randar::GraphicsContext& context,
    uint32_t width,
    uint32_t height
)
: randar::GraphicsContextResource(&context),
  randar::Dimensional2<uint32_t>(width, height)
{
    ::XSetWindowAttributes swa;
    this->bindContext();

    swa.event_mask = ExposureMask | KeyPressMask;
    swa.colormap = ::XCreateColormap(
        this->ctx->display,
        DefaultRootWindow(this->ctx->display),
        this->ctx->visual->visual,
        AllocNone
    );

    this->handle = ::XCreateWindow(
        this->ctx->display,
        DefaultRootWindow(this->ctx->display),
        0,
        0,
        width,
        height,
        0,
        this->ctx->visual->depth,
        InputOutput,
        this->ctx->visual->visual,
        CWColormap | CWEventMask,
        &swa
    );

    ::XMapWindow(this->ctx->display, this->handle);
    ::XSync(this->ctx->display, false);

    this->glxWindow = ::glXCreateWindow(
        this->ctx->display,
        this->ctx->fbConfigs[0],
        this->handle,
        nullptr
    );

    this->fb.reset(new Framebuffer(*this));
}

// Destructor.
randar::Window::~Window()
{
    this->close();
}

// Closes the window.
void randar::Window::close()
{
    this->fb.release();

    ::glXDestroyWindow(this->ctx->display, this->glxWindow);
    ::XDestroyWindow(this->ctx->display, this->handle);
}

// Retrieves the associated context.
randar::GraphicsContext& randar::Window::context()
{
    return *this->ctx;
}

// Retrieves the native window handle.
::Window randar::Window::native()
{
    return this->handle;
}

// Retrieves the GLX window handle.
::GLXWindow randar::Window::glx()
{
    return this->glxWindow;
}

// Binds the window for further operations.
void randar::Window::bind()
{
    if (!this->ctx) {
        throw std::runtime_error("Graphics context unavailable");
    }

    this->ctx->use(*this);
}

// Retrieves an internal instance of the default framebuffer.
randar::Framebuffer& randar::Window::framebuffer()
{
    if (!this->fb) {
        throw std::runtime_error("Default framebuffer not available");
    }

    return *this->fb;
}

// Presents the contents of the default framebuffer.
void randar::Window::present()
{
    ::glXSwapBuffers(this->ctx->display, this->glxWindow);
}
