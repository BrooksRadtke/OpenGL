#pragma once

#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Texture.h"

// BallObject holds the state of the ball obj inheriting
// relevant state data from GameObject. Contains some 
// extra functionality specific to Breakout's ball object
// that were too secific for GameObject alone
class BallObject : public GameObject
{
public:
	// ball state
	float	Radius;
	bool	Stuck;

	// constructor(s)
	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	// moves ball, keeping it constrained within th ewindow bounds (except the bottm); returns new positions
	glm::vec2	Move(float dt, unsigned int window_width);
	// resets the ball to original state with given position and velocity
	void		Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // !BALLOBJECT_H
