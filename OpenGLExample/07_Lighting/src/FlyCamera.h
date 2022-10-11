#pragma once
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

class FlyCamera
{
public:
	FlyCamera(Camera* camera, float moveSpeed)
		: m_camera(camera),
		m_pitch(0),
		m_yaw(0),
		m_moveSpeed(moveSpeed)
	{
	};

	/// <summary>
	/// Changes position
	/// </summary>
	/// <param name="horizontalAxis"></param>
	/// <param name="forwardAxis"></param>
	/// <param name="upAxis"></param>
	void ProcessMovement(float horizontalAxis, float forwardAxis, float upAxis, float deltaTime) {
		const float speed = m_moveSpeed * deltaTime;
		
		glm::vec3 pos = m_camera->GetPosition();
		pos += m_camera->GetForward() * forwardAxis * speed;
		pos += m_camera->GetRight() * horizontalAxis * speed;
		pos += m_camera->GetUp() * upAxis * speed;
		m_camera->SetPosition(pos);
	}

	/// <summary>
	/// Adjusts rotation from input
	/// </summary>
	/// <param name="horizontalAxis"></param>
	/// <param name="verticalAxis"></param>
	void ProcessLook(float horizontalAxis, float verticalAxis) {
		m_yaw += horizontalAxis;
		m_pitch += verticalAxis;
		m_pitch = glm::clamp(m_pitch, -MAX_PITCH_DEGREES, MAX_PITCH_DEGREES);

		glm::vec3 forward;
		forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		forward.y = sin(glm::radians(m_pitch));
		forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_camera->SetForward(forward);
	}

private:
	Camera* m_camera = nullptr;
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;
	float m_moveSpeed = 1.0f;
	const float MAX_PITCH_DEGREES = 89.0f;
};

