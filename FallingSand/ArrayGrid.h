/*
* Grid made of Uint vector array, where each in represents
* the state or material of a cell.
* @author Dominick Dimpfel
* @date 02/02/2024
*/
#ifndef ARRAYGRID_H
#define ARRAYGRID_H

#include <vector>
#include <cstdint>

using uint = uint64_t;

template <typename T>
class ArrayGrid {
private:
	std::vector<T> m_cells;
	uint m_width;
	uint m_height;
	uint m_size;

public:
	ArrayGrid(uint width = 0, uint height = 0) :
		m_width(width), m_height(height), m_size(height*width) {
		m_cells.resize(width * height);
	}
	~ArrayGrid() = default;

	std::vector<T>& cells() { return m_cells; }
	uint size() const		{ return m_size; }

	uint width() const		{ return m_width; }
	uint height() const		{ return m_height; }

	// Index i should be x + y * width
	T& cell(uint i)							{ return m_cells[i]; }
	const T& getCell(uint i)				{ return m_cells[i]; }
	void setCell(uint i, T& data)			{ m_cells[i] = data; }

	T& cell(uint x, uint y)					{ return m_cells[x + y * m_width]; }
	const T& getCell(uint x, uint y)		{ return m_cells[x + y * m_width]; }
	void setCell(uint x, uint y, T& data)	{ m_cells[x + y * m_width] = data; }

	void swap(ArrayGrid& other) {
		std::swap(m_cells, other.cells()); // need to ensure this is exception free
	}

	bool inBounds(uint x, uint y) const {
		return x >= 0 && x < m_width
			&& y >= 0 && y < m_height;
	}

	bool inBounds(uint index) const {
		return index < m_size;
	}
};

#endif // !VECTORGRID_H
