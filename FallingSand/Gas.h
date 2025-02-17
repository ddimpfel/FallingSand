/*
* Gases in physics-based falling sand simulation
* @author Dominick Dimpfel
* @date 02/24/2024
*/
#ifndef GAS_H
#define GAS_H
#include "Element.h"

struct Gas : public Element {
	Gas(
		sf::Color _color,
		float _mass,
		float _density
	) : 
		Element(Phase::GAS, 
			_color, 
			_mass, 
			_density)
	{}

	std::shared_ptr<Element> clone() const override {
		return std::make_shared<Gas>(*this);
	}

	~Gas() override = default;
};

#endif // !GAS_H
