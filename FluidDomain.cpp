#include "FluidDomain.h"

using namespace std;

FluidDomain::FluidDomain(float diffusion, float viscosity, float dt){

	_pixMap.resize(NxN);
	for (int i = 0; i < NxN; ++i) {
		_pixMap[i].position = sf::Vector2f(i % N, (i / N) % N);
	}
	_rectMap.resize(NxN);
	for (int i = 0; i < NxN; ++i) {
		_rectMap[i].setSize(sf::Vector2f(SCALE, SCALE));
		_rectMap[i].setFillColor(sf::Color(rand()%255,rand()%255,rand()%255));
		_rectMap[i].setPosition(sf::Vector2f(i % N*SCALE, (i / N) % N*SCALE));
	}
	_size = N;
	_fluidSim = new FluidSim(N, ITER, diffusion, viscosity, dt);
}

void
FluidDomain::addDensity(int x, int y, float amount) {
	if ((x >= _size) || (y >= _size)
		|| (x < 0) || (y < 0)) {
		return;
		cerr << "[ERROR] addDensity(): coordinates (" << x << "," << y << ")\
			are out of range (0,0) -> (" << _size << "," << _size << ")\n";
		return;
	}
	_fluidSim->_density[XY(x, y)] += amount;
}

void
FluidDomain::addVelocity(int x, int y, float amountX, float amountY) {
	if ((x >= _size) || (y >= _size)
		|| (x < 0) || (y < 0)) {
		return;
		cerr << "[ERROR] addVelocity(): coordinates (" << x << "," << y << ")\
			are out of range (0,0) -> (" << _size << "," << _size << ")\n";
		return;
	}

	_fluidSim->_vx[XY(x, y)] += amountX;
	_fluidSim->_vy[XY(x, y)] += amountY;
}

void
FluidDomain::step() {
	_fluidSim->advance();
}

void
FluidDomain::renderAsPoint() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {

			float d = _fluidSim->_density[XY(i, j)];
			if (d > 255) d = 255.f;
			_pixMap[XY(i, j)].color = sf::Color(d,d, d);
		}
	}
}

void
FluidDomain::renderAsRect() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			float d = _fluidSim->_density[XY(i, j)];
			_maxd = max(d, _maxd);
			float h_d = min(d, 200.f);
			float h_d3 = min(d, 500.f);
			float h_d2 = min(d, 10000.f);

			//sf::Color rgb = hsv2rgb(100.f*h_d2/10000.f, 0.6f, h_d/200.f);
			//sf::Color rgb = hsv2rgb(290.f + 12 * (h_d2 / 10000.f), 0.6f, h_d3 / 500.f); pink :p
			sf::Color rgb = hsv2rgb(100 + 12 * (h_d2 / 10000.f), 0.6f, h_d3 / 500.f);
			_rectMap[XY(i, j)].setFillColor(rgb);
		}
	}
}


float
FluidDomain::getDensityAt(const sf::Vector2i& pos) const {
	//convert to i and j in fluid space
	int i = pos.x / SCALE;
	int j = pos.y / SCALE;

	i = max(i, 0);
	j = max(j, 0);

	i = min(i, _size - 1);
	j = min(j, _size - 1);

	return _fluidSim->_density[XY(i, j)];
}

float
FluidDomain::getVelocityAt(const sf::Vector2i& pos) const {
	//convert to i and j in fluid space
	int i = pos.x / SCALE;
	int j = pos.y / SCALE;

	i = max(i, 0);
	j = max(j, 0);

	i = min(i, _size - 1);
	j = min(j, _size - 1);

	float vx = _fluidSim->_vx[XY(i, j)];
	float vy = _fluidSim->_vy[XY(i, j)];
	float vel = sqrt(vx * vx + vx * vx);
	return vel;
}