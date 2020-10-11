#pragma once

#include "FluidSim.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#define N 128
#define NxN N*N
#define ITER 16
#define SCALE 8

class FluidDomain {

public:

	FluidDomain() = delete;
	FluidDomain(float diffusion, float viscosity, float dt);
	~FluidDomain() {
		delete _fluidSim;
	}

	static constexpr int XY(int x, int y) {
		return x + N * y;
	}

	//h [0,360], s v [0,1], output rgb [0,1]
	sf::Color hsv2rgb(float h, float s, float v) {
		h = std::min(h, 360.f);
		s = std::min(s, 1.f);
		v = std::min(v, 1.f);

		return sf::Color(
			fn_hsv2rgb(5, h, s, v) * 255,
			fn_hsv2rgb(3, h, s, v) * 255,
			fn_hsv2rgb(1, h, s, v) * 255);
	}

	//https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
	float fn_hsv2rgb(float n, float h, float s, float v) {
		float k = fmod(n + (h / 60), 6);
		return v - v * s * std::max(0.f,
			std::min(std::min(k, 4 - k), 1.0f));
	}

	void addDensity(int x, int y, float amount);
	void addVelocity(int x, int y, float amountX, float amountY);

	void step();
	void renderAsPoint();
	void renderAsRect();
	const sf::VertexArray& getVertexArray() const { return _pixMap; }
	const std::vector<sf::RectangleShape>& getRectMap() const { return _rectMap; }

	float getDensityAt(const sf::Vector2i& pos) const;
	float getVelocityAt(const sf::Vector2i& pos) const;

	float getMaxD() const { return _maxd; }
private:
	FluidSim*							_fluidSim;
	int									_size;
	float								_maxd = 0.f;

	sf::VertexArray						_pixMap;
	std::vector<sf::RectangleShape>		_rectMap;
};