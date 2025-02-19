/*
* Universe of falling sand. This is a restart to Falling Sand World.
* This class handles the grid the cells live in.
* Adding more physics based cells.
* @author Dominick Dimpfel
* @date 02/18/2024
*/

// 
//  IDEA - fluid sim-esque, with each pixel being a particle
// ------
//  Early out if cell is empty (air)
// 
//	All particles feel gravity, negative gravity for gases lighter than air.
//		By mass being negative (?).
// 
//	All particles feel a force to spreadSolid out, dampened by particles inertia. From liquids
// filling all space to solids forming pyramid-like structures. Make gases fill all space
// available(?). 1 is perfectly fluid, no inertia, 0 is immovable, max inertia.
// 
//	Particles will attempt to move to a position based on their current velocity. Each 
// position is checked along the path, starting from the particle's current position. If 
// a position along the path is taken by another particle, stop before there this frame, 
// impart force and gain reactant force. Checking every cell in contact with velocity ray.
//		View velocity handling diagram for ray check implementation
//
#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <cmath>
#include <vector>
#include <unordered_map>
#include <random>
#include <cstdint>

#include "ArrayGrid.h"
#include <SFML/Graphics.hpp>
#include "SFMath.h"

#include "Element.h"
#include "Empty.h"
#include "Gas.h"
#include "Liquid.h"
#include "Solid.h"

using uint = uint64_t;
using shrt = int16_t;
using byte = int8_t;

constexpr float gravity = 1.f;

class Universe
{
private:
	sf::RenderWindow& window;

	size_t m_width;
	size_t m_height;

	ArrayGrid<std::shared_ptr<Element>> m_grid;
	ArrayGrid<std::shared_ptr<Element>> m_buffer;

	std::vector<std::pair<uint, uint>> m_collisions;
	bool m_collisionHandling = false;

	std::vector<std::uint8_t> m_pixels;

	std::unordered_map<std::string, std::shared_ptr<Element>> m_elements;

	std::random_device rd;
	std::mt19937 random;
	std::uniform_int_distribution<int> uniform;

public:
	int elementCount = 0;
	int sandCount = 0;
	int gravelCount = 0;

	Universe(uint width, uint height, sf::RenderWindow& window) :
		window(window), m_width(width), m_height(height),
		m_grid(width, height), m_buffer(width, height) {
		m_pixels.resize(width * height * 4);
		random = std::mt19937(rd());

		initElements();
		initGrids();
	}
	~Universe() = default;

	void initElements() {
		m_elements["border"] = std::make_shared<Empty>();
		m_elements["air"] = std::make_shared<Empty>();

		// **************  SOLIDS  **************

		m_elements["sand"] = std::make_shared<Solid>(
			sf::Color(235, 149, 52, 255),
			1.f,	// Mass
			2.f,	// Density
			1		// inertia
		);

		m_elements["stone"] = std::make_shared<Solid>(
			sf::Color(150, 150, 150, 255),
			2.f,	// Mass
			100.f,	// Density
			0,		// inertia
			false	// Not movable
		);

		m_elements["gravel"] = std::make_shared<Solid>(
			sf::Color(200, 200, 200, 255),
			1.f,	// Mass
			3.f,	// Density
			3		// inertia
		);

		// **************  LIQUIDS  **************

		m_elements["water"] = std::make_shared<Liquid>(
			sf::Color(82, 134, 255, 255),
			1.f,	// Mass
			1.f,	// Density
			5,		// Dispersion
			1.f		// Viscosity
		);

		m_elements["slime"] = std::make_shared<Liquid>(
			sf::Color(136, 255, 110, 255),
			1.f,	// Mass
			0.9f,	// Density
			5,		// Dispersion
			1.f	// Viscosity
		);

		m_elements["lava"] = std::make_shared<Liquid>(
			sf::Color(255, 79, 48, 255),
			3.f,	// Mass
			1.1f,	// Density
			2,		// Dispersion
			1.2f		// Viscosity
		);

		// **************   GASES   **************
	}

	void initGrids() {
		const auto& air = m_elements["air"];
		for (uint x = 0; x < m_width; x++) {
			for (uint y = 0; y < m_height; y++) {
				addElement(x, y, air);
			}
		}
	}

	ArrayGrid<std::shared_ptr<Element>>&  grid()  { return m_grid; }
	ArrayGrid<std::shared_ptr<Element>>& buffer() { return m_buffer; }

	size_t width() const		{ return m_width; }
	void setWidth(int val)		{ m_width = val; }
	size_t height() const		{ return m_height; }
	void setHeight(int val)		{ m_height = val; }

	std::shared_ptr<Element> getParticleType(const std::string& type) { return m_elements[type]; }

	std::vector<std::uint8_t>& pixels() { return m_pixels; }

	void update();
	void updateElement(uint x, uint y);

	bool canMoveTo(uint x, uint y, float density) {
		return m_grid.inBounds(x, y) && (m_buffer.getCell(x, y)->density < density || m_grid.getCell(x, y)->density < density);
	}
	bool canMoveToBuffer(uint x, uint y, float density) {
		return m_grid.inBounds(x, y) && m_buffer.getCell(x, y)->density < density;
	}

	void moveGas   (uint x, uint y, std::shared_ptr<Gas> g);
	void moveLiquid(uint x, uint y, std::shared_ptr<Liquid> l);
	void moveSolid (uint x, uint y, std::shared_ptr<Solid> s);

	bool diffuseLiquid(uint x, uint y, std::shared_ptr<Liquid> l);
	void applyViscosityToNeighbors(uint x, uint y, std::shared_ptr<Liquid> l);

	float spreadSolid(uint x, uint y, float vx, float density, byte inertia);

	void collide(std::shared_ptr<Element> e1, std::shared_ptr<Element> e2, bool isXAxis) const;

	void traverseGrid(uint x, uint y, std::shared_ptr<Element> e);

	// TODO Duplication bug lies in here
	void swapCells(uint x, uint y, uint xn, uint yn) {
		const std::shared_ptr<Element>& dstCell = m_buffer.cell(xn, yn);
		setBufferCell(x, y, dstCell);
		const std::shared_ptr<Element>& srcCell = m_grid.cell(x, y);
		setBufferCell(xn, yn, srcCell);
	}

	void swapGridCells(uint x, uint y, uint xn, uint yn) {
		const std::shared_ptr<Element>& dstCell = m_grid.cell(xn, yn);
		setGridCell(x, y, dstCell);
		const std::shared_ptr<Element>& srcCell = m_buffer.cell(x, y);
		setGridCell(xn, yn, srcCell);
	}

	void setGridCell(uint x, uint y, std::shared_ptr<Element> e) {
		uint index = x + y * m_width;
		m_grid.setCell(index, e);

		// Set pixel array color values
		index *= 4;
		m_pixels[index]		= e->color.r;
		m_pixels[index + 1] = e->color.g;
		m_pixels[index + 2] = e->color.b;
		m_pixels[index + 3] = e->color.a;
	}

	void setBufferCell(uint x, uint y, std::shared_ptr<Element> e) {
		uint index = x + y * m_width;
		m_buffer.setCell(index, e);

		// Set pixel array color values
		index *= 4;
		m_pixels[index]		= e->color.r;
		m_pixels[index + 1] = e->color.g;
		m_pixels[index + 2] = e->color.b;
		m_pixels[index + 3] = e->color.a;
	}

	void addElement(uint x, uint y, std::shared_ptr<Element> templatePE) {
		setGridCell(x, y, templatePE);
		setBufferCell(x, y, templatePE);
	}
};

#endif // !UNIVERSE_H
