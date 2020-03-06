#include "SFML\Graphics.hpp"
#include <string>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float CELL_SIZE = 10;
const int FRAME_RATE = 60;

void WorldToScreen(float worldX, float worldY, int &screenX, int &screenY, float offsetX, float offsetY, float scaleX, float scaleY);
void ScreenToWorld(int screenX, int screenY, float &worldX, float &worldY, float offsetX, float offsetY, float scaleX, float scaleY);

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Polymorphism");
	window.setFramerateLimit(FRAME_RATE);

	sf::CircleShape circle;
	
	sf::Mouse mouse;

	float offsetX = -WINDOW_WIDTH / 2;
	float offsetY = -WINDOW_HEIGHT / 2;

	float startPanX = 0.0;
	float startPanY = 0.0;
	bool panning = false;

	float scaleX = 1.0;
	float scaleY = 1.0;

	float selectedCellX = 0.0;
	float selectedCellY = 0.0;

	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				window.close();
			}
		}

		// Update

		float mouseX = (float)mouse.getPosition(window).x;
		float mouseY = (float)mouse.getPosition(window).y;

		// Capture location when user starts to pan
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !panning)
		{
			startPanX = mouseX;
			startPanY = mouseY;

			panning = true;
		}
		// As the user moves, the screen location changes. Convert screen coordinates to world coordinates to implement pan
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			offsetX -= (mouseX - startPanX) / scaleX;
			offsetY -= (mouseY - startPanY) / scaleY;

			// start new pan
			startPanX = mouseX;
			startPanY = mouseY;
		}
		// After user ends panning, reset panning to false
		if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			panning = false;
		}


		float mouseWorldX_BeforeZoom, mouseWorldY_BeforeZoom;
		ScreenToWorld(mouseX, mouseY, mouseWorldX_BeforeZoom, mouseWorldY_BeforeZoom, offsetX, offsetY, scaleX, scaleY);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			scaleX *= 1.005;
			scaleY *= 1.005;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			scaleX *= 0.995;
			scaleY *= 0.995;
		}

		float mouseWorldX_AfterZoom, mouseWorldY_AfterZoom;
		ScreenToWorld(mouseX, mouseY, mouseWorldX_AfterZoom, mouseWorldY_AfterZoom, offsetX, offsetY, scaleX, scaleY);

		offsetX += (mouseWorldX_BeforeZoom - mouseWorldX_AfterZoom);
		offsetY += (mouseWorldY_BeforeZoom - mouseWorldY_AfterZoom);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			selectedCellX = (int)(mouseWorldX_AfterZoom / CELL_SIZE);
			selectedCellY = (int)(mouseWorldY_AfterZoom / CELL_SIZE);
		}

		// Clear Window
		window.clear();

		// Clip offscreen objects
		float worldLeft, worldTop, worldRight, worldBottom;
		ScreenToWorld(0, 0, worldLeft, worldTop, offsetX, offsetY, scaleX, scaleY);
		ScreenToWorld(WINDOW_WIDTH, WINDOW_HEIGHT, worldRight, worldBottom, offsetX, offsetY, scaleX, scaleY);

		int linesDrawn = 0;

		// Draw objects
		// Draw horizontal lines
		for (float y = 0.0; y <= 10.0; y++)
		{
			if (y >= worldTop / CELL_SIZE && y <= worldBottom / CELL_SIZE)
			{
				float sx = 0.0 * CELL_SIZE, sy = y * CELL_SIZE;
				float ex = 10.0 * CELL_SIZE, ey = y * CELL_SIZE;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy, offsetX, offsetY, scaleX, scaleY);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey, offsetX, offsetY, scaleX, scaleY);

				sf::Vertex Line[] = { sf::Vector2f(pixel_sx, pixel_sy), sf::Vector2f(pixel_ex, pixel_ey) };
				window.draw(Line, 2, sf::Lines);
				linesDrawn++;
			}
		}
		// Draw vertical lines
		for (float x = 0.0; x <= 10.0; x++)
		{
			if (x >= worldLeft / CELL_SIZE && x <= worldRight / CELL_SIZE)
			{
				float sx = x * CELL_SIZE, sy = 0.0 * CELL_SIZE;
				float ex = x * CELL_SIZE, ey = 10.0 * CELL_SIZE;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy, offsetX, offsetY, scaleX, scaleY);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey, offsetX, offsetY, scaleX, scaleY);

				sf::Vertex Line[] = { sf::Vector2f(pixel_sx, pixel_sy), sf::Vector2f(pixel_ex, pixel_ey) };
				window.draw(Line, 2, sf::Lines);
				linesDrawn++;
			}
		}

		int cx, cy, cr;
		WorldToScreen(selectedCellX * CELL_SIZE, selectedCellY * CELL_SIZE, cx, cy, offsetX, offsetY, scaleX, scaleY);
		cr = (CELL_SIZE / 2) * scaleX;
		circle.setRadius(cr);
		circle.setPosition(cx, cy);
		circle.setFillColor(sf::Color::Red);
		window.draw(circle);

		sf::Font font;
		font.loadFromFile("Roboto-Medium.ttf");
		sf::Text text("Lines drawn: " + std::to_string(linesDrawn), font);
		text.setCharacterSize(20);
		text.setPosition(0, WINDOW_HEIGHT / 16);
		text.setStyle(sf::Text::Bold);
		text.setFillColor(sf::Color::Green);
		window.draw(text);

		window.display();
	}

	return 0;
}

void WorldToScreen(float worldX, float worldY, int &screenX, int &screenY, float offsetX, float offsetY, float scaleX, float scaleY)
{
	screenX = (int)((worldX - offsetX) * scaleX);
	screenY = (int)((worldY - offsetY) * scaleY);
}

void ScreenToWorld(int screenX, int screenY, float &worldX, float &worldY, float offsetX, float offsetY, float scaleX, float scaleY)
{
	worldX = (float)(screenX / scaleX) + offsetX;
	worldY = (float)(screenY / scaleY) + offsetY;
}
