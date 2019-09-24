#include "Laborator3_Vis2D.h"

#include <vector>
#include <iostream>
#include <time.h>

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"

using namespace std;

Laborator3_Vis2D::Laborator3_Vis2D()
{
}

Laborator3_Vis2D::~Laborator3_Vis2D()
{
}

Mesh* CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(width, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };
	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* CreateCircle(std::string name, glm::vec3 center, float radius, glm::vec3 color, bool fill) {
	float x, y;
	float px = radius, py = 0;

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;
	vertices.push_back(VertexFormat(center, color));
	vertices.push_back(VertexFormat(glm::vec3(center.x + px, center.y + py, 0), color));

	for (int i = 1; i <= 360; i++) {
		x = center.x + radius * cos(3.14f / 180 * i);
		y = center.y + radius * sin(3.14f / 180 * i);
		vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
		indices.push_back(0);
		indices.push_back(i - 1);
		indices.push_back(i);
		px = x;
		py = y;
	}

	indices.push_back(0);
	indices.push_back(360);
	indices.push_back(1);

	Mesh* circle = new Mesh(name);
	if (!fill) {
		circle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		indices.push_back(0);
		indices.push_back(2);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}

typedef struct {
	float corner_x;
	float corner_y;
	char status;   // 0 for dead, 1 for normal, 2 for fading, 3 for hard brick
	float fading_time;
	float scalling_factor;
	bool has_powerup;
	int powerup;
} Brick;

typedef struct {
	float x;
	float y;
	bool falling;
	float rotateangle;
	int type;
	bool activated;
	float activation_time;
} Powerup;

Brick bricks[B];
Powerup powerups[P];

void Laborator3_Vis2D::Init()
{
	int index = 0;
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	logicSpace.x = 0;		// logic x
	logicSpace.y = 0;		// logic y
	logicSpace.width = 4;	// logic width
	logicSpace.height = 4;	// logic height

	glm::ivec2 resolution = window->GetResolution();

	Mesh* rectangle = CreateRectangle("normal_rectangle", corner_rectangle, width, height, glm::vec3(1, 0, 0), true);
	AddMeshToList(rectangle);

	Mesh* rectangle_hard = CreateRectangle("hard_rectangle", corner_rectangle, width, height, glm::vec3(1, 0.5, 0.5), true);
	AddMeshToList(rectangle_hard);

	Mesh* rectangle_bonus = CreateRectangle("bonus_rectangle", corner_rectangle, width, height, glm::vec3(1, 0.5, 0), true);
	AddMeshToList(rectangle_bonus);

	Mesh* bonus1 = CreateRectangle("bonus1", corner_rectangle, width / 3, width / 3, glm::vec3(1, 0.3, 0.5), true);
	AddMeshToList(bonus1);

	Mesh* bonus2 = CreateRectangle("bonus2", corner_rectangle, width / 3, width / 3, glm::vec3(0.6, 0.3, 0.5), true);
	AddMeshToList(bonus2);

	Mesh* wall_up = CreateRectangle("wall_up", corner_rectangle, width_wall_up, height_wall_up, glm::vec3(1, 0, 0), true);
	AddMeshToList(wall_up);

	Mesh* wall_side = CreateRectangle("wall_side", corner_rectangle, width_wall_side, height_wall_side, glm::vec3(1, 0, 0), true);
	AddMeshToList(wall_side);

	Mesh* platform = CreateRectangle("platform", corner_rectangle, width_platform, height_platform, glm::vec3(1, 1, 1), true);
	AddMeshToList(platform);

	Mesh* ball = CreateCircle("circle", glm::vec3(ball_x, ball_y, 0), radius, glm::vec3(0, 0.3, 0.5), true);
	AddMeshToList(ball);

	ball_x = platform_x + width_platform / 2;
	ball_y = height_platform + radius;

	for (int i = 600; i > 200; i -= 40) {
		for (int j = 250; j < 1090; j += 70) {
			bricks[index].corner_y = i;
			bricks[index].corner_x = j;
			bricks[index].status = 1;
			bricks[index].has_powerup = false;

			index++;
		}
	}
	srand(time(NULL));

	for (int i = 0; i < H; i++) {
		int random_number = std::rand() % B;
		bricks[random_number].status = 3;
	}

	for (int i = 0; i < P; i++) {
		int random_number = std::rand() % B;
		bricks[random_number].has_powerup = true;
		bricks[random_number].powerup = i;

		powerups[i].falling = false;
		powerups[i].x = bricks[random_number].corner_x + width / 2;
		powerups[i].y = bricks[random_number].corner_y + height / 2;
		powerups[i].rotateangle = 0;
		powerups[i].type = random_number % 2;
		powerups[i].activated = false;
	}
}

void Laborator3_Vis2D::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

void Laborator3_Vis2D::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Laborator3_Vis2D::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->GetResolution();
	currentTime += deltaTimeSeconds;

	// Sets the screen area where to draw
	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	SetViewportArea(viewSpace, glm::vec3(0), true);

	// Draw the bricks
	for (int i = 0; i < B; i++) {
		if (bricks[i].status != 0) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(bricks[i].corner_x, bricks[i].corner_y);

			if (bricks[i].status == 1) {
				if (bricks[i].has_powerup) {
					RenderMesh2D(meshes["bonus_rectangle"], shaders["VertexColor"], modelMatrix);
				}
				else {
					RenderMesh2D(meshes["normal_rectangle"], shaders["VertexColor"], modelMatrix);
				}
			} 
			else if (bricks[i].status == 3) {
				RenderMesh2D(meshes["hard_rectangle"], shaders["VertexColor"], modelMatrix);
			}
			else {
				float deltaTime = currentTime - bricks[i].fading_time;

				if (deltaTime < 0.3) {
					modelMatrix *= Transform2D::Translate(width / 2, height / 2);
					modelMatrix *= Transform2D::Scale(bricks[i].scalling_factor, bricks[i].scalling_factor);
					modelMatrix *= Transform2D::Translate(-width / 2, -height / 2);
					
					RenderMesh2D(meshes["normal_rectangle"], shaders["VertexColor"], modelMatrix);
					bricks[i].scalling_factor -= deltaTimeSeconds * 3;
				}
				else {
					bricks[i].status = 0;
				}
			}
 		}
	}

	// Draw the upper wall
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, resolution.y - height_wall_up);
	RenderMesh2D(meshes["wall_up"], shaders["VertexColor"], modelMatrix);

	// Draw the wall on the left
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 60);
	RenderMesh2D(meshes["wall_side"], shaders["VertexColor"], modelMatrix);

	// Draw the wall on the right
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(resolution.x - width_wall_side, 60);
	RenderMesh2D(meshes["wall_side"], shaders["VertexColor"], modelMatrix);

	// Draw the platform
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(platform_x, platform_y);
	RenderMesh2D(meshes["platform"], shaders["VertexColor"], modelMatrix);

	// Draw the circle
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(ball_x, ball_y);
	RenderMesh2D(meshes["circle"], shaders["VertexColor"], modelMatrix);

	// Draw the lives
	for (int i = 0; i < number_of_lives; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(20 + i * 30, 30);
		RenderMesh2D(meshes["circle"], shaders["VertexColor"], modelMatrix);
	}

	// Draw powerups
	for (int i = 0; i < P; i++) {
		if (powerups[i].falling) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(powerups[i].x, powerups[i].y);
			modelMatrix *= Transform2D::Translate(width / 6, width / 6);
			modelMatrix *= Transform2D::Rotate(powerups[i].rotateangle += (deltaTimeSeconds));
			modelMatrix *= Transform2D::Translate(-width / 6, -width / 6);

			if (powerups[i].type == 1)
				RenderMesh2D(meshes["bonus1"], shaders["VertexColor"], modelMatrix);
			else if (powerups[i].type == 0)
				RenderMesh2D(meshes["bonus2"], shaders["VertexColor"], modelMatrix);

			powerups[i].y -= 2.5;
		}
	}

	// Activate powerups
	for (int i = 0; i < P; i++) {
		if (powerups[i].activated) {
			if (powerups[i].type == 0) {
				release = false;
				ball_x = platform_x + width_platform / 2;
				ball_y = height_platform + radius;
				powerups[i].activated = false;

			}

			if (powerups[i].type == 1) {
				if (currentTime - powerups[i].activation_time < 5) {
					speed = 10;
				}
				else {
					speed = 5;
					powerups[i].activated = false;
				}
			}
		}
	}

	if (release) {
		ball_x += direction_x;
		ball_y += direction_y;

		// Check for brick collision
		for (int i = 0; i < B; i++) {
			if (bricks[i].status == 1 || bricks[i].status == 3) {
				if (ball_x - radius <= bricks[i].corner_x + width && ball_x + radius >= bricks[i].corner_x &&
					ball_y - radius <= bricks[i].corner_y + height && ball_y + radius >= bricks[i].corner_y) {
					// Detect which side it touches

					if (ball_x + radius <= bricks[i].corner_x + 5) {
						direction_x *= -1;
					}

					if (ball_x - radius >= bricks[i].corner_x + width - 5) {
						direction_x *= -1;
					}

					if (ball_y - radius >= bricks[i].corner_y - 5) {
						direction_y *= -1;
					}

					if (ball_y + radius <= bricks[i].corner_y + height + 5) {
						direction_y *= -1;
					}

					if (bricks[i].status == 1) {
						bricks[i].status = 2;
						bricks[i].fading_time = currentTime;
						bricks[i].scalling_factor = 1;
						number_of_bricks--;

						if (bricks[i].has_powerup) {
							powerups[bricks[i].powerup].falling = true;
							bricks[i].has_powerup = false;
						}
					}
					else {
						bricks[i].status = 1;
					}
					
				}
			}
		}

		// Check ball - platform collision
		if (ball_x + radius >= platform_x && ball_x - radius <= platform_x + width_platform &&
			ball_y + radius >= platform_y && ball_y - radius <= platform_y + height_platform)
		{
			cos_direction = (ball_x - platform_x - width_platform / 2) / (width_platform / 2);
			if (cos_direction < -0.85f)
				cos_direction = -0.85f;

			if (cos_direction > 0.85f)
				cos_direction = 0.85f;

			if (cos_direction > 0) {
				direction_y = (1 - cos_direction) * speed;
				direction_x = cos_direction * speed;
			}
			else if (cos_direction < 0) {
				direction_y = (1 - fabs(cos_direction)) * speed;
				direction_x = -fabs(cos_direction) * speed;
			}
			else {
				direction_y *= -1;
			}
		}

		// Check for bonus - platform collision
		for (int i = 0; i < P; i++) {
			if (powerups[i].falling) {
				if (powerups[i].x + width / 6 >= platform_x && powerups[i].x - width / 6 <= platform_x + width_platform &&
					powerups[i].y + width / 6 >= platform_y && powerups[i].y - width / 6 <= platform_y + height_platform) {
					powerups[i].activated = true;
					powerups[i].falling = false;
					powerups[i].activation_time = currentTime;
				}
			}
		}
		// Check for collision with upper wall
		if (ball_y - radius < resolution.y && ball_y + radius >= resolution.y - height_wall_up) {
			direction_y *= -1;
		}

		// Check for collision with left wall
		if (ball_x + radius >= 0  && ball_x - radius <= width_wall_side &&
			ball_y + radius >= 60 && ball_y - radius <= resolution.y) {
			direction_x *= -1;
		}

		// Check for collision with right wall
		if (ball_x + radius >= resolution.x - width_wall_side && ball_x - radius <= resolution.x &&
			ball_y + radius >= 60 && ball_y - radius <= resolution.y) {
			direction_x *= -1;
		}

		// Check if out of bounds ball
		if (ball_x + radius > resolution.x || ball_y + radius > resolution.y || ball_y - radius < 0 || ball_x - radius < 0) {
			if (number_of_lives == 0) {
				release = false;

				if (number_of_bricks != 0) {
					cout << "AI PIERDUT!";
				}
				else {
					cout << "AI CASTIGAT";
				}

				return;
			}
			else {
				ball_x = platform_x + width_platform / 2;
				ball_y = height_platform + radius;
				release = false;
				number_of_lives--;
			}
		}

		// Check if out of bounds ball
		if (ball_x > resolution.x || ball_y > resolution.y || ball_y < 0 || ball_x < 0) {
			if (number_of_lives == 0) {
				release = false;

				if (number_of_bricks != 0) {
					cout << "AI PIERDUT!";
				}
				else {
					cout << "AI CASTIGAT";
				}

				return;
			}
			else {
				ball_x = platform_x + width_platform / 2;
				ball_y = height_platform + radius;
				release = false;
				number_of_lives--;
			}
		}
	}

	// Check for winning
	if (number_of_bricks == 0) {
		cout << "AI CASTIGAT!";
		return;
	}
}

void Laborator3_Vis2D::FrameEnd()
{

}

void Laborator3_Vis2D::OnInputUpdate(float deltaTime, int mods)
{

}

void Laborator3_Vis2D::OnKeyPress(int key, int mods)
{

}

void Laborator3_Vis2D::OnKeyRelease(int key, int mods)
{

}

void Laborator3_Vis2D::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (!release) {
		ball_x += deltaX;
	}
	
	platform_x += deltaX;
}

void Laborator3_Vis2D::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	release = true;
	direction_y = speed;
	direction_x = 0;
}

void Laborator3_Vis2D::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}

void Laborator3_Vis2D::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}