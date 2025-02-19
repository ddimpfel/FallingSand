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
// position is checked along the path, starting from the particle'e current position. If 
// a position along the path is taken by another particle, stop before there this frame, 
// impart force and gain reactant force. Checking every cell in contact with velocity ray.
//		View velocity handling diagram for ray check implementation
//

#include "Universe.h"
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


void Universe::update() {

	elementCount = 0;
	sandCount = 0;
	gravelCount = 0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
		Vec2i pos = sf::Mouse::getPosition(window);
		std::shared_ptr<Element> e = m_grid.getCell(pos.x, pos.y);
		std::cout << (int)e->phase << " (" << e->vx << ", " << e->vy << ")" << " mass = " << e->mass << std::endl;
	}


	// INPUTS

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Extra2)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 20; x++) {
			for (size_t y = pos.y; y < pos.y + 20; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				addElement(x, y, m_elements["air"]->clone());
			}
		}
	}



	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 100; x++) {
			for (size_t y = pos.y; y < pos.y + 10; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				addElement(x, y, m_elements["stone"]->clone());
			}
		}
	}
		
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 10; x++) {
			for (size_t y = pos.y; y < pos.y + 10; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				addElement(x, y, m_elements["sand"]->clone());
			}
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 10; x++) {
			for (size_t y = pos.y; y < pos.y + 10; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				addElement(x, y, m_elements["gravel"]->clone());
			}
		}
	}



	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Extra1)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 10; x++) {
			for (size_t y = pos.y; y < pos.y + 10; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				addElement(x, y, m_elements["water"]->clone());
			}
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 10; x++) {
			for (size_t y = pos.y; y < pos.y + 10; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				addElement(x, y, m_elements["slime"]->clone());
			}
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 10; x++) {
			for (size_t y = pos.y; y < pos.y + 10; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				addElement(x, y, m_elements["lava"]->clone());
			}
		}
	}

	// 
	// UPDATES
	// 
	// PRIMARY LOOP - Iterates over all cells
	//		(Starting from bottom -> Choose random scan line direction to avoid bias)
	// - Apply movement forces;
	// - Solve grid traversal, add "best" position to buffer -> add collisions; 
	//		Elements attempt to path to furthest desired cell. If blocked by denser cell, 
	//		consider it a collision and handle with impulse calculation. Add element 
	//		colliding with other to m_collisions for continuous solving (could add both?).
	// 
	// Update Grid with Buffer (TODO make loop to only update cells with altered data)
	//		Storing best positions and sets grid for use in continuous collision handling.
	// 
	// SECONDARY LOOP - Iterates over m_collisions (colliders only ATM)
	//		Solve grid traversal post collision and add updates to buffer. Do not solve
	//		collisions further. (Could set up step counter - keeps track of amount of cells
	//		an element traverses pre and post collision, to ensure it does not 'over-update')
	// 
	// Update Grid with Buffer (TODO make loop to only update cells with altered data)
	//		Final state of grid set.
	//
	
	for (int64_t y = m_height - 1; y >= 0; y--) {
		// TODO pre-generate random directions to avoid random generations each update ?
		if (uniform(random) % 2 == 0) 
			for (int64_t x = m_width - 1; x >= 0; x--)	updateElement(x, y);
		else 
			for (uint x = 0; x < m_width; x++)			updateElement(x, y);
	}

	m_grid = m_buffer;

	//m_collisionHandling = true;
	//while (!m_collisions.empty()) {
	//	const auto& [x, y] = m_collisions.back();
	//	traverseGrid(x, y, m_grid.cell(x, y));
	//	m_collisions.pop_back();
	//}
	//m_collisionHandling = false;
	
	//m_grid = m_buffer;
}

void Universe::updateElement(uint x, uint y) {
	std::shared_ptr<Element> e = m_grid.cell(x + y * m_width);

	switch (e->phase)
	{
	case Phase::EMPTY: {
		return;
	}
	case Phase::IMMOVABLE: {
		elementCount++;
		return;
	}
	case Phase::GAS: {
		elementCount++;
		moveGas(x, y, std::dynamic_pointer_cast<Gas>(e));
		break;
	}
	case Phase::LIQUID: {
		elementCount++;
		moveLiquid(x, y, std::dynamic_pointer_cast<Liquid>(e));
		break;
	}
	case Phase::SOLID: {
		elementCount++;
		moveSolid(x, y, std::dynamic_pointer_cast<Solid>(e));
		break;
	}
	default:
		break;
	}
	
	// TODO track total traversed velocity of elements to ensure they do not 'over-update' post collisions
	// e->vMag = sqrt(e->vx * e->vx + e->vy * e->vy); (make vMag an int)

	traverseGrid(x, y, e);

	return;
}

void Universe::moveGas(uint x, uint y, std::shared_ptr<Gas> g) {
	return;
}

void Universe::moveLiquid(uint x, uint y, std::shared_ptr<Liquid> l) {
	// Apply forces according to rules of solids in falling sand -> 
	// (1) move down		
	// (2) spread out horizontally (dispersion)
	// (3) stick to similar liquids (viscosity)

	l->vx *= abs(l->vx) > 0.01f ? l->airResistance * l->invMass : 0; // air resistance and friction
	if (canMoveTo(x, y + 1, l->density)) {
		l->active = true;
		l->vy += gravity * l->mass;
	}
	else if (diffuseLiquid(x, y, l)) {
		l->active = true;
		l->vy = 0;
		return;
	}
	else {
		l->active = false;
		l->vx = 0;
	}

	return;
}

bool Universe::diffuseLiquid(uint x, uint y, std::shared_ptr<Liquid> l) {
	float density = l->density;
	float vx = l->vx;

	bool canMoveLeft = vx <= 0 && canMoveTo(x - 1, y, density);
	bool canMoveRght = vx >= 0 && canMoveTo(x + 1, y, density);

	if (canMoveLeft && canMoveRght) {
		canMoveLeft = uniform(random) % 2 == 1;
		canMoveRght = !canMoveLeft;
	}

	if (canMoveLeft || canMoveRght) {
		float dir = canMoveLeft ? -1.f : 1.f;
		vx += abs(vx) >= l->diffusion ? 0 : dir;
		l->vx = vx;
		return true;
	}

	return false;
}

void Universe::moveSolid(uint x, uint y, std::shared_ptr<Solid> s) {
	// Apply forces according to rules of solids in falling sand -> 
	// (1) move down  
	// (2) spread out diagonally (inertia)

		 if	(s->density == 2.f)	sandCount++;
	else if (s->density == 3.f) gravelCount++;

	s->vy += gravity * s->mass;
	if (canMoveTo(x, y + 1, s->density)) {
		s->vx *= abs(s->vx) > 0.01f ? s->airResistance * s->invMass : 0;
		return;
	}

	s->vy *= s->vy > 0.01f ? s->friction * s->invMass : 0;
	float spread = spreadSolid(x, y, s->vx, s->density, s->inertia);
	if (spread) {
		s->vx += spread;
		s->vy += 1;//s->inertia;
		return;
	}

	s->vx = 0;
	s->vy = 0;
}

float Universe::spreadSolid(uint x, uint y, float vx, float density, byte inertia) {
	bool canMoveLeft = vx <= 0 && canMoveTo(x - 1, y, density);
	bool canMoveRght = vx >= 0 && canMoveTo(x + 1, y, density);

	if (canMoveLeft && canMoveRght) {
		canMoveLeft = uniform(random) % 2 == 1;
		canMoveRght = !canMoveLeft;
	}

	if (canMoveLeft) {
		for (byte i = 1; i <= inertia; i++) { // TUMBLE METHOD
			if (!canMoveToBuffer(x - 1, y + i, density)) return 0;
		}
		return -1.f;
	}
	if (canMoveRght) {
		for (byte i = 1; i <= inertia; i++) { // TUMBLE METHOD
			if (!canMoveToBuffer(x + 1, y + i, density)) return 0;
		}
		return 1.f;
	}
	return 0;
}

void Universe::traverseGrid(uint x, uint y, std::shared_ptr<Element> e) {
	// Rounding toward 0 to get rid of float bias
	auto vx = static_cast<shrt>(e->vx);
	auto vy = static_cast<shrt>(e->vy);

	if (abs(vx) < 0.5f && abs(vy) < 0.5f) {
		// buffer must be updated to avoid loss of data
		setBufferCell(x, y, e);
		return;
	}

	// limit max speed
	if (vx < -max_v)
		e->vx = -max_v;
	else if (vx > max_v)
		e->vx = max_v;
	if (vy < -max_v)
		e->vy = -max_v;
	else if (vy > max_v)
		e->vy = max_v;

	// Check rasterized line cells from closest to destination for valid move (Bresenham's line)
	// ************* Could potentially change to Dijkstra or A* pathfinding for variable terrain costs
	shrt dx = abs(vx);
	shrt dy = -abs(vy);

	byte sx = e->vx > 0 ? 1 : -1;
	byte sy = e->vy > 0 ? 1 : -1;

	shrt er = dx + dy;

	uint x0 = x;
	uint y0 = y;
	uint x1 = x + vx;
	uint y1 = y + vy;

	uint xPrev = x;
	uint yPrev = y;
	while (true) {
		if (x0 == x1 && y0 == y1) break;
		shrt er2 = 2 * er;
		if (er2 >= dy) {
			if (x0 == x1) break;
			er += dy;
			x0 += sx;
		}
		if (er2 <= dx) {
			if (y0 == y1) break;
			er += dx;
			y0 += sy;
		}

		if (m_collisionHandling == false && !canMoveTo(x0, y0, e->density)) {
			//std::shared_ptr<Element> other = !m_grid.inBounds(x0, y0) ? m_elements["border"] : m_buffer.cell(x0, y0);
			//bool xAxis = xPrev - x0 != 0;
			//collide(e, other, xAxis);

			m_collisions.emplace_back(xPrev, yPrev);

			swapCells(x, y, xPrev, yPrev);
			return;
		}
		else if (m_collisionHandling == true && !canMoveTo(x0, y0, e->density)) {
			swapCells(x, y, xPrev, yPrev);
			return;
		}

		xPrev = x0;
		yPrev = y0;
	}

	// Move full distance since not stopped
	swapCells(x, y, x + vx, y + vy);
}

void Universe::collide(std::shared_ptr<Element> e1, std::shared_ptr<Element> e2, bool isXAxis) const {
	float e = 0.9f; // Coefficient of restitution
	// Calculate velocities in the direction of collision
	float v1_primary = isXAxis ? e1->vx : e1->vy;
	float v2_primary = isXAxis ? e2->vx : e2->vy;
	// Calculate velocities perpendicular to the direction of collision
	float v1_secondary = isXAxis ? e1->vy : e1->vx;
	float v2_secondary = isXAxis ? e2->vy : e2->vx;

	float e1invMass = e1->invMass;
	float e2invMass = e2->invMass;

	// Primary direction impulse calculation
	float J_primary = -(1 + e) * (v2_primary - v1_primary) / (e1invMass + e2invMass);
	float v1_prime_primary = v1_primary - J_primary * e1invMass;
	float v2_prime_primary = v2_primary + J_primary * e2invMass;

	// Assuming secondary direction is affected less, apply a smaller restitution effect
	float J_secondary = -(1 + e) * (v2_secondary - v1_secondary) / (e1invMass + e2invMass) * 0.5f; // Example: Half the impulse
	float v1_prime_secondary = v1_secondary - J_secondary * e1invMass;
	float v2_prime_secondary = v2_secondary + J_secondary * e2invMass;

	// Apply the updated velocities based on the direction
	if (isXAxis) {
		e1->vx = v1_prime_primary;
		e2->vx = v2_prime_primary;
		e1->vy = v1_prime_secondary;
		e2->vy = v2_prime_secondary;
	}
	else {
		e1->vy = v1_prime_primary;
		e2->vy = v2_prime_primary;
		e1->vx = v1_prime_secondary;
		e2->vx = v2_prime_secondary;
	}
}
