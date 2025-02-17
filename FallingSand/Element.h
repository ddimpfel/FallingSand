/*
* Element base class for physics-based cells in falling sand simulation
* @author Dominick Dimpfel
* @date 02/18/2024
*/
#ifndef ELEMENT_H
#define ELEMENT_H
#include "SFML/Graphics/Color.hpp"
#include <cstdint>

using uint = uint64_t;
using shrt = int16_t;
using byte = int8_t;

constexpr float max_mass = 100.f;
constexpr float max_v = 5.f;

enum class Phase {
	GAS,
	LIQUID,
	SOLID,
	IMMOVABLE,
	EMPTY
};

struct Element {
	Phase phase;
	sf::Color color;

	float mass;		// negative mass is lighter than air ?
	float invMass; 

	float density;

	float vx = 0;	
	float vy = 0;

	float airResistance = 0.9f;
	float friction = 0.9f;

	bool active = false;

	Element(
		Phase _phase,
		sf::Color _color,
		float _mass,
		float _density
	) :
		phase(_phase), 
		color(_color),  
		mass(_mass), 
		density(_density) {
		mass = _mass > max_mass ? max_mass : _mass;
		invMass = mass < 1 ? 1.f : 1 / mass; 
	}

	virtual std::shared_ptr<Element> clone() const {
		return std::make_shared<Element>(*this);
	}

	virtual ~Element() = default;

	virtual void setAirResistance(float ar) { airResistance = ar; }
	virtual void setFriction(float fr)		{ friction = fr; }

	virtual void applyForce(float fx, float fy) {
		vx += fx * mass;
		vy += fy * mass;
	}
};

#endif // !ELEMENT_H