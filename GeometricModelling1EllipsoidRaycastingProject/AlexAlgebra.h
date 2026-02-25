#ifndef ALEX_ALGEBRA_H
#define ALEX_ALGEBRA_H

namespace aa {
	
	// Basic structures
	struct vec2 {
		float x, y;
		
		// Constructors
		vec2() : x(0), y(0) {};
		vec2(float scalar) : x(scalar), y(scalar) {};
		vec2(float _x, float _y) : x(_x), y(_y) {};

		// Operators
		vec2 operator+(const vec2& v) const
		{
			return vec2(x + v.x, y + v.y);
		}
		vec2 operator-(const vec2& v) const
		{
			return vec2(x - v.x, y - v.y);
		}
		vec2 operator*(const float s) const
		{
			return vec2(x * s, y * s);
		}
		vec2 operator/(const float s) const
		{
			return vec2(x / s, y / s);
		}
		vec2& operator+=(const vec2& v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}
	};

	struct vec3 {
		union {
			struct { float x, y, z; };
			struct { float r, g, b; };
		};

		// Constructors
		vec3() : x(0), y(0), z(0) {};
		vec3(float scalar) : x(scalar), y(scalar), z(scalar) {};
		vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
		vec3(vec2 v, float _z) : x(v.x), y(v.y), z(_z) {};
		vec3(float _x, vec2 v) : x(_x), y(v.x), z(v.y) {};

		// Operators
		vec3 operator+(const vec3& v) const
		{
			return vec3(x + v.x, y + v.y, z + v.z);
		}
		vec3 operator-(const vec3& v) const
		{
			return vec3(x - v.x, y - v.y, z - v.z);
		}
		vec3 operator*(const float s) const
		{
			return vec3(x * s, y * s, z * s);
		}
		vec3 operator/(const float s) const
		{
			return vec3(x / s, y / s, z / s);
		}
		vec3& operator+=(const vec3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
	};

	struct vec4 {
		union {
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
		};

		// Constructors
		vec4() : x(0), y(0), z(0), w(0) {};
		vec4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {};
		vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};
		vec4(vec2 v, float _z, float _w) : x(v.x), y(v.y), z(_z), w(_w) {};
		vec4(float _x, float _y, vec2 v) : x(_x), y(_y), z(v.x), w(v.y) {};
		vec4(vec3 v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {};

		// Operators
		vec4 operator+(const vec4& v) const
		{
			return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
		}
		vec4 operator-(const vec4& v) const
		{
			return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
		}
		vec4 operator*(const float s) const
		{
			return vec4(x * s, y * s, z * s, w * s);
		}
		vec4 operator/(const float s) const
		{
			return vec4(x / s, y / s, z / s, w / s);
		}
		vec4& operator+=(const vec4& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
			return *this;
		}
		float& operator[](const int i)
		{
			return (&x)[i];
		}
		const float& operator[](const int i) const
		{
			return (&x)[i];
		}
	};

	struct mat4 {
		vec4 columns[4];
		vec4& operator[](const int i) { return columns[i]; };
		const vec4& operator[](const int i) const { return columns[i]; }

		// Constructors
		

		// Operators
		mat4 operator+(const mat4& b) {
			mat4 a = *this;
			mat4 result;

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					result[i][j] = a[i][j] + b[i][j];
			return result;
		}
		mat4 operator*(const mat4& b) {
			mat4 a = *this;
			mat4 result;

			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					result[i][j] =
					a[0][j] * b[i][0] +
					a[1][j] * b[i][1] +
					a[2][j] * b[i][2] +
					a[3][j] * b[i][3];

			return result;
		}
	};

};


#endif