#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <random>
#include <time.h>
#include <stdlib.h> 
#include <string>
#include <math.h>
using namespace std;

#define MAX_SPEED = 5.0;
// 60 FPS (1.0f/60.0f)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
float timeLeftOver = 0.0f;

enum direction
{
	LEFT,RIGHT,TOP,BOTTOM,NONE
};

class SheetSprite
{
public:
	SheetSprite() :
		textureID(0), u(0), v(v), width(0), height(0){}
	SheetSprite(unsigned int textureID, float u, float v, float width, float height) :
		textureID(textureID), u(u), v(v), width(width), height(height){}
	float scale;
	unsigned int textureID;
	float x;
	float y;
	float u;
	float v;
	float width;
	float height;

	void Draw(float x, float y, float rotation = 0.0, float scale = 0.5)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslatef(x, y, 0.0);
		glRotatef(rotation, 0.0, 0.0, 1.0);
		GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale,
			width * scale, -height * scale, width * scale, height * scale };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);
		GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
	}
};

void DrawSpriteSheetSprite(int spriteTexture, int index, int spriteCountX, int spriteCountY, float x, float y, float rotation = 0.0, float scale = 0.5)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spriteTexture);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);
	GLfloat quad[] = { -0.15*scale, 0.15f*scale, -0.15f*scale, -0.15f*scale, 0.15f*scale, -0.15f*scale, 0.15f*scale, 0.15f*scale };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;
	GLfloat quadUVs[] = { u, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight,
		u + spriteWidth, v
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
}

void DrawSprite(GLint texture, float x, float y, float rotation = 0.0)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -0.15, 0.15f, -0.15f, -0.15f, 0.15f, -0.15f, 0.15f, 0.15f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
}

GLuint LoadTexture(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);
	return textureID;
}

void DrawText(GLuint fontTexture, string text, float size, float spacing, float r, float g, float b, float a, float x, float y)
{
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;
	for (int i = 0; i < text.size(); i++)
	{
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) +
			(-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f
			* size });
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x +
			texture_size, texture_y + texture_size, texture_x + texture_size, texture_y });
	}
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4.0);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

float lerp(float v0, float v1, float t)
{
	return (1.0 - t)*v0 + t*v1;
}

class Entity
{
public:
	Entity(float x, float y, float w, float h) :x(x), y(y), width(w), height(h), top(false), bottom(false), right(false), left(false){}
	Entity() :x(0.0), y(0.0), width(0.0), height(0.0), top(false), bottom(false), right(false), left(false){}
	float x;
	float y;
	float width;
	float height;
	bool top;
	bool bottom;
	bool right;
	bool left;
};

class Player : public Entity
{
public:
	Player() :Entity(0,-0.5,0.15/2,0.15), x_velocity(0.0f), y_velocity(0.0f), x_accelartion(0.0f), y_accelartion(0.0f){}
	float x_velocity;
	float y_velocity;
	float x_accelartion;
	float y_accelartion;
	vector<direction> collision;
};



class Block : public Entity
{
public:
	Block() :Entity(){}
	Block(float x, float y,float w, float h) :Entity(x,y,w,h){}

};

class Snake : public Entity
{
public:
	Snake() :Entity(){}
	Snake(float x, float y, float w, float h) :Entity(x, y, w, h){}
};

class Game
{
private:
	bool done;
	SDL_Window* displayWindow;
	vector<SheetSprite> SpriteSheet;
	vector<unsigned int> Sprite;
	vector<Block> blocks;
	vector<Snake> snakes;
	vector<unsigned int> font;
	Player player;
	float elapsed;
	int run_animation_player[8];
	int numFrames;
	int score;
	float animationElapsed;
	float framesPerSecond;
	float lastFrameTicks;
	float gravity;
	int current_index_player_animation;
	float x_friction;
	float y_friction;
public:
	void initialize();
	void render();
	void update();
	void draw_ground();
	void animation_update();
	void player_update();
	void collision();
	void player_block_collision();
	void player_snake_collision();
	void draw_wall();
	void draw_snake();
	bool is_done();
	bool entity_collide(const Entity &entity1, const Entity &entity2); 
	bool entity_block_collide(Entity &entity1, Entity &entity2);
	float y_penetration(const Entity& entity_1, const Entity& entity_2);
	float x_penetration(const Entity& entity_1, const Entity& entity_2);
};

void Game::draw_ground()
{
	for (int i = 0; i <= 4; i++)
	{
		blocks.push_back(Block((float(i) / 2.2) - 0.8, -0.93, 220.0 / 1024.0, 70.0 / 1024.0));
	}
	blocks.push_back(Block(-0.7, -0.8, 220.0 / 1024.0, 70.0 / 1024.0));
}

void Game::draw_wall()
{
	for (int i = 0; i <= 2; i++)
	{
		blocks.push_back(Block((float(i) / 1.0) - 1.0, -0.4 + i*0.2, 220.0 / 1024.0, 70.0 / 1024.0));
	}
}

/*bool Game::entity_collide(Entity &entity1, Entity &entity2)
{
	return (fabs(entity1.x - entity2.x) < (entity1.width + entity2.width)*0.5) && (fabs(entity1.y - entity2.y) < (entity1.height + entity2.height)*0.5);
}*/

bool Game::entity_collide(const Entity &entity1, const Entity &entity2)
{
	float left1 = entity1.x - entity1.width*0.5;
	float right1 = entity1.x + entity1.width*0.5;
	float top1 = entity1.y + entity1.height*0.5;
	float bottom1 = entity1.y - entity1.height*0.5;
	float left2 = entity2.x - entity2.width*0.5;
	float right2 = entity2.x + entity2.width*0.5;
	float top2 = entity2.y + entity2.height*0.5;
	float bottom2 = entity2.y - entity2.height*0.5;
	if (bottom1 >= top2)
	{	
		return false;
	}
	if (top1 <= bottom2)
	{
		return false;
	}
	if (right1 <= left2)
	{
		return false;
	}
	if (left1 >= right2)
	{
		return false;
	}
	return true;
}

void Game::draw_snake()
{
	for (int i = 0; i <= 2; i++)
	{
		snakes.push_back(Snake((float(i) / 1.0) - 1.0, -0.2 + i*0.2, 0.15 / 2, 0.15));
	}
}

void Game::initialize()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, 800, 600);
	bool done = false;
	for (int i = 0; i < 8; i++)
	{
		run_animation_player[i] = i + 8;
	}
	numFrames = 5;
	animationElapsed = 0.0f;
	framesPerSecond = 30.0f;
	elapsed = 0.0f;
	lastFrameTicks = 0.0f;
	current_index_player_animation = 0;
	x_friction = 3.0f;
	y_friction = 3.0f;
	gravity = 3.0f;
	score = 0;
	unsigned int static_entity = LoadTexture("metal.png");
	unsigned int animation = LoadTexture("characters.png");
	unsigned int font1 = LoadTexture("font1.png");
	SheetSprite road(static_entity, 0.0/1024.0, 140.0/1024.0, 220.0/1024.0, 70.0/1024.0);
	SpriteSheet.push_back(road);
	Sprite.push_back(animation);
	Sprite.push_back(font1);
	draw_ground();
	draw_wall();
	draw_snake();
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < blocks.size(); i++)
	{
		SpriteSheet[0].Draw(blocks[i].x, blocks[i].y, 0.0);
	}
	for (int i = 0; i < snakes.size(); i++)
	{
		DrawSpriteSheetSprite(Sprite[0], 25, 8, 4, snakes[i].x, snakes[i].y);
	}
	DrawSpriteSheetSprite(Sprite[0], run_animation_player[current_index_player_animation], 8, 4, player.x, player.y);
	DrawText(Sprite[1], "SCORE", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.7, 0.9);
	DrawText(Sprite[1], to_string(score), 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.1, 0.9);
	SDL_GL_SwapWindow(displayWindow);
}

void Game::animation_update()
{
	animationElapsed += player.x_accelartion;
	if (animationElapsed > 1.0 / framesPerSecond)
	{
		current_index_player_animation++;
		animationElapsed = 0.0;
		if (current_index_player_animation > numFrames - 1) 
		{
			current_index_player_animation = 0;
		}
	}
}

void Game::player_update()
{
	player.x_velocity = lerp(player.x_velocity, 0.0f, FIXED_TIMESTEP*x_friction);
	//player.y_velocity = lerp(player.y_velocity, 0.0f, FIXED_TIMESTEP*y_friction);
	player.x_velocity += player.x_accelartion * FIXED_TIMESTEP;
	player.y_velocity += -gravity * elapsed;
	float previous_x = player.x;
	float current_x = player.x - previous_x;
	animationElapsed += abs(current_x / 3.0);
	if (animationElapsed > 1.0 / framesPerSecond)
	{
		current_index_player_animation++;
		animationElapsed = 0.0;
		if (current_index_player_animation > numFrames - 1 || abs(player.x_velocity) <= 0.01)
		{
			current_index_player_animation = 0;
		}
	}
	

}

float Game::y_penetration(const Entity& entity_1, const Entity& entity_2)
{
	float y_distance = fabs(entity_1.y - entity_2.y);
	float y_penetration = fabs(y_distance - entity_1.height*0.5 - entity_2.height*0.5);
	return y_penetration;
}

float Game::x_penetration(const Entity& entity_1, const Entity& entity_2)
{
	float x_distance = fabs(entity_1.x - entity_2.x);
	float x_penetration = fabs(x_distance - entity_1.width*0.5 - entity_2.width*0.5);
	return x_penetration;
}

void Game::player_block_collision()
{
	player.bottom = false;
	player.top = false;
	player.left = false;
	player.right = false;
	/*for (int i = 0; i < blocks.size(); i++)
	{
		float hight = blocks[i].height;
		float width = blocks[i].height;
		float top = blocks[i].y + blocks[i].height;
		float bottom = blocks[i].y - blocks[i].height*0.5;
		float left = blocks[i].x - blocks[i].width*0.5;
		float right = blocks[i].x + blocks[i].width*0.5;
		if (player.x + player.width*0.5 > left && player.x + player.width*0.5< right && (fabs(player.y - blocks[i].y) < (player.height + blocks[i].height)*0.5))
		{
			if (player.x_velocity > 0)
				player.x_velocity = 0;
		}
		if (player.x - player.width*0.5 < right && player.x - player.width*0.5 > left && (fabs(player.y - blocks[i].y) < (player.height + blocks[i].height)*0.5))
		{
			if (player.x_velocity < 0)
				player.x_velocity = 0;
		}
		if (player.y - player.height*0.5 < top && player.y - player.height*0.5 >bottom && fabs(player.x - blocks[i].x) < (player.width + blocks[i].width)*0.5)
		{
			if (player.y_velocity < 0)
				player.y_velocity = 0;
		}
		if (player.y + player.height*0.5 > bottom && player.y + player.height * 0.5 < top && fabs(player.x - blocks[i].x) < (player.width + blocks[i].width)*0.5)
		{
			if (player.y_velocity > 0)
				player.y_velocity = 0;
		}
	}/*
	for (int i = 0; i < blocks.size(); i++)
	{
		if (y_penetration(player, blocks[i]) < 0.1)
		{
			if (x_penetration(player, blocks[i]) <= blocks[i].width*0.5)
				player.y_velocity = 0;
		}
	}*/

	player.y += player.y_velocity * FIXED_TIMESTEP;

	for (int i = 0; i < blocks.size(); i++)
	{
		if (entity_collide(player, blocks[i]))
		{
			if (player.y < blocks[i].y)
			{
				player.top = true;
				player.y -= y_penetration(player,blocks[i]) + 0.0001;
				player.y_velocity = 0;
			}
			if (player.y > blocks[i].y)
			{
				player.bottom = true;
				player.y += y_penetration(player, blocks[i]) + 0.0001;
				player.y_velocity = 0;
			}
		}
	}

	player.x += player.x_velocity * FIXED_TIMESTEP;

	for (int i = 0; i < blocks.size(); i++)
	{
		if (entity_collide(player, blocks[i]))
		{
			if (player.x < blocks[i].x)
			{
				player.right = true;
				player.x -= x_penetration(player, blocks[i]) + 0.0001;
			}
			if (player.x > blocks[i].x)
			{
				player.left = true;
				player.x += x_penetration(player, blocks[i]) + 0.0001;
			}
		}
	}
}

void Game::player_snake_collision()
{
	for (int i = 0; i < snakes.size(); i++)
	{
		if (entity_collide(player, snakes[i]))
		{
			score++;
			snakes.erase(snakes.begin() + i);
		}
		/*
		float hight = snakes[i].height;
		float width = snakes[i].height;
		float top = snakes[i].y + snakes[i].height*0.5;
		float bottom = snakes[i].y - snakes[i].height*0.5;
		float left = snakes[i].x - snakes[i].width*0.5;
		float right = snakes[i].x + snakes[i].width*0.5;
		if (player.x + player.width*0.5 > left && player.x + player.width*0.5< right && player.y < top && player.y > bottom)
		{
			score++;
			snakes.erase(snakes.begin() + i);
		}
		if (player.x - player.width*0.5 < right && player.x - player.width*0.5 > left && player.y < top && player.y > bottom)
		{
			score++;
			snakes.erase(snakes.begin() + i);
		}
		if (player.y - player.height*0.5 < top && player.y - player.height*0.5 >bottom && player.x < right && player.x > left)
		{
			score++;
			snakes.erase(snakes.begin() + i);
		}
		if (player.y + player.height*0.5 > bottom && player.y + player.height * 0.5 < top && player.x < right && player.x > left)
		{
			score++;
			snakes.erase(snakes.begin() + i);
		}*/
	}
}

void Game::collision()
{
	player_block_collision();
	player_snake_collision();
}

void Game::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			done = true;
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_UP)
			{
				if (player.bottom)
					player.y_velocity = 2.0;
			}
		}
	}
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT])
	{
		player.x_accelartion = -2.0f;
	}
	else if (keys[SDL_SCANCODE_RIGHT])
	{
		player.x_accelartion = 2.0f;
	}
	else
	{
		player.x_accelartion = 0.0f;
	}
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) 
	{
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) 
	{
		fixedElapsed -= FIXED_TIMESTEP;
		collision();
	}
	timeLeftOver = fixedElapsed;
	//Update(elapsed);
	//animation_update();
	player_update();
	//collision();
}

bool Game::is_done()
{
	return done;
}




int main(int argc, char *argv[])
{
	Game Assignmeng_4;
	Assignmeng_4.initialize();
	while (!Assignmeng_4.is_done())
	{
		Assignmeng_4.render();
		Assignmeng_4.update();
	}
	SDL_Quit();
	return 0;
}