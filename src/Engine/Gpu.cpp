#include <randar/Engine/Gpu.hpp>
#include <randar/Render/Model.hpp>

// Construction.
randar::Gpu::Gpu()
: defaultFramebuffer(randar::Viewport(0, 0, 800, 600))
{
    // Initialize GLFW.
    if (!::glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    ::glfwWindowHint(GLFW_SAMPLES, 0);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = ::glfwCreateWindow(800, 600, "Randar", NULL, NULL);
    if (!this->window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    ::glfwMakeContextCurrent(this->window);

    // Initialize GLEW.
    ::glewExperimental = true;
    if (::glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    // @todo - Ignore driver error. Investigate later.
    if (::glGetError() != GL_NO_ERROR) {

    }

    // Configure OpenGL.
    ::glEnable(GL_DEPTH_TEST);
    ::glDepthFunc(GL_LESS);
}

// Destruction.
randar::Gpu::~Gpu()
{
    ::glfwDestroyWindow(this->window);
}

// Retrieves the default window.
::GLFWwindow& randar::Gpu::getWindow()
{
    return *this->window;
}

// Retrieves the default framebuffer.
const randar::Framebuffer& randar::Gpu::getDefaultFramebuffer() const
{
    return this->defaultFramebuffer;
}

randar::Framebuffer& randar::Gpu::getDefaultFramebuffer()
{
    return this->defaultFramebuffer;
}

// Initializes a framebuffer.
void randar::Gpu::initialize(randar::Framebuffer& framebuffer)
{
    if (framebuffer.isInitialized()) {
        return;
    }

    // Initialize framebuffer.
    ::glGenFramebuffers(1, framebuffer);
    this->bind(framebuffer);

    // Initialize attachments, add them to the framebuffer.
    Texture *texture = framebuffer.getTexture();
    if (texture) {
        this->initialize(*texture);
        this->bind(*texture);

        switch (texture->textureType) {
            case Texture::DEPTH:
                ::glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *texture, 0);
                break;

            case Texture::RGBA:
                ::glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *texture, 0);
                break;

            default:
                throw std::runtime_error("Adding invalid texture to framebuffer");
                break;
        }
    }

    ::glDrawBuffer(GL_NONE);

    if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Incomplete framebuffer");
    }
}

// Initializes an index buffer.
void randar::Gpu::initialize(randar::IndexBuffer& buffer)
{
    if (buffer.isInitialized()) {
        return;
    }

    ::glGenBuffers(1, buffer);
}

// Initializes a shader.
void randar::Gpu::initialize(randar::Shader& shader)
{
    if (shader.isInitialized()) {
        return;
    }

    GLint compileStatus, logLength;
    shader.setGlName(::glCreateShader(shader.shaderType));

    const char *rawCode = shader.code.c_str();
    ::glShaderSource(shader, 1, &rawCode, nullptr);
    ::glCompileShader(shader);

    ::glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    ::glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (compileStatus == GL_FALSE) {
        if (logLength > 0) {
            GLchar *log = new char[logLength + 1];
            ::glGetShaderInfoLog(shader, logLength, nullptr, log);

            std::string safeLog(log);
            delete[] log;
            throw std::runtime_error(safeLog);
        } else {
            throw std::runtime_error("Cannot compile shader. No log available");
        }
    }
}

// Initializes a shader program.
void randar::Gpu::initialize(randar::ShaderProgram& program)
{
    if (program.isInitialized()) {
        return;
    }

    // Initialize dependencies.
    this->initialize(program.vertexShader);
    this->initialize(program.fragmentShader);

    GLint linkStatus, logLength;
    program.setGlName(::glCreateProgram());

    ::glAttachShader(program, program.vertexShader);
    ::glAttachShader(program, program.fragmentShader);
    ::glLinkProgram(program);

    ::glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    ::glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (linkStatus == GL_FALSE) {
        if (logLength > 0) {
            GLchar *log = new char[logLength + 1];
            ::glGetProgramInfoLog(program, logLength, nullptr, log);

            std::string safeLog(log);
            delete[] log;
            throw std::runtime_error(safeLog);
        } else {
            throw std::runtime_error("Cannot link shader, no log available");
        }
    }

    ::glDetachShader(program, program.vertexShader);
    ::glDetachShader(program, program.fragmentShader);
}

// Initializes a texture.
void randar::Gpu::initialize(randar::Texture& texture)
{
    if (texture.isInitialized()) {
        return;
    }

    ::glGenTextures(1, texture);
    this->bind(texture);
    this->clear(texture);

    switch (texture.textureType) {
        default:
            ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
    }
}

// Initializes a vertex array.
// @todo - Make this into a useful function, out of vertex buffer initialization.
void randar::Gpu::initialize(randar::VertexArray& vertexArray)
{
    if (vertexArray.isInitialized()) {
        return;
    }

    ::glGenVertexArrays(1, vertexArray);
}

// Initializes a vertex buffer.
void randar::Gpu::initialize(randar::VertexBuffer& buffer)
{
    if (buffer.isInitialized()) {
        return;
    }

    ::glGenVertexArrays(1, buffer.vertexArray);
    ::glBindVertexArray(buffer.vertexArray);

    ::glGenBuffers(1, buffer);
    ::glBindBuffer(GL_ARRAY_BUFFER, buffer);

    unsigned int stride = Vertex::stride * sizeof(GLfloat);

    // Position.
    ::glEnableVertexAttribArray(0);
    ::glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)0
    );  

    // Color.
    ::glEnableVertexAttribArray(1);
    ::glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(3 * sizeof(GLfloat))
    );  

    // Bone index.
    ::glEnableVertexAttribArray(2);
    ::glVertexAttribPointer(
        2,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(7 * sizeof(GLfloat))
    );  

    // Bone weights.
    ::glEnableVertexAttribArray(3);
    ::glVertexAttribPointer(
        3,
        4,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(11 * sizeof(GLfloat))
    );

    // Texture coordinates.
    ::glEnableVertexAttribArray(4);
    ::glVertexAttribPointer(
        4,
        2,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void*)(15 * sizeof(GLfloat))
    );
}

// Destroys a framebuffer.
void randar::Gpu::destroy(randar::Framebuffer& framebuffer)
{
    if (!framebuffer.isInitialized()) {
        throw std::runtime_error("Destroying framebuffer that is not initialized");
    }
    ::glDeleteFramebuffers(1, framebuffer);
}

// Destroys an index buffer.
void randar::Gpu::destroy(randar::IndexBuffer& buffer)
{
    if (!buffer.isInitialized()) {
        throw std::runtime_error("Destroying index buffer that is not initialized");
    }
    ::glDeleteBuffers(1, buffer);
}

// Destroys a shader.
void randar::Gpu::destroy(randar::Shader& shader)
{
    if (!shader.isInitialized()) {
        throw std::runtime_error("Destroying shader that is not initialized");
    }
    ::glDeleteShader(shader);
}

// Destroys a shader program.
void randar::Gpu::destroy(randar::ShaderProgram& program)
{
    if (!program.isInitialized()) {
        throw std::runtime_error("Destroying shader program that is not initialized");
    }
    ::glDeleteProgram(program);
}

// Destroys a texture.
void randar::Gpu::destroy(randar::Texture& texture)
{
    if (!texture.isInitialized()) {
        throw std::runtime_error("Destroying texture that is not initialized");
    }
    ::glDeleteTextures(1, texture);
}

// Destroys a vertex array.
// @todo - Separate logic from vertex buffers.
void randar::Gpu::destroy(randar::VertexArray& vertexArray)
{
    if (!vertexArray.isInitialized()) {
        throw std::runtime_error("Destroying vertex array that is not initialized");
    }
}

// Destroys a vertex buffer.
void randar::Gpu::destroy(randar::VertexBuffer& buffer)
{
    if (!buffer.isInitialized()) {
        throw std::runtime_error("Destroying vertex buffer that is not initialized");
    }
    ::glDeleteBuffers(1, buffer);
}

// Clears a framebuffer.
void randar::Gpu::clear(const randar::Framebuffer& framebuffer, const randar::Color& color)
{
    ::glClearColor(color.r, color.g, color.b, color.a);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Clears a texture.
void randar::Gpu::clear(const randar::Texture& texture)
{
    this->write(texture, nullptr);
}

// Writes indices to an index buffer.
void randar::Gpu::write(const randar::IndexBuffer& indexBuffer, const std::vector<unsigned int>& indices)
{
    this->bind(indexBuffer);

    ::glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        &indices.data()[0],
        GL_STATIC_DRAW
    );
}

// Writes an image to a texture.
void randar::Gpu::write(const randar::Texture& texture, const GLvoid* data)
{
    this->bind(texture);

    switch (texture.textureType) {
        case Texture::RGBA:
            ::glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                texture.width,
                texture.height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );
            break;

        case Texture::DEPTH:
            ::glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_DEPTH_COMPONENT,
                texture.width,
                texture.height,
                0,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                data
            );
            break;

        default:
            throw std::runtime_error("Setting data on invalid texture type");
    }
}

// Writes vertices to a vertex buffer.
void randar::Gpu::write(const randar::VertexBuffer& buffer, const std::vector<Vertex>& vertices)
{
    unsigned int count = vertices.size();
    GLfloat *data = new GLfloat[count * Vertex::stride];
    for (unsigned int i = 0; i < count; i++) {
        vertices[i].appendTo(&data[i * Vertex::stride]);
    }

    ::glBindBuffer(GL_ARRAY_BUFFER, buffer);
    ::glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), data, GL_STATIC_DRAW);
    delete[] data;
}

// Binds a framebuffer.
void randar::Gpu::bind(const randar::Framebuffer& framebuffer)
{
    ::glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    const Viewport &viewport = framebuffer.camera.viewport;
    ::glViewport(viewport.x1, viewport.y1, viewport.x2, viewport.y2);
}

// Binds an index buffer.
void randar::Gpu::bind(const randar::IndexBuffer& buffer)
{
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

// Binds the vertex and index buffers of a mesh.
void randar::Gpu::bind(const randar::Model& model)
{
    this->bind(model.mesh.vertexBuffer);
    this->bind(model.mesh.indexBuffer);
}

// Binds a texture.
void randar::Gpu::bind(const randar::Texture& texture)
{
    if (texture.getGlName() != boundTexture) {
        ::glBindTexture(GL_TEXTURE_2D, texture);
    }
}

// Binds a vertex buffer.
void randar::Gpu::bind(const randar::VertexBuffer& buffer)
{
    ::glBindVertexArray(buffer.vertexArray);
}

// Drawing.
void randar::Gpu::draw(
    const ShaderProgram& program,
    const randar::Framebuffer& framebuffer,
    const randar::Model& model)
{
    if (!program.isInitialized()) {
        throw std::runtime_error("Drawing model without shader program");
    }

    this->bind(framebuffer);

    // Set MVP uniform.
    glm::mat4 mvp = framebuffer.camera.getProjectionMatrix()
        * framebuffer.camera.getViewMatrix()
        * model.getTransformMatrix();

    ::glUseProgram(program);
    ::GLuint mvpId = ::glGetUniformLocation(program, "mvp");
    ::glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

    // Set joints uniform.
    if (model.joints.size()) {
        glm::mat4 jointMatrices[model.joints.size()];
        for (unsigned int i = 0; i < model.joints.size(); i++) {
            jointMatrices[i] = model.joints[i]->getPoseMatrix();
        }

        ::glUniformMatrix4fv(
            ::glGetUniformLocation(program, "joints"),
            model.joints.size(),
            GL_FALSE,
            &jointMatrices[0][0][0]
        );
    }

    // Draw model.
    //this->bind(model.mesh);
    /*::glDrawElements(
        GL_TRIANGLES,
        model.mesh.indices.size(),
        GL_UNSIGNED_INT,
        (void*)0
    );*/
}

// Retrieves the default GPU context.
randar::Gpu& randar::getDefaultGpu()
{
    static Gpu defaultGpu;
    return defaultGpu;
}
