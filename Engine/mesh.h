//
//  mesh.h
//  adapted from Spring
//
//  Created by Matthew Dillard on 11/3/15.
//

#ifndef mesh_h
#define mesh_h

#include <iostream>

#include "v3.h"

namespace mesh {

	struct vertex {
		v3 pos, vel;
		float red, green, blue;
		float mass;
		bool fixed;

		vertex(const v3 p = v3(), const v3 v = v3(), const float r = 0.0f, const float g = 0.5f, const float b = 0.4f, const float m = 0.0, const bool f = false):
			pos(p), vel(v), red(r), green(g), blue(b), mass(m), fixed(f) {}
		vertex(const vertex& v): pos(v.pos), vel(v.vel), red(v.red), green(v.green), blue(v.blue), mass(v.mass), fixed(v.fixed) {}

		vertex operator+(const vertex& v) const { return {pos + v.pos, vel + v.vel, red, green, blue, mass, fixed}; }
		vertex operator-(const vertex& v) const { return {pos - v.pos, vel - v.vel, red, green, blue, mass, fixed}; }
		vertex operator*(const float value) const { return {pos * value, vel * value, red, green, blue, mass, fixed}; }
		vertex& operator=(const vertex& v) { pos = v.pos; vel = v.vel; mass = v.mass; red = v.red; green = v.green; blue = v.blue; fixed = v.fixed; return *this; }

		operator bool() const { return fixed; }
	};

	struct strut {
		float k, d, l;
		unsigned long i1, i2;
	};

	struct face {
	private:
		v3 pick_axis_proj(const v3& normal) const {
			if (normal.x > normal.y && normal.x > normal.z)
				return {0, 1, 1};
			else if (normal.y > normal.x && normal.y > normal.z)
				return {1, 0, 1};
			return {1, 1, 0};
		}

	public:
		unsigned long i0, i1, i2, i3;

		bool cross_plane(const v3& c, const v3& normal, const v3& x0, const v3& x1) const {
			float sign1 = (x0 - c).dot(normal);
			float sign2 = (x1 - c).dot(normal);
			if (sign1 * sign2 < 0.0)
				return true;
			return false;
		}

		v3 find_intersect(const v3& c, const v3& normal, const v3& x1, const v3& x2) const {
			v3 l = (x2 - x1).normalize();
			float d = ((x1 - c).dot(normal)) / (l.dot(normal));
			return x1 + l * d;
		}

		bool projection_intersection(const v3& normal, const v3& c1, const v3& c2, const v3& c3, const v3& c4, const v3& x) const {
			v3 proj(pick_axis_proj(normal)), p_i(v3{1, 1, 1} -proj);
			v3 V1((c2 - c1) * proj), x1((x - c1) * proj), cp1((V1.cross(x1) * p_i).normalize());
			v3 V2((c3 - c2) * proj), x2((x - c2) * proj), cp2((V2.cross(x2) * p_i).normalize());
			v3 V3((c4 - c3) * proj), x3((x - c3) * proj), cp3((V3.cross(x3) * p_i).normalize());
			v3 V4((c1 - c4) * proj), x4((x - c3) * proj), cp4((V4.cross(x4) * p_i).normalize());

			if (cp1 == cp2 && cp2 == cp3 && cp3 == cp4)
				return true;
			return false;
		}

		float dist_from_plane(const v3& c, const v3& normal, const v3& x) const {
			return (x - c).dot(normal);
		}
	};
}

#endif /* mesh_h */
