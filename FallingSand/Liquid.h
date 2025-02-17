/*
* Liquids in physics-based falling sand simulation
* @author Dominick Dimpfel
* @date 02/24/2024
*/
#ifndef LIQUID_H
#define LIQUID_H
#include "Element.h"

struct Liquid : public Element {
	byte diffusion;
	float viscosity;

	Liquid(
		sf::Color _color,
		float _mass,
		float _density,
		byte _dispersion,
		float _viscosity,
		bool _movable = true
	) : 
		Element(_movable ? Phase::LIQUID : Phase::IMMOVABLE, 
			_color, 
			_mass, 
			_density),
		diffusion(_dispersion),
		viscosity(_viscosity) {
		airResistance /= viscosity;
	}

	std::shared_ptr<Element> clone() const override {
		return std::make_shared<Liquid>(*this);
	}

	~Liquid() override = default;
};

#endif // !LIQUID_H
