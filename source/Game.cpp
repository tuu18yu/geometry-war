#include "Game.h"

#include <iostream>
#include <fstream>

constexpr auto ROTATION_ANGLE = 2.5f;
constexpr auto SPECIAL_ATTACK_COOLDOWN = 100;
constexpr auto BLACKHOLE_RADIUS = 100;
constexpr auto BLACKHOLE_THICKNESS = 4;
constexpr auto BLACKHOLE_LIFESPAN = 50;
constexpr int BLACKHOLE_OUTLINE_COLOR[3] = { 255, 255, 255 };

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	// Variables

	// Read config file
	std::ifstream ifile("../bin/config.txt");
	if (ifile.is_open())
	{
		std::string lineName;
		while (ifile >> lineName)
		{
			if (lineName == "Window")
			{
				int wWidth = 0, wHeight = 0, frameLimit = 0, FS = 0;
				ifile >> wWidth >> wHeight >> frameLimit >> FS;

				// Create full window if FS is true
				if (FS == 1) { m_window.create(sf::VideoMode(), "Assignment 2", sf::Style::Fullscreen); }
				else{ m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2"); }

				m_window.setFramerateLimit(frameLimit);
			}
			else if (lineName == "Font")
			{
				std::string F;
				int S = 0, R = 0, G = 0, B = 0;
				ifile >> F >> S >> R >> G >> B;

				// Load font from a file
                if (!m_font.loadFromFile(F))
                {
                    std::cerr << "Font is not loaded..";
                    exit(-1);
                }
				// set m_text 
				m_text.setFont(m_font);
				m_text.setCharacterSize(S);
				m_text.setFillColor(sf::Color(R, G, B));
			}
			else if (lineName == "Player")
			{
				ifile >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG
					>> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
					>> m_playerConfig.OT >> m_playerConfig.V;
			}
			else if (lineName == "Enemy")
			{
				ifile >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR
					>> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
					>> m_enemyConfig.L >> m_enemyConfig.SI;
			}
			else if (lineName == "Bullet")
			{
				ifile >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR
					>> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG
					>> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
			}
		}

		spawnPlayer();
	}
	ifile.close();
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();

		sUserInput();

		if (!m_paused)
		{
			sEnemySpawner();
			sMovement();
			sCollision();
			sLifespan();
		}

		sRender();

		// increment the current frame
		m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	m_paused = !paused;
}

void Game::sMovement()
{
	// handle player movement
	m_player->cTransform->velocity.x = 0;
	m_player->cTransform->velocity.y = 0;

	if (m_player->cInput->up)
	{
		m_player->cTransform->velocity.y = -m_playerConfig.S;
	}
	if (m_player->cInput->down)
	{
		m_player->cTransform->velocity.y = m_playerConfig.S;
	}
	if (m_player->cInput->right)
	{
		m_player->cTransform->velocity.x = m_playerConfig.S;
	}
	if (m_player->cInput->left)
	{
		m_player->cTransform->velocity.x = -m_playerConfig.S;
	}

	// Check if movement hits the borders, if it does set the velocity to 0
	Vec2 futurePos(m_player->cTransform->pos.x + m_player->cTransform->velocity.x, m_player->cTransform->pos.y + m_player->cTransform->velocity.y);
	if (futurePos.x - m_playerConfig.CR <= 0 || futurePos.x + m_playerConfig.CR >= m_window.getSize().x)
	{
		m_player->cTransform->velocity.x = 0;
	}
	if (futurePos.y - m_playerConfig.CR <= 0 || futurePos.y + m_playerConfig.CR >= m_window.getSize().y)
	{
		m_player->cTransform->velocity.y = 0;
	}

	// update player's movement
	m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

	for (auto& e : m_entities.getEntities())
	{
		if (e->tag() != "player" && e->cTransform && !e->paused)
		{
			e->cTransform->pos.x += e->cTransform->velocity.x;
			e->cTransform->pos.y += e->cTransform->velocity.y;
		}
	}
}
void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// When user clicks the close button
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// When keyboard in pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
			case sf::Keyboard::Up:
				m_player->cInput->up = true;
				break;

			case sf::Keyboard::S:
			case sf::Keyboard::Down:
				m_player->cInput->down = true;
				break;

			case sf::Keyboard::D:
			case sf::Keyboard::Right:
				m_player->cInput->right = true;
				break;

			case sf::Keyboard::A:
			case sf::Keyboard::Left:
				m_player->cInput->left = true;
				break;


			default: break;
			}
		}

		// When keyboard is release
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
			case sf::Keyboard::Up:
				m_player->cInput->up = false;
				break;

			case sf::Keyboard::S:
			case sf::Keyboard::Down:
				m_player->cInput->down = false;
				break;

			case sf::Keyboard::D:
			case sf::Keyboard::Right:
				m_player->cInput->right = false;
				break;

			case sf::Keyboard::A:
			case sf::Keyboard::Left:
				m_player->cInput->left = false;
				break;

			case sf::Keyboard::Escape:
				m_running = false;
				break;

			case sf::Keyboard::P:
				setPaused(m_paused);
				break;

			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				spawnBullet(m_player, mousePos);
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				spawnSpecialWeapon(mousePos);
			}
		}
	}
}
void Game::sLifespan()
{
	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan && e->cShape && !e->paused)
		{
			// if there is no remaining time remove the entity
			if (e->cLifespan->remaining <= 0)
			{
				e->destroy();

				if (e->tag() == "blackHole")
				{
					for (auto& enemy : m_entities.getEntities("enemy"))
					{
						// entities collide when their length of distance is less or equal than the sum of their radius
						Vec2 distance = enemy->cTransform->pos - e->cTransform->pos;
						if (distance.getLength() <= enemy->cCollision->radius + e->cCollision->radius)
						{
							std::cout << "Before blackhole disappeared: " << e->paused << "\n";
							e->paused = !e->paused;
							std::cout << "After blackhole disappeared: " << e->paused << "\n";
						}

					}
				}
				break;
			}

			// change transparency of entity depending on the remaining time
			float alphaRatio = (float) e->cLifespan->remaining / (float) e->cLifespan->total;
			sf::Uint8 alpha = 255 * alphaRatio;
			sf::Color eColor = e->cShape->circle.getFillColor();
			eColor.a = alpha;
			if (e->tag() != "blackHole") { e->cShape->circle.setFillColor(eColor); }

			sf::Color oColor = e->cShape->circle.getOutlineColor();
			oColor.a = alpha;
			e->cShape->circle.setOutlineColor(oColor);

			// record remaining frame
			e->cLifespan->remaining -= 1;
		}
	}
}

void Game::sRender()
{
	m_window.clear();

	m_text.setString("Score: " + std::to_string(m_score));
	m_text.setPosition(5, 5);
	m_window.draw(m_text);
	
	for (auto& e : m_entities.getEntities())
	{
		if (e->cShape && e->cTransform)
		{
			// Set the position of the entities based on the transform component
			e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

			// Set the roatation of the entities based on the transform component
			e->cShape->circle.rotate(e->cTransform->angle);

			// Draw the entities shape on the window
			m_window.draw(e->cShape->circle);
		}
	}

	m_window.display();
}

void Game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}
void Game::sCollision()
{	
	// Enemy collision with border or enemy collides with player
	for (auto& e : m_entities.getEntities("enemy"))
	{
		Vec2 currPos(e->cTransform->pos.x, e->cTransform->pos.y);
		Vec2 wSize(m_window.getSize().x, m_window.getSize().y);
		float CR = e->cCollision->radius;

		// if next move hits top of the window or the bottom of the window
		if (currPos.y - CR + e->cTransform->velocity.y <= 0.0f || currPos.y + CR + e->cTransform->velocity.y >= wSize.y)
		{
			e->cTransform->velocity.y *= -1.0f;
		}
		// if next move hits left of the window or right of the window
		if (currPos.x - CR + e->cTransform->velocity.x <= 0.0f || currPos.x + CR + e->cTransform->velocity.x >= wSize.x)
		{
			e->cTransform->velocity.x *= -1.0f;
		}

		// When enemy collides with player, player spawns back to the middle
		Vec2 distance = m_player->cTransform->pos - currPos;
		if (distance.getLength() <= m_player->cCollision->radius + e->cCollision->radius)
		{
			m_player->cTransform->pos.x = m_window.getSize().x / 2.0;
			m_player->cTransform->pos.y = m_window.getSize().y / 2.0;
		}

	}

	// Bullet collides with Enemies then destroy the bullet and the enemy
	for (auto& b : m_entities.getEntities("bullet"))
	{
		for (auto& e : m_entities.getEntities("enemy"))
		{
			// entities collide when their length of distance is less or equal than the sum of their radius
			Vec2 distance = b->cTransform->pos - e->cTransform->pos;
			if (distance.getLength() <= b->cCollision->radius + e->cCollision->radius)
			{
				b->destroy();
				e->destroy();
				m_score += e->cScore->score;

				if (!e->cLifespan)
				{
					spawnSmallEnemies(e);
				}
			}
			
		}
	}

	// When blackhole collides with the enemy pause the enemy
	for (auto& b : m_entities.getEntities("blackHole"))
	{
		for (auto& e : m_entities.getEntities("enemy"))
		{
			// entities collide when their length of distance is less or equal than the sum of their radius
			Vec2 distance = b->cTransform->pos - e->cTransform->pos;
			if (distance.getLength() <= b->cCollision->radius + e->cCollision->radius)
			{
				e->paused = true;
			}

		}
	}

}

void Game::spawnPlayer()
{
	std::shared_ptr<Entity> entity = m_entities.addEntity("player");

	// Spawn player at the center of the window
	float centerX = m_window.getSize().x / 2.0, centerY = m_window.getSize().y / 2.0;
	entity->cTransform = std::make_shared<CTransform>(Vec2(centerX, centerY), Vec2(0.0f, 0.0f), ROTATION_ANGLE, m_playerConfig.S);

	// Set shape to the Player based on config file
	sf::Color fColor(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);
	sf::Color oColor(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, fColor, oColor,
		m_playerConfig.OT);

	// Add input component to the Player entity in order to recieve inputs from user
	entity->cInput = std::make_shared<CInput>();

	// Add collision component to detect collision
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// set entity to be the player of the game
	// NOTE: This goes slightly against the EntityManager paradigm as Game manages the player entity 
	m_player = entity;
}

void Game::spawnEnemy()
{
	// Enemy spawns in a random location within the [radius, window - radius]
	float ePosX = rand() % (m_window.getSize().x - (2 * m_enemyConfig.CR) + 1);
	ePosX += m_enemyConfig.CR;
	float ePosY = rand() % (m_window.getSize().y - (2 * m_enemyConfig.CR) + 1);
	ePosY += m_enemyConfig.CR;
	Vec2 ePos(ePosX, ePosY);

	// Enemy has random verticies between VMIN and VMAX inclusive
	int verticies = rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1);
	verticies += m_enemyConfig.VMIN;

	// Enemy has random speed between SMIN and SMAX inclusive
	float speed = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_enemyConfig.SMAX - m_enemyConfig.SMIN)));
	speed += m_enemyConfig.SMIN;
	
	// Initially enemy will move in a random angle
	int angle = rand() % 360;
	float velX = speed * cos(angle), velY = speed * sin(angle);
	Vec2 eVel(velX, velY);

	// Enemy has random fill color and given outline color
	int R = rand() % 255, G = rand() % 255, B = rand() % 255;
	sf::Color fColor(R, G, B);
	sf::Color oColor(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);

	// add enemy to the EntityManager
	std::shared_ptr<Entity> entity = m_entities.addEntity("enemy");
	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, verticies, fColor, oColor, m_enemyConfig.OT);
	entity->cTransform = std::make_shared<CTransform>(ePos, eVel, ROTATION_ANGLE, speed);
	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
	entity->cScore = std::make_shared<CScore>(verticies * 100);

	// record the most recent enemy spawn time
	m_lastEnemySpawnTime = m_currentFrame;
}
void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	// Small enemies spawn in the middle of the parent enemy
	Vec2 sPos(entity->cTransform->pos.x, entity->cTransform->pos.y);

	// Inherits shape from parent, but half the size
	sf::Color fColor = entity->cShape->circle.getFillColor();
	sf::Color oColor = entity->cShape->circle.getOutlineColor();
	int OT = entity->cShape->circle.getOutlineThickness();
	int radius = entity->cShape->circle.getRadius() / 2;
	int nVerticies = entity->cShape->circle.getPointCount();

	// Moves in different directions
	int currAngle = 0;
	int increaseAngle = 360 / nVerticies;
	
	// Create nVerticies of small enemies
	for (int i = 0; i < nVerticies; i++)
	{
		std::shared_ptr<Entity> smallEnemy = m_entities.addEntity("enemy");
		smallEnemy->cShape = std::make_shared<CShape>(radius, nVerticies, fColor, oColor, OT);
		smallEnemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR / 2);
		smallEnemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
		smallEnemy->cScore = std::make_shared<CScore>(entity->cScore->score * 2);

		Vec2 sVel(entity->cTransform->speed * cos(currAngle), entity->cTransform->speed * sin(currAngle));
		smallEnemy->cTransform = std::make_shared<CTransform>(sPos, sVel, ROTATION_ANGLE, entity->cTransform->speed);

		currAngle += increaseAngle;
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
	// Bullet spawns in middle of the given entity
	Vec2 bPos(entity->cTransform->pos.x, entity->cTransform->pos.y);

	// Bullet moves to the location of the mouse at given speed
	Vec2 bVel = mousePos - bPos;
	bVel.normalize();
	bVel *= m_bulletConfig.S;

	// Given fill and out color for the bullet
	sf::Color fColor(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB);
	sf::Color oColor(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB);

	// add bullet to the EntityManager
	std::shared_ptr<Entity> bullet = m_entities.addEntity("bullet");
	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, fColor, oColor, m_bulletConfig.OT);
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	bullet->cTransform = std::make_shared<CTransform>(bPos, bVel, ROTATION_ANGLE, m_bulletConfig.S);
	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);

}
void Game::spawnSpecialWeapon(const Vec2& mousePos)
{
	// Black Hole: within some radius captures the enemies

	// Given fill and out color for the bullet
	sf::Color oColor(BLACKHOLE_OUTLINE_COLOR[0], BLACKHOLE_OUTLINE_COLOR[1], BLACKHOLE_OUTLINE_COLOR[2]);
	sf::Color fColor(BLACKHOLE_OUTLINE_COLOR[0], BLACKHOLE_OUTLINE_COLOR[1], BLACKHOLE_OUTLINE_COLOR[2], 0);

	// add bullet to the EntityManager
	std::shared_ptr<Entity> bullet = m_entities.addEntity("blackHole");
	bullet->cShape = std::make_shared<CShape>(BLACKHOLE_RADIUS, 20, fColor, oColor, BLACKHOLE_THICKNESS);
	bullet->cTransform = std::make_shared<CTransform>(mousePos, Vec2(0.0f, 0.0f), ROTATION_ANGLE, 0);
	bullet->cLifespan = std::make_shared<CLifespan>(BLACKHOLE_LIFESPAN);
	bullet->cCollision = std::make_shared<CCollision>(BLACKHOLE_RADIUS + BLACKHOLE_THICKNESS);
}