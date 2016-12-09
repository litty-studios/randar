#include <randar/render/Camera.hpp>

randar::Camera::Camera()
: fieldOfView(45.0f),
  aspectRatio(4.0f / 3.0f),
  nearZ(0.1f),
  farZ(100.0f)
{
    this->updateMatrices();
}

void randar::Camera::updateMatrices()
{
    this->projection = glm::perspective(
        this->fieldOfView.toRadians(),
        this->aspectRatio,
        this->nearZ,
        this->farZ
    );

    this->view = glm::lookAt(
        glm::vec3(this->position.x, this->position.y, this->position.z),
        glm::vec3(this->target.x, this->target.y, this->target.z),
        glm::vec3(0, 1, 0)
    );
}

void randar::Camera::setPosition(randar::Vector newPosition)
{
    this->position = newPosition;
    this->updateMatrices();
}

randar::Vector randar::Camera::getPosition() const
{
    return this->position;
}

void randar::Camera::setTarget(randar::Vector newTarget)
{
    this->target = newTarget;
    this->updateMatrices();
}

randar::Vector randar::Camera::getTarget() const
{
    return this->target;
}

glm::mat4 randar::Camera::getViewMatrix() const
{
    return this->view;
}

glm::mat4 randar::Camera::getProjectionMatrix() const
{
    return this->projection;
}
