#pragma once
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& forward, float fov, float aspectRatio):
		m_position(position), m_fov(fov), m_aspectRatio(aspectRatio)
	{
		SetForward(forward);
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(m_position, m_position + m_forward, m_up);
	}
	glm::mat4 GetProjectionMatrix() {
		return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
	}
	inline void SetFov(float fov) { m_fov = fov; }
	inline float GetFov() const { return m_fov; }
	inline void SetAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; }
	inline float GetAspectRatio() const { return m_aspectRatio; }
	inline void SetPosition(const glm::vec3& position) { m_position = position; }
	inline glm::vec3 GetPosition() const { return m_position; }

	/// <summary>
	/// Sets the direction the camera is facing. Also updates right and up vectors.
	/// </summary>
	/// <param name="forward"></param>
	inline void SetForward(const glm::vec3& forward) { 
		m_forward = glm::normalize(forward);
		m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_forward));
	}

	inline void SetFarPlane(float farPlane) { m_farPlane = farPlane; }
	inline void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }

	inline glm::vec3 GetForward() const { return m_forward; }
	inline glm::vec3 GetRight() const { return m_right; }
	inline glm::vec3 GetUp() const { return m_up; }
	inline float GetNearPlane() const { return m_nearPlane; }
	inline float GetFarPlane() const { return m_farPlane; }
private:
	glm::vec3 m_position = glm::vec3(0);
	glm::vec3 m_forward = glm::vec3(0);
	glm::vec3 m_right = glm::vec3(0);
	glm::vec3 m_up = glm::vec3(0);

	float m_fov = 60.0f;
	float m_aspectRatio = 16.0f / 9.0f;
	float m_nearPlane = 0.1f;
	float m_farPlane = 1000.0f;
	glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
};

