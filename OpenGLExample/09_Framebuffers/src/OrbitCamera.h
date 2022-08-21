#pragma once
#include <glm/glm.hpp>

class OrbitCamera {
public:
	OrbitCamera(const glm::vec3& center, float fov, float radius, float radiusMax);
	void rotate(float azimuth, float polar);
	void zoom(float amount);
	const float getFov() const { return m_fov; }
	glm::mat4 getViewMatrix()const;
	glm::vec3 getEyePos() const;
	void setAzimuth(float azimuth) { m_azimuth = azimuth; }
private:
	glm::vec3 m_center;
	float m_radius = 3.0f;
	float m_azimuth = 0, m_polar = 0;
	float m_fov = 45.0f;
	float m_radiusMax = 5.0f;
};