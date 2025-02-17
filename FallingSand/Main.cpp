#include "Main.h"
#include <SFML/Graphics.hpp>
#include "Universe.h"

#define WIDTH       600
#define HEIGHT      400
#define FRAMERATE	60

int main() {
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "FALLING SAND SIMULATION");
	window.setFramerateLimit(FRAMERATE);
	Universe universe(WIDTH, HEIGHT, window);

	sf::Texture textureBuffer;
	textureBuffer.create(WIDTH, HEIGHT);

	sf::Sprite sprite(textureBuffer);

	sf::Text text;
	sf::Font font; 
	font.loadFromFile("arial.ttf");
	text.setFont(font);
	text.setFillColor(sf::Color::White);

	auto mouseBox = sf::RectangleShape(Vec2f(1.f, 1.f));
	mouseBox.setOutlineColor(sf::Color::Red);
	mouseBox.setOutlineThickness(0.3f);
	mouseBox.setFillColor(sf::Color::Transparent);

	while (window.isOpen()) {
		sf::Event event{};
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		textureBuffer.update(&universe.pixels()[0]);

		window.clear();
		window.draw(sprite);

		text.setCharacterSize(18);
		text.setPosition(Vec2f(10, 10));
		text.setString(std::to_string((universe.elementCount)));
		window.draw(text);

		text.setCharacterSize(12);
		text.setPosition(Vec2f(10, 32));
		text.setString(std::to_string((universe.sandCount)));
		window.draw(text);

		text.setCharacterSize(12);
		text.setPosition(Vec2f(10, 46));
		text.setString(std::to_string((universe.gravelCount)));
		window.draw(text);

		mouseBox.setPosition((Vec2f)sf::Mouse::getPosition(window));
		window.draw(mouseBox);

		window.display();

		universe.update();
	}

	return 0;
}
