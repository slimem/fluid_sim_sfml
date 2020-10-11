#include "FluidSim.h"
#include "FluidDomain.h"

using namespace std;

FluidSim::FluidSim(int size, int iters, float diffusion, float viscosity, float dt):
	_size(size), _iters(iters), _diff(diffusion), _visc(viscosity), _dt(dt) {

	//2d vectors
	_s.resize(_size * _size);
	_density.resize(_size * _size);
	_vx.resize(_size * _size);
	_vy.resize(_size * _size);
	_vx0.resize(_size * _size);
	_vy0.resize(_size * _size);
}

void
FluidSim::advance() {
	diffuse(1, _vx0, _vx, _visc, _dt);
	diffuse(2, _vy0, _vy, _visc, _dt);

	project(_vx0, _vy0, _vx, _vy);

	advect(1, _vx, _vx0, _vx0, _vy0, _dt);
	advect(2, _vy, _vy0, _vx0, _vy0, _dt);

	project(_vx, _vy, _vx0, _vy0);
	diffuse(0, _s, _density, _diff, _dt);
	advect(0, _density, _s, _vx, _vy, _dt);
}

void
FluidSim::diffuse(int b, vector<float>& v, vector<float>& v0, float diff, float dt) {
	float a = dt * diff * (_size - 2) * (_size - 2);
	linearSolve(b, v, v0, a, 1 + 4 * a);
}

void
FluidSim::linearSolve(int b, std::vector<float>& v, std::vector<float>& v0, float a, float c) {
	float cRecip = 1.0f / c;
	for (int k = 0; k < _iters; ++k) {
		for (int j = 1; j < _size - 1; ++j) {
			for (int i = 1; i < _size - 1; i++) {
				v[XY(i, j)] =
					(
						v0[XY(i, j)]
						+ a * (v[XY(i + 1, j)]
							+ v[XY(i - 1, j)]
							+ v[XY(i, j + 1)]
							+ v[XY(i, j - 1)]
							)) * cRecip;
			}
		}
		setBound(b, v);
	}
}

void
FluidSim::project(vector<float>& velX, vector<float>& velY, vector<float>& p, vector<float>& div) {
	for (int j = 1; j < _size - 1; j++) {
		for (int i = 1; i < _size - 1; i++) {
			div[XY(i, j)] = -0.5f * (
				velX[XY(i + 1, j)]
				- velX[XY(i - 1, j)]
				+ velY[XY(i, j + 1)]
				- velY[XY(i, j - 1)]
				) / _size;
			p[XY(i, j)] = 0;
		}
	}

	setBound(0, div);
	setBound(0, p);
	linearSolve(0, p, div, 1, 4);

	for (int j = 1; j < _size - 1; j++) {
		for (int i = 1; i < _size - 1; i++) {
			velX[XY(i, j)] -= 0.5f * (p[XY(i + 1, j)]
				- p[XY(i - 1, j)]) * _size;
			velY[XY(i, j)] -= 0.5f * (p[XY(i, j + 1)]
				- p[XY(i, j - 1)]) * _size;
		}
	}
	setBound(1, velX);
	setBound(2, velY);
}

void
FluidSim::advect(int b, std::vector<float>& d, vector<float>& d0, vector<float>& velX, vector<float>& velY, float dt) {
	float i0, i1, j0, j1;

	float dtx = dt * (_size - 2);
	float dty = dt * (_size - 2);

	float s0, s1, t0, t1;
	float tmp1, tmp2, x, y;

	float Nfloat = _size;
	float ifloat, jfloat;
	int i, j;

	for (j = 1, jfloat = 1; j < _size - 1; j++, jfloat++) {
		for (i = 1, ifloat = 1; i < _size - 1; i++, ifloat++) {
			tmp1 = dtx * velX[XY(i, j)];
			tmp2 = dty * velY[XY(i, j)];
			x = ifloat - tmp1;
			y = jfloat - tmp2;

			if (x < 0.5f) x = 0.5f;
			if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
			i0 = floor(x);
			i1 = i0 + 1.0f;
			if (y < 0.5f) y = 0.5f;
			if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
			j0 = floor(y);
			j1 = j0 + 1.0f;

			s1 = x - i0;
			s0 = 1.0f - s1;
			t1 = y - j0;
			t0 = 1.0f - t1;

			int i0i = int(i0);
			int i1i = int(i1);
			int j0i = int(j0);
			int j1i = int(j1);

			// DOUBLE CHECK THIS!!!
			d[XY(i, j)] =
				s0 * (t0 * d0[XY(i0i, j0i)] + t1 * d0[XY(i0i, j1i)]) +
				s1 * (t0 * d0[XY(i1i, j0i)] + t1 * d0[XY(i1i, j1i)]);
		}
	}

	setBound(b, d);
}

void
FluidSim::setBound(int b, vector<float>& v) {
	for (int i = 1; i < _size - 1; ++i) {
		v[XY(i, 0)] = b == 2 ? -v[XY(i, 1)] : v[XY(i, 1)];
		v[XY(i, _size - 1)] = b == 2 ? -v[XY(i, _size - 2)] : v[XY(i, _size - 2)];
	}
	for (int j = 1; j < _size - 1; ++j) {
		v[XY(0, j)] = b == 1 ? -v[XY(1, j)] : v[XY(1, j)];
		v[XY(_size - 1, j)] = b == 1 ? -v[XY(_size - 2, j)] : v[XY(_size - 2, j)];
	}

	//average corners to neighbors
	v[XY(0, 0)] = 0.5f * (v[XY(1, 0)] + v[XY(0, 1)]);
	v[XY(0, _size - 1)] = 0.5f * (v[XY(1, _size - 1)] + v[XY(0, _size - 2)]);
	v[XY(_size - 1, 0)] = 0.5f * (v[XY(_size - 2, 0)] + v[XY(_size - 1, 1)]);
	v[XY(_size - 1, _size - 1)] = 0.5f * (v[XY(_size - 2, _size - 1)] + v[XY(_size - 1, _size - 2)]);
}