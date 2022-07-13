#include "OrbitCamera.h"
#include <glm/gtc/matrix_transform.hpp>

OrbitCamera::OrbitCamera(const glm::vec3& center, float fov, float radius, float radiusMax)
    : m_center(center), m_fov(fov), m_radius(radius), m_radiusMax(radiusMax)
{
}

void OrbitCamera::rotate(float azimuth, float polar)
{
    m_azimuth += azimuth;
    m_polar += polar;
    const auto polarMax = glm::pi<float>() / 2.0f - 0.001f;
    if (m_polar > polarMax)
        m_polar = polarMax;
    if (m_polar < -polarMax)
        m_polar = -polarMax;
}

void OrbitCamera::zoom(float amount)
{
    m_radius += amount;
    if (m_radius < 0)
        m_radius = 0;
    if (m_radius > m_radiusMax)
        m_radius = m_radiusMax;
}

glm::mat4 OrbitCamera::getViewMatrix() const
{
    const float cosPolar = cos(m_polar);
    const float sinPolar = sin(m_polar);
    const float cosAzimuth = cos(m_azimuth);
    const float sinAzimuth = sin(m_azimuth);

    float camX = m_radius * cosPolar * cosAzimuth;
    float camY = m_radius * sinPolar;
    float camZ = m_radius * cosPolar * sinAzimuth;

    return glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
}
