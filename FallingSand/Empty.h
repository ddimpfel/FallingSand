/*
* Empty cell in physics-based falling sand simulation
* @author Dominick Dimpfel
* @date 02/24/2024
*/
#ifndef EMPTY_H
#define EMPTY_H
#include "Element.h"

struct Empty : public Element {
	Empty() : Element(Phase::EMPTY, sf::Color::Transparent, 0.f, 0.f) {}
	~Empty() override = default;
};

#endif // !EMPTY_H
