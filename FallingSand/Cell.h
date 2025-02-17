/*
* Sand cells for falling sand
* @author Dominick Dimpfel
* @date 02/02/2024
*/
#ifndef CELL_H
#define CELL_H
#include "SFML/Graphics/Color.hpp"
#include <cstdint>

using uint = uint64_t;
using shrt = int16_t;
using byte = int8_t;

// Try to organize by density
// Spaced by phase types
enum Type {
	// GAS
	AIR		= 0,

	//LIQUID
	WATER,

	// SOLID
	SAND,

	// IMMOVABLE
	STONE,

	COUNT
};

enum Phase {
	GAS		= 0,
	LIQUID,
	SOLID,
	IMMOVABLE
};

struct Cell {
	Type type;
	byte density;
	Phase phase;
	sf::Color color;

	uint x = 0;
	uint y = 0;
	shrt vx = 0; // momentum
	shrt vy = 1; // Should always be trying to move down.

	Cell(Type _type = Type::AIR,
		byte _density = 0,
		Phase _state = Phase::GAS,
		sf::Color _color = sf::Color::Transparent) :
		type(_type), density(_density), phase(_state), color(_color) {}
	~Cell() = default;

	void setAir() {
		type = Type::AIR; 
		density = 0;
		phase = Phase::GAS;
		vx = 0;
		vy = 1;
		color = sf::Color::Transparent;
	}
};

#endif // !SAND_H