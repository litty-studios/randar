#include <randar/Engine/EngineMonitor.hpp>
#include <randar/Engine/Gpu.hpp>

randar::EngineMonitor::EngineMonitor()
: monitorFramebuffer("rgba", true),
  camera(monitorFramebuffer.camera),
  newTarget(false)
{
    this->clearTarget();

    screenProgram.set(
        randar::Shader(GL_VERTEX_SHADER, randar::readAsciiFile("./resources/shaders/screen.vert")),
        randar::Shader(GL_FRAGMENT_SHADER, randar::readAsciiFile("./resources/shaders/screen.frag"))
    );

    modelProgram.set(
        randar::Shader(GL_VERTEX_SHADER, randar::readAsciiFile("./resources/shaders/monitor-model.vert")),
        randar::Shader(GL_FRAGMENT_SHADER, randar::readAsciiFile("./resources/shaders/monitor-model.frag"))
    );

    textureProgram.set(
        randar::Shader(GL_VERTEX_SHADER, randar::readAsciiFile("./resources/shaders/monitor-texture.vert")),
        randar::Shader(GL_FRAGMENT_SHADER, randar::readAsciiFile("./resources/shaders/monitor-texture.frag"))
    );

    // Screen vertices.
    Vertex vertex;
    vertex.position.set(-0.5f, -1.0f, 0.001f);
    vertex.textureCoordinate.u = 0.0f;
    vertex.textureCoordinate.v = 0.0f;
    this->screen.vertices.push_back(vertex);

    vertex.position.set(-0.5f, 1.0f, 0.001f);
    vertex.textureCoordinate.u = 0.0f;
    vertex.textureCoordinate.v = 1.0f;
    this->screen.vertices.push_back(vertex);

    vertex.position.set(1.0f, -1.0f, 0.001f);
    vertex.textureCoordinate.u = 1.0f;
    vertex.textureCoordinate.v = 0.0f;
    this->screen.vertices.push_back(vertex);

    vertex.position.set(1.0f, 1.0f, 0.001f);
    vertex.textureCoordinate.u = 1.0f;
    vertex.textureCoordinate.v = 1.0f;
    this->screen.vertices.push_back(vertex);

    // Screen face indices.
    this->screen.faceIndices.push_back(0);
    this->screen.faceIndices.push_back(1);
    this->screen.faceIndices.push_back(2);

    this->screen.faceIndices.push_back(3);
    this->screen.faceIndices.push_back(1);
    this->screen.faceIndices.push_back(2);

    // Send models to the GPU.
    this->gpu.write(this->screen);

    // Initialize the UI size.
    this->resize();
}

randar::EngineMonitor::~EngineMonitor()
{

}

// Resizes the UI to fit the default window.
void randar::EngineMonitor::resize()
{
    int width, height;
    ::glfwGetWindowSize(&this->gpu.getWindow(), &width, &height);

    this->defaultFramebuffer.resize(width, height);
    this->monitorFramebuffer.resize(width * 0.75f, height);
}

// Clears the current target.
void randar::EngineMonitor::clearTarget()
{
    this->newTarget = false;

    this->targetModel = nullptr;
    this->targetTexture = nullptr;
}

// Sets a model as the monitoring target.
void randar::EngineMonitor::setTarget(randar::Model& model)
{
    this->clearTarget();
    this->newTarget = true;
    this->targetModel = &model;
}

void randar::EngineMonitor::setTarget(randar::Texture& texture)
{
    this->clearTarget();
    this->newTarget = true;
    this->targetTexture = &texture;

}

// Whether the monitoring target has been changed.
bool randar::EngineMonitor::hasNewTarget() const
{
    return this->newTarget;
}

// Initializes the current target onto the monitor.
void randar::EngineMonitor::initializeTarget()
{
    if (this->targetModel) {
        this->monitorFramebuffer.camera.setPosition(randar::Vector(0.0f, 0.0f, -15.0f));
        this->monitorFramebuffer.camera.setTarget(randar::Vector(0.0f, 0.0f, 0.0f));
    }

    else if (this->targetTexture) {
        // Texture model vertices.
        Vertex vertex;
        vertex.position.set(-1.0f, -1.0f, 0.001f);
        vertex.textureCoordinate.u = 0.0f;
        vertex.textureCoordinate.v = 0.0f;
        this->targetTextureModel.vertices.push_back(vertex);

        vertex.position.set(-1.0f, 1.0f, 0.001f);
        vertex.textureCoordinate.u = 0.0f;
        vertex.textureCoordinate.v = 1.0f;
        this->targetTextureModel.vertices.push_back(vertex);

        vertex.position.set(1.0f, -1.0f, 0.001f);
        vertex.textureCoordinate.u = 1.0f;
        vertex.textureCoordinate.v = 0.0f;
        this->targetTextureModel.vertices.push_back(vertex);

        vertex.position.set(1.0f, 1.0f, 0.001f);
        vertex.textureCoordinate.u = 1.0f;
        vertex.textureCoordinate.v = 1.0f;
        this->targetTextureModel.vertices.push_back(vertex);

        // Texture model face indices.
        this->targetTextureModel.faceIndices.push_back(0);
        this->targetTextureModel.faceIndices.push_back(1);
        this->targetTextureModel.faceIndices.push_back(2);

        this->targetTextureModel.faceIndices.push_back(3);
        this->targetTextureModel.faceIndices.push_back(1);
        this->targetTextureModel.faceIndices.push_back(2);

        // Write texture model to GPU.
        this->gpu.write(this->targetTextureModel);
    }

    this->newTarget = false;
}

// Draws the monitoring target.
void randar::EngineMonitor::draw()
{
    this->monitorFramebuffer.clear(Color(0.03f, 0.03f, 0.25f));

    // Initialize the target if it is new.
    if (this->hasNewTarget()) {
        this->initializeTarget();
    }

    // Target is a model.
    if (this->targetModel) {
        this->gpu.draw(
            this->modelProgram,
            this->monitorFramebuffer,
            *this->targetModel
        );
    }

    // Target is a texture.
    else if (this->targetTexture) {
        this->gpu.bind(*this->targetTexture);

        this->gpu.draw(
            this->textureProgram,
            this->monitorFramebuffer,
            this->targetTextureModel
        );
    }

    // No target to draw.
    else {

    }

    // Draw monitor framebuffer onto screen model.
    this->gpu.bind(this->monitorFramebuffer.getTexture());
    this->gpu.draw(this->screenProgram, this->defaultFramebuffer, this->screen);
}
