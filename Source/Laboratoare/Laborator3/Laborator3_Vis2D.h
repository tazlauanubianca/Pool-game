#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>

#define B 120
#define P 20
#define H 50

class Laborator3_Vis2D : public SimpleScene
{
	public:
		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		struct LogicSpace
		{
			LogicSpace() : x(0), y(0), width(1), height(1) {}
			LogicSpace(float x, float y, float width, float height)
				: x(x), y(y), width(width), height(height) {}
			float x;
			float y;
			float width;
			float height;
		};

	public:
		Laborator3_Vis2D();
		~Laborator3_Vis2D();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

		void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

	protected:
		float length;
		ViewportSpace viewSpace;
		LogicSpace logicSpace;
		glm::mat3 modelMatrix, visMatrix;
		glm::vec3 corner_rectangle = glm::vec3(0.001f, 0.001f, 0);

		float width = 60;
		float height = 30;

		float width_wall_up = 1280;
		float height_wall_up = 30;

		float width_wall_side = 30;
		float height_wall_side = 720 - 60;

		float width_platform = 300;
		float height_platform = 20;

		float platform_x = 1280 / 2 - 150;
		float platform_y = 0;

		float radius = 10;
		float ball_x = 0;
		float ball_y = 0;

		bool release = false;
		float cos_direction = 0;
		float direction_x;
		float direction_y;
		float speed = 5;
		int number_of_lives = 3;
		int number_of_bricks = B;
		float currentTime = 0;
};
