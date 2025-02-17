/*
* World of falling sand (modified version of WinterDev SandWorld)
* @author Dominick Dimpfel
* @date 02/02/2024
*/

#include "World.h"
#include "ArrayGrid.h"
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include "SFML/Graphics.hpp"
#include "SFMath.h"
#include "Cell.h"
#include <cstdint>

void World::update() {

	// INPUTS

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 8; x++) {
			for (size_t y = pos.y; y < pos.y + 8; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				setBufferCell(x, y, m_cellTypes[SAND + COUNT * (uniform(random) % 3)]);
			}
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 10; x++) {
			for (size_t y = pos.y; y < pos.y + 10; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				setBufferCell(x, y, m_cellTypes[STONE + COUNT * (uniform(random) % 1)]);
			}
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::XButton1)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 8; x++) {
			for (size_t y = pos.y; y < pos.y + 8; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				setBufferCell(x, y, m_cellTypes[WATER + COUNT * (uniform(random) % 2)]);
			}
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::XButton2)) {
		Vec2i pos = sf::Mouse::getPosition(window);

		for (size_t x = pos.x; x < pos.x + 20; x++) {
			for (size_t y = pos.y; y < pos.y + 20; y++) {
				if (!m_grid.inBounds(x, y)) continue;
				setBufferCell(x, y, m_cellTypes[AIR]);
			}
		}
	}

	// UPDATES

	for (int64_t y = m_height - 1; y >= 0; y--) { // Start from bottom
		// Choose random scan line to avoid bias
		if (uniform(random) % 2 == 0) {
			for (int64_t x = m_width - 1; x >= 0; x--) updateCell(x, y);
			//std::cout << std::endl;
		}
		else {
			for (uint x = 0; x < m_width; x++) updateCell(x, y);
			//std::cout << std::endl;
		}
	}

	m_grid = m_buffer;
}
//
// TODO On the right track, need to add momentum transfers, viscocity, pressure, cell velocity limits, 
//		liquids should try to stick to eachother and find the point of lowest pressure to travel to.
//		Change cell checking to use velocities as a ray. Similar to how universe handles velocities.
//
void World::updateCell(uint x, uint y) {
	uint index = x + y * m_width;
	Cell& cell = m_grid.cell(index);

	switch (cell.phase)
	{
	case GAS:
	{
		// TODO create gases
		break;
	}
	case LIQUID: // OVERENGINEERED ? ?
	{
		//if (uniform(random) % 100 > 90) break;
		moveLiquid(cell, x, y);
		break;
	}
	case SOLID:
	{
		moveSolid(cell, x, y);
		break;
	}
	default: // IMMOVABLE
		break;
	}

}

void World::moveLiquid(Cell& cell, uint x, uint y) {
	byte density = cell.density;
	shrt& vx = cell.vx;
	shrt& vy = cell.vy;

	shrt moveDown = canMoveDown(x, y, vy, density);

	// Did not use all of downward momemntum, convert to side movement
	if (moveDown && moveDown != vy) { 
		if (vx == 0)	// Find direction to begin lateral movement
			vx = uniform(random) % 2 == 1 ? vy - moveDown : moveDown - vy;
		else			// Continue in direction of lateral movement
			vx += vx > 0 ? (vy - moveDown) : (moveDown - vy);

		if (vx > 5)  vx = 5;	// TODO SET TO LIMITER
		if (vx < -5) vx = -5;	// TODO SET TO LIMITER
	}

	if (moveDown) {
		if (vx == 0) {
			vy = vy < 4 ? moveDown + 1 : 4;
			swapBufferCells(x, y, x, y + moveDown);
			return;
		}

		shrt moveSide = canMoveSideWMo(x, y + moveDown, vx, density);
		if (moveSide) {
			vx = vx > 0 ? moveSide - 1 : moveSide + 1;
			vy = vy < 4 ? moveDown + 1 : 4;
			swapBufferCells(x, y, x + moveSide, y + moveDown);
			return;
		}

		// Can't move laterally
		vx = 0;
		vy = vy < 4 ? moveDown + 1 : 4;
		swapBufferCells(x, y, x, y + moveDown);
		return;
	}
	vy = 1; // Safety reset

	// Can't move down
	// Find direction to begin lateral movement
	if (vx == 0) {
		shrt moveSide = canMoveSide(x, y, density);
		if (moveSide) {
			vx = moveSide;
			swapBufferCells(x, y, x + moveSide, y);
		}
		return;
	}
	// Continue in direction of lateral movement
	shrt moveSide = canMoveSideWMo(x, y, vx, density);
	if (moveSide) {
		vx = moveSide > 0 ? moveSide + 1 : moveSide - 1;
		swapBufferCells(x, y, x + moveSide, y);
		return;
	}

	// Can't move down or laterally
	// Try to move diagonally down (fill gaps)
	shrt moveDiagDn = canMoveDiagDn(x, y, density);
	if (moveDiagDn) {
		vx = moveDiagDn;
		swapBufferCells(x, y, x + moveDiagDn, y + 1);
		return;
	}

	// Nothing occurred, must update buffer or information is lost to old grid
	m_buffer.cell(x, y).vx = 0;
	m_buffer.cell(x, y).vy = 1;
}

void World::moveSolid(Cell& cell, uint x, uint y) {
	byte density = cell.density;
	shrt& vx = cell.vx;
	shrt& vy = cell.vy;

	shrt moveDown = canMoveDown(x, y, vy, density);
	if (moveDown) {
		if (vx != 0) {
			// Has lateral velocity
			shrt moveDiag = canMoveDiagDnWMo(x, y + moveDown, vx, density);
			if (moveDiag /*&& (canPassThrough(GAS, x + moveDiag, y + moveDown, 75) || canPassThrough(LIQUID, x + moveDiag, y + moveDown, 30))*/) {
				vy = moveDown + 1;
				vx /= 2;
				swapBufferCells(x, y, x + moveDiag, y + moveDown);
				return;
			}
		}
		// Has no lateral velocity
		//if (canPassThrough(GAS, x, y + moveDown, 75) || canPassThrough(LIQUID, x, y + moveDown, 30)) {
			vy = moveDown + 1;
			swapBufferCells(x, y, x, y + moveDown);
		//}
		return;
	}
	vy = 1; // Safety reset

	// Can't move down
	if (vx == 0) { // Find direction to begin lateral movement
		shrt moveDiag = canMoveDiagDn(x, y, density);
		if (moveDiag /*&& (canPassThrough(GAS, x + moveDiag, y + 1, 75) || canPassThrough(LIQUID, x + moveDiag, y + 1, 30))*/) {
			vx = moveDiag;
			swapBufferCells(x, y, x + moveDiag, y + 1);
		}
		return;
	}
	// Continue in direction of lateral movement
	shrt moveDiag = canMoveDiagDnWMo(x, y, vx, density);
	if (moveDiag) {
		vx = moveDiag > 0 ? moveDiag + 1 : moveDiag - 1;
		swapBufferCells(x, y, x + moveDiag, y);
		return;
	}

	// Can't move diag or down, use rest of lateral momentum
	shrt moveSide = canMoveSideWMo(x, y, vx, density);
	if (moveSide) {
		vx = moveSide > 0 ? moveSide - 1 : moveSide + 1;
		swapBufferCells(x, y, x + moveSide, y);
		return;
	}

	// Nothing occurred, must update buffer or information is lost to old grid
	m_buffer.cell(x, y).vx = 0;
	return;
}



// Acts as density : percent chance to move through unhindered
bool World::canPassThrough(byte density, uint x, uint y, byte percentChance) {
	return m_grid.getCell(x, y).density == density && uniform(random) % 101 < percentChance;
}

// TODO SHOULD THESE ALL LOOK AT GRID?
// Go from 1 to vy to avoid tunneling
shrt World::canMoveDown(uint x, uint y, shrt vy, byte density) {
	for (shrt i = 1; i <= vy; i++) {
		if (m_buffer.inBounds(x, y + i) && m_buffer.getCell(x, y + i).density < density)
			continue;

		// Can no longer move, 
		return i - 1;
	}
	// TODO MAKE CELLS HAVE DIFFRENT CAPS ?
	if (vy > 4)
		return 4;
	return vy; // Less than cap
}

shrt World::canMoveDiagDn(uint x, uint y, byte density) {
	bool downLeftEmpty = m_buffer.inBounds(x - 1, y + 1) && m_buffer.getCell(x - 1, y + 1).density < density;
	bool downRightEmpty = m_buffer.inBounds(x + 1, y + 1) && m_buffer.getCell(x + 1, y + 1).density < density;

	if (downLeftEmpty && downRightEmpty) {
		downLeftEmpty = uniform(random) % 2 == 1;
		downRightEmpty = !downLeftEmpty;
	}

	if (downLeftEmpty) {
		return -1;
	}
	if (downRightEmpty) {
		return 1;
	}

	return 0;
}

shrt World::canMoveDiagDnWMo(uint x, uint y, shrt vx, byte density)
{
	if (vx < 0) {
		for (shrt dx = 1; dx <= -vx; dx++) {
			if (m_buffer.inBounds(x - dx, y + dx) && m_buffer.getCell(x - dx, y + dx).density < density)
				continue;
			return dx - 1;
		}
	}
	else { // vx > 0
		for (shrt dx = 1; dx <= vx; dx++) {
			if (m_buffer.inBounds(x + dx, y + dx) && m_buffer.getCell(x + dx, y + dx).density < density)
				continue;
			return dx - 1;
		}
	}

	return vx;
}

// Using m_grid to avoid scan line bias. Since direction of scan line is unknown
// Initializes lateral movement in random direction, based on open cells.
shrt World::canMoveSide(uint x, uint y, byte density) {
	bool canMoveLeft = m_grid.inBounds(x - 1, y) && m_grid.getCell(x - 1, y).density < density;
	bool canMoveRght = m_grid.inBounds(x + 1, y) && m_grid.getCell(x + 1, y).density < density;

	if (canMoveLeft && canMoveRght) {
		canMoveLeft = uniform(random) % 2 == 1;
		canMoveRght = !canMoveLeft;
	}

	if (canMoveLeft) {
		return -1;
	}
	if (canMoveRght) {
		return 1;
	}

	return 0;
}

// Separated from other moveSide since direction matters.
// Returns the x coordinate of the most distant cell that can be occupied, 
//		checking from (-)1 to vx, to avoid tunnelling.
shrt World::canMoveSideWMo(uint x, uint y, shrt vx, byte density) {
	if (vx < 0) {
		for (shrt dx = -1; dx >= vx; dx--) {
			if (m_grid.inBounds(x + dx, y) && m_grid.getCell(x + dx, y).density < density)
				continue;
			return dx + 1;
		}
	}
	else { // vx > 0
		for (shrt dx = 1; dx <= vx; dx++) {
			if (m_grid.inBounds(x + dx, y) && m_grid.getCell(x + dx, y).density < density)
				continue;
			return dx - 1;
		}
	}

	if (vx == 0) return 0;

	// TODO MAKE TYPES HAVE DIFFRENT CAPS ? */
	if (vx > 5) return 5;
	if (vx < -5) return -5;

	return vx;
}