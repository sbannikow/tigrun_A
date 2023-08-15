#pragma once

#define GLEW_STATIC
#include <iostream>
#include <GL/glew.h>
#define M_PI 3.14159265358979323846
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

 extern void firstPersonMovingCalc(GLFWwindow* window, glm::vec3 &out_camDir, GLfloat &out_angleX, GLfloat &t_WX, GLfloat &t_WY);
 extern void firstPersonMoveForward (GLfloat in_angleX, glm::vec3 &out_camPos, GLfloat speed);
 extern void firstPersonMoveBackward (GLfloat in_angleX, glm::vec3 &out_camPos, GLfloat speed);
 extern void firstPersonStrafeLeft (glm::vec3 in_camDir, glm::vec3 in_camUp, glm::vec3 &out_camPos, GLfloat speed);
 extern void firstPersonStrafeRight (glm::vec3 in_camDir, glm::vec3 in_camUp, glm::vec3 &out_camPos, GLfloat speed);
 extern void firstPersonMoveSEward(GLfloat in_angleX, glm::vec3 &out_camPos, GLfloat speed);
