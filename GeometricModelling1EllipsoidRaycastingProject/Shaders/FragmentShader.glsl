#version 330 core

in vec2 uv;

uniform vec3 color;
uniform mat4 inverseViewProjection;
uniform vec2 resolution;

struct Ellipsoid
{
	vec3 center;
	vec3 radii;
};

uniform Ellipsoid ellipsoid;

out vec4 FragColor;

vec3 castRay(vec3 rayOrigin, vec3 rayDir, Ellipsoid ellipsoid, vec3 backgroundColor)
{
// solving the equation of the ellipsoid, in a simplified form
	vec3 oc = rayOrigin - ellipsoid.center;
	float a = ellipsoid.radii.x;
	float b = ellipsoid.radii.y;
	float c = ellipsoid.radii.z;

	float A =
		(rayDir.x * rayDir.x) / (a*a) +
		(rayDir.y * rayDir.y) / (b*b) +
		(rayDir.z * rayDir.z) / (c*c);

	float B =
		2.0 * (
			(oc.x * rayDir.x) / (a*a) +
			(oc.y * rayDir.y) / (b*b) +
			(oc.z * rayDir.z) / (c*c)
		);

	float C =
		(oc.x * oc.x) / (a*a) +
		(oc.y * oc.y) / (b*b) +
		(oc.z * oc.z) / (c*c)
		- 1.0;

	float delta = B*B - 4.0*A*C;
	if(delta < 0.0)
	{
		return backgroundColor;
	}

	float sqrtD = sqrt(delta);

	float t1 = (-B - sqrtD) / (2.0 * A);
	float t2 = (-B + sqrtD) / (2.0 * A);

	float t = 1e20;

	if(t1 > 0.0) t = t1;
	if(t2 > 0.0) t = min(t, t2);

	if(t == 1e20)
	{
		return backgroundColor;
	}

	vec3 hitPoint = rayOrigin + t * rayDir;

	vec3 local = hitPoint - ellipsoid.center;

	vec3 normal = vec3(
		local.x / (a*a),
		local.y / (b*b),
		local.z / (c*c)
	);

	normal = normalize(normal);
	return normal;
}

void main()
{
	vec2 pixelSizeInNDC = 2.0 / resolution;

	vec3 color = vec3(0.0f);
	for(int i=0; i<16; i++)
	{
		float seed = uv.x + uv.y * 133.0 + float(i) * 256.0;
		vec2 randomJitter = vec2(fract(sin(seed)*43758.5453), fract(cos(seed)*43758.5453)) - 0.5;
		vec2 uvJittered = uv + randomJitter * pixelSizeInNDC;
		vec4 nearPoint = vec4(uvJittered, -1.0, 1.0);
		vec4 farPoint  = vec4(uvJittered,  1.0, 1.0);

		vec4 nearWorld = inverseViewProjection * nearPoint;
		vec4 farWorld  = inverseViewProjection * farPoint;

		nearWorld /= nearWorld.w;
		farWorld  /= farWorld.w;

		vec3 rayOrigin = nearWorld.xyz;
		vec3 rayDir = normalize(farWorld.xyz - nearWorld.xyz);

		color += castRay(rayOrigin, rayDir, ellipsoid, vec3(0.0f));
	}
	color /= 16.0f;

	FragColor = vec4(color, 1.0f);
}