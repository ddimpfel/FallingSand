/*
* World of falling sand (modified version of WinterDev SandWorld)
* @author Dominick Dimpfel
* @date 02/02/2024
*/
#ifndef WORLD_H
#define WORLD_H
#include "ArrayGrid.h"
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include "SFML/Graphics.hpp"
#include "SFMath.h"
#include "Cell.h"
#include <cstdint>

using uint = uint64_t;
using shrt = int16_t;
using byte = int8_t;

class World
{
private:
	sf::RenderWindow& window;
	
	size_t m_width;
	size_t m_height;

	ArrayGrid<Cell> m_grid;
	ArrayGrid<Cell> m_buffer;

	std::vector<std::pair<uint, uint>> m_changes; // source, destination

	std::vector<sf::Uint8> m_pixels;

	std::array<Cell, Type::COUNT * 3> m_cellTypes;

	std::random_device rd;
	std::mt19937 random;
	std::uniform_int_distribution<int> uniform;

public:
	World(uint width, uint height, sf::RenderWindow& window) :
		window(window), m_width(width), m_height(height),
		m_grid(width, height), m_buffer(width, height) {

		m_pixels.resize(width * height * 4);
		random = std::mt19937(rd());

		initializeCellTypes();
	}
	~World() = default;

	void initializeCellTypes() {
		m_cellTypes[AIR] = {};
		
		m_cellTypes[SAND] = {
			Type::SAND,
			/*density*/ SAND,
			Phase::SOLID,
			sf::Color(235, 149, 52, 255)
		};

		m_cellTypes[SAND + COUNT] = {
			Type::SAND,
			/*density*/ SAND,
			Phase::SOLID,
			sf::Color(219, 139, 48, 255)
		};

		m_cellTypes[SAND + COUNT + COUNT] = {
			Type::SAND,
			/*density*/ SAND,
			Phase::SOLID,
			sf::Color(199, 126, 44, 255)
		};

		m_cellTypes[STONE] = {
			Type::STONE,
			/*density*/ STONE,
			Phase::IMMOVABLE,
			sf::Color(140, 140, 140, 255)
		};

		m_cellTypes[WATER] = {
			Type::WATER,
			/*density*/ WATER,
			Phase::LIQUID,
			sf::Color(31, 84, 173, 180)
		};

		m_cellTypes[WATER + COUNT] = {
			Type::WATER,
			/*density*/ WATER,
			Phase::LIQUID,
			sf::Color(41, 97, 195, 180)
		};
	}

	bool cellIsEmpty(uint x, uint y) {
		return m_grid.inBounds(x, y) && m_grid.getCell(x, y).type == AIR;
	}
	
	bool cellIsImmovable(uint x, uint y) {
		return m_grid.inBounds(x, y) && m_grid.getCell(x, y).phase == IMMOVABLE;
	}
	

	size_t width() const		{ return m_width; }
	void setWidth(int val)		{ m_width = val; }
	size_t height() const		{ return m_height; }
	void setHeight(int val)		{ m_height = val; }

	ArrayGrid<Cell>& grid()		{ return m_grid; }

	const Cell& getCellType(uint8_t type) const { return m_cellTypes[type]; }

	std::vector<sf::Uint8>& pixels()			{ return m_pixels; }


	// Faster but small bias to side scanned from (allieviated by random scan line chosen)
	void update();
	void updateCell(uint x, uint y);

	void handleLiquidVRay(Cell& cell, uint x, uint y);

	void moveGas();
	void moveLiquid(Cell& cell, uint x, uint y);
	void moveSolid(Cell& cell, uint x, uint y);

	bool canPassThrough(byte phase, uint x, uint y, byte viscocity);
	
	shrt canMoveDown(uint x, uint y, shrt vy, byte phase);
	shrt canMoveDiagDn(uint x, uint y, byte phase);
	shrt canMoveDiagDnWMo(uint x, uint y, shrt vx, byte density);
	shrt canMoveSide(uint x, uint y, byte phase);
	shrt canMoveSideWMo(uint x, uint y, shrt vx, byte phase);

	void swapBufferCells(uint x, uint y, uint xn, uint yn) {
		Cell& dstCell = m_buffer.cell(xn, yn);
		setBufferCell(x, y, dstCell);
		Cell& srcCell = m_grid.cell(x, y);
		setBufferCell(xn, yn, srcCell);
	}

	void setBufferCell(uint x, uint y, Cell& cell) {
		uint index = x + y * m_width;
		m_buffer.setCell(index, cell);

		// Set pixel array color values
		index *= 4;
		m_pixels[index]		= cell.color.r;
		m_pixels[index + 1] = cell.color.g;
		m_pixels[index + 2] = cell.color.b;
		m_pixels[index + 3] = cell.color.a;
	}

	void setCell(uint x, uint y, Cell& cell) {
		uint index = x + y * m_width;
		m_grid.setCell(index, cell);

		// Set pixel array color values
		index *= 4;
		m_pixels[index]		= cell.color.r;
		m_pixels[index + 1] = cell.color.g;
		m_pixels[index + 2] = cell.color.b;
		m_pixels[index + 3] = cell.color.a;
	}

	void setCell(uint i, Cell& cell) {
		m_grid.setCell(i, cell);

		// Set pixel array color values
		i *= 4;
		m_pixels[i] = cell.color.r;
		m_pixels[i + 1] = cell.color.g;
		m_pixels[i + 2] = cell.color.b;
		m_pixels[i + 3] = cell.color.a;
	}
};

#endif // !WORLD_H
