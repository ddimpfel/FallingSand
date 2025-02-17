/*
* Solids in physics-based falling sand simulation
* @author Dominick Dimpfel
* @date 02/24/2024
*/
#ifndef SOLID_H
#define SOLID_H
#include "Element.h"

struct Solid : public Element {
	byte inertia;
	float invInertia;

	Solid(
		sf::Color _color,
		float _mass,
		float _density,
		byte _inertia,
		bool _movable = true
	) : 
		Element(_movable ? Phase::SOLID : Phase::IMMOVABLE, 
			_color, 
			_mass, 
			_density),
		inertia(_inertia) {
		invInertia = inertia == 0 ? 0 : 1.f / inertia;
	}

	std::shared_ptr<Element> clone() const override {
		return std::make_shared<Solid>(*this);
	}

	~Solid() override = default;
};

#endif // !SOLID_H
