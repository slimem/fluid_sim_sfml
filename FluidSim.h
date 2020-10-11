#pragma once

#include <vector>

class FluidDomain;

class FluidSim {
	friend FluidDomain;
public:
	FluidSim(int size, int iters, float diffusion, float viscosity, float dt);
	~FluidSim() {};

	//using a one dim but with 2d access
	int XY(int x, int y) {
		return x + _size * y;
	}

	void setBound(int b, std::vector<float>& v);
	void linearSolve(
		int b,
		std::vector<float>& v,
		std::vector<float>& v0,
		float a,
		float c
	);
	void diffuse(
		int b,
		std::vector<float>& v,
		std::vector<float>& v0,
		float diff, float dt
	);
	void project(
		std::vector<float>& velX,
		std::vector<float>& velY,
		std::vector<float>& p,
		std::vector<float>& div
	);
	void advect(
			int b,
			std::vector<float>& d,
			std::vector<float>& d0,
			std::vector<float>& velX,
			std::vector<float>& velY,
			float dt
	);

	void addDensity(int x, int y, float amount) {
		_density[XY(x, y)] += amount;
	}

	void addVelocity(int x, int y, float amountX, float amountY) {
		_vx[XY(x, y)] += amountX;
		_vy[XY(x, y)] += amountY;
	}

	const std::vector<float>& getDensity() const { return _density; }

	//Advance by 1 frame step
	void advance();

private:
	int									_size;
	int									_iters;
	float								_dt;   //timestep
	float								_diff; //diffusion amount
	float								_visc; //viscosity

	std::vector<float>					_s;			//previous density
	std::vector<float>					_density;	//density
	std::vector<float>					_vx;
	std::vector<float>					_vy;
	std::vector<float>					_vx0;
	std::vector<float>					_vy0;
};