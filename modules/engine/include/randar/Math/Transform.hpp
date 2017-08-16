#ifndef RANDAR_MATH_TRANSFORM_HPP
#define RANDAR_MATH_TRANSFORM_HPP

#include <randar/Math/Quaternion.hpp>
#include <randar/Utility/Json.hpp>

namespace randar
{
    class Transform
    {
    private:
        Vector3 pos;
        Quaternion rot;

    public:
        ~Transform();

        /**
         * Sets this transformation from a physical transformation.
         */
        void set(const btTransform& transform);

        /**
         * Absolutely sets the position.
         */
        void position(const Vector3& newPosition);

        /**
         * Relatively sets the position.
         */
        void move(const Vector3& movement);
        void move(const Quaternion& angularMovement);
        void move(const Vector3& axis, const Angle& angle);

        /**
         * Retrieves the position.
         */
        Vector3 position() const;

        /**
         * Sets the rotation.
         */
        void rotation(const Quaternion& quaternion);
        void rotation(const Vector3& axis, const Angle& angle);

        /**
         * Retrieves the rotation.
         */
        Quaternion rotation() const;

        /**
         * Sets and retrieves the axis of rotation.
         */
        void rotationAxis(const Vector3& axis);
        Vector3 rotationAxis() const;

        /**
         * Absolutely sets the angle of rotation.
         */
        void angle(const Angle& angle);

        /**
         * Relatively sets the angle of rotation.
         */
        void rotate(const Angle& angle);

        /**
         * Retrieves the angle of rotation.
         */
        Angle angle() const;

        /**
         * Applies another transform to this transform.
         */
        void apply(const Transform& other);

        /**
         * Function called when a transformation occurs.
         *
         * @@@ deprecating this
         */
        void onTransform();

        /**
         * Transforms a vector.
         */
        Vector3 transform(const Vector3& vector) const;

        /**
         * Retrieves the current transformation matrix.
         */
        glm::mat4 transformMatrix() const;

        /**
         * Converts to physics transformation.
         */
        operator btTransform() const;

        /**
         * An identity transformation.
         */
        static Transform Identity;
    };

    // Node.js helper.
    Transform transform();
}

#endif
