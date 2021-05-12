#pragma once

#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameLevel.h"

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Four possible collision directions
enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// Define a collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Initial size of player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
// Initial velocity of the ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of ball object
const float BALL_RADIUS = 12.5f;

// Game class holds all game-related state and functionality
// Combines all game-related data into a single class for
// easy access to each of the components and manageability
class Game
{
public:
	// Game State
	GameState				State;
	bool					Keys[1024];
	unsigned int			Width, Height;

	std::vector<GameLevel>	Levels;
	unsigned int			Level;

	// Constructor/destructor
	Game(unsigned int width, unsigned int height);
	~Game();

	// Initialize game state (load all shaders/textures/levels)
	void Init();

	// Game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void DoCollisions();

	void ResetLevel();
	void ResetPlayer();
};

#endif // GAME_H