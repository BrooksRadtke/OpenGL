#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"

// Game-related state data
SpriteRenderer	*Renderer;
GameObject		*Player;
GameObject		*Ball;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
}

void Game::Init()
{
	// load shaders
	ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
	
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	
	// set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	
	// load textures
	ResourceManager::LoadTexture("textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("textures/block.png", false, "block");
	ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");

	// Load levels
	GameLevel one; one.Load("Levels/one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load("Levels/two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load("Levels/three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load("Levels/four.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);

	// configure game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::Update(float dt)
{
	// update objects
	Ball->Move(dt, this->Width);

	// check collisions
	this->DoCollisions();

	// check loss condition, i.e. did ball reach bottom of screen
	if (Ball->Position.y >= this->Height)
	{
		this->ResetLevel();
		this->ResetPlayer();
	}
}

void Game::ProcessInput(float dt)
{
	if (this->State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * dt;

		// move playerboard
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x += velocity;
			}
		}
		if(this->Keys[GLFW_KEY_D])
		{
			if(Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
		{
			Ball->Stuck = false;
		}
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		// Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f),
			glm::vec2(this->Width, this->Height),
			0.0f);

		// draw level
		this->Levels[this->Level].Draw(*Renderer);
		// draw player
		Player->Draw(*Renderer);
	}
}

void Game::ResetLevel()
{
	if (this->Level == 0)
	{
		this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
	}
	else if (this->Level == 1)
	{
		this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
	}
	else if (this->Level == 2)
	{
		this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
	}
	else if (this->Level == 3)
	{
		this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);
	}
}

void Game::ResetPlayer()
{
	// reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

bool CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(BallObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*ball, box);
			// if collision is true
			if (std::get_temporary_buffer<0>(collision))
			{
				//destroy block if not solid
				if (!box.IsSolid)
				{
					box.Destroyed = true;
				}
				// collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);

				// horizontal collision
				if (dir == LEFT || dir == RIGHT)
				{
					// Reverse horizontal velocity
					Ball->Velocity.x = -Ball->Velocity.x;
					//relocate
					float penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
					{
						// move ball right
						Ball->Position.y -= penetration;
					}
					else
					{
						// move ball left
						Ball->Position.y += penetration;
					}
				}
				// vertical collision
				else
				{
					// Reverse vertical velocity
					Ball->Velocity.y = -Ball->Velocity.y;
					//relocate
					float penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
					{
						// move ball up
						Ball->Position.y -= penetration;
					}
					else
					{
						// move ball down
						Ball->Position.y += penetration;
					}
				}
			}
		}
	}

	// check collisions for player pad
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// check where hit was on board, change velocity based on where it hit
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);

		// move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		// Keep consistance speed between both axises
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
	}
}

bool CheckCollision(GameObject& one, GameObject& two)
{
	// collision x-axis
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// collision only if on both axises
	return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, GameObject &two)
{
	glm::vec2 center(one.Position + one.Radius);
	glm::vec2 aabb_half_entents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;

	if (glm::length(difference) < one.Radius)
	{
		return std::make_tuple(true, VectorDirection(difference), difference);
	}
	else
	{
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
	}
}

Direction VectorDirection(glm::vec2 closest)
{
	glm::vec2 compass[] =
	{
		// UP
		glm::vec2(0.0f, 1.0f),
		// RIGHT
		glm::vec2(1.0F, 0.0F),
		// DOWN
		glm::vec2(0.0f, -1.0f),
		// LEFT
		glm::vec2(-1.0F, 0.0F),
	};

	float max = 0.0f;
	unsigned int best_match = -1;

	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}

	return (Direction)best_match;
}