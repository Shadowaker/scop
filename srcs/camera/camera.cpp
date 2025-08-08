#include "../../headers/camera/camera.hpp"
#include "../../headers/datrix/datrix.hpp"

Camera::Camera() {

	position = glm::vec3(0.0f, 0.0f, 3.0f);
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 0.5f, 0.0f);
	side = glm::normalize(glm::cross(front, DEFAULT_UP));
	view = Datrix::lookAtGl(position, position + front, up);
}

Camera::~Camera() {

}

glm::vec3 Camera::getPosition() const {
	return this->position;
}

glm::vec3 Camera::getFront() const {
	return this->front;
}

glm::vec3 Camera::getUp() const {
	return this->up;
}

glm::mat4 Camera::getView() const {
	//return Datrix::lookAtGl(position, position + front, up);
	return glm::lookAt(position, position + front, up);
}

Datrix	Camera::getView2() const {
	return Datrix::lookAt(position, position + front, up);
}

void Camera::setPosition(const glm::vec3 new_position) {
	this->position = new_position;
}

void Camera::setFront(const glm::vec3 new_front) {
	this->front = new_front;
}

void Camera::setUp(const glm::vec3 new_up) {
	this->up = new_up;
}

void Camera::setView(const glm::mat4 &new_view) {
	this->view = new_view;
}

glm::mat4 Camera::createView(const glm::vec3 new_position, const glm::vec3 new_front, const glm::vec3 new_up) {
	this->setPosition(new_position);
	this->setFront(new_front);
	this->setUp(new_up);
	//glm::mat4 new_view = glm::lookAt(new_position, new_position + new_front, new_up);
	glm::mat4 new_view = Datrix::lookAtGl(new_position, new_position + new_front, new_up);
	this->view = new_view;

	return new_view;
}

void Camera::forward() {
	position += front * SPEED;
}

void Camera::backward() {
	position -= front * SPEED;
}

void Camera::left() {
	position -= side * SPEED;
}

void Camera::right() {
	position += side * SPEED;
}

void Camera::goUp() {
	position += up * SPEED;
}

void Camera::goDown() {
	position -= up * SPEED;
}

void Camera::pitchUp() {
	front.y += ROT_SPEED;
}
void Camera::pitchDown() {
	front.y -= ROT_SPEED;
}

void Camera::rotateRight() {
	front.x += ROT_SPEED;
}

void Camera::rotateLeft() {
	front.x -= ROT_SPEED;
}