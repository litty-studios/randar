function assertPosition(transform, x, y, z) {
    assert.equal(transform.position().x.toFixed(2), x.toFixed(2));
    assert.equal(transform.position().y.toFixed(2), y.toFixed(2));
    assert.equal(transform.position().z.toFixed(2), z.toFixed(2));
}

function assertAxis(transform, x, y, z) {
    for (axis of [transform.rotation().axis(), transform.axis()]) {
        assert.equal(axis.x.toFixed(2), x.toFixed(2));
        assert.equal(axis.y.toFixed(2), y.toFixed(2));
        assert.equal(axis.z.toFixed(2), z.toFixed(2));
    }
}

function assertAngle(transform, radians) {
    for (angle of [transform.rotation().angle(), transform.angle()]) {
        assert.equal(angle.toRadians().toFixed(2), radians.toFixed(2));
    }
}

function assertTransform(transform, xPos, yPos, zPos, xAx, yAx, zAx, rads) {
    assertPosition(transform, xPos, yPos, zPos);
    assertAxis(transform, xAx, yAx, zAx);
    assertAngle(transform, rads);
}

describe('Transform', function() {
    describe('construction', function() {
        it('accepts no parameters', function() {
            assertTransform(
                randar.transform(),
                0, 0, 0,
                0, 0, 0,
                0
            );
        });

        it('accepts position', function() {
            assertTransform(
                randar.transform(randar.vector(1, 2, 3)),
                1, 2, 3,
                0, 0, 0,
                0
            );
        });

        it('accepts position and quaternion', function() {
            assertTransform(
                randar.transform(
                    randar.vector(4, 5, 6),
                    randar.quaternion(randar.vector(7, 8, 9), randar.angle(3.01))
                ),
                4, 5, 6,
                0.5, 0.57, 0.65,
                3.01
            );
        });

        it('accepts position, axis of rotation, and angle', function() {
            assertTransform(
                randar.transform(
                    randar.vector(0.1, 0.2, 0.3),
                    randar.vector(0.4, 0.5, 0.6),
                    randar.angle(0.7)
                ),
                0.1, 0.2, 0.3,
                0.46, 0.57, 0.68,
                0.7
            );
        });

        it('accepts quaternion', function() {
            assertTransform(
                randar.transform(randar.quaternion(
                    randar.vector(0.5, 0, 0),
                    randar.angle(0.6)
                )),
                0, 0, 0,
                1, 0, 0,
                0.6
            );
        });
    });
});
