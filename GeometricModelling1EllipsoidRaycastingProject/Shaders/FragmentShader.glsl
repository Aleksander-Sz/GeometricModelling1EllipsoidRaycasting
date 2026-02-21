#version 330 core

in vec2 uv;

uniform vec3 color;
uniform mat4 inverseViewProjection;

struct Ellipsoid
{
	vec3 center;
	vec3 radii;
};

uniform Ellipsoid ellipsoid;

out vec4 FragColor;

void main()
{
	vec4 nearPoint = vec4(uv, -1.0, 1.0);
	vec4 farPoint  = vec4(uv,  1.0, 1.0);

	vec4 nearWorld = inverseViewProjection * nearPoint;
	vec4 farWorld  = inverseViewProjection * farPoint;

	nearWorld /= nearWorld.w;
	farWorld  /= farWorld.w;

	vec3 rayOrigin = nearWorld.xyz;
	vec3 rayDir = normalize(farWorld.xyz - nearWorld.xyz);

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
		FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		return;
	}

	float sqrtD = sqrt(delta);

	float t1 = (-B - sqrtD) / (2.0 * A);
	float t2 = (-B + sqrtD) / (2.0 * A);

	float t = 1e20;

	if(t1 > 0.0) t = t1;
	if(t2 > 0.0) t = min(t, t2);

	if(t == 1e20)
	{
		FragColor = vec4(0,0,0,1);
		return;
	}

	vec3 hitPoint = rayOrigin + t * rayDir;

	vec3 local = hitPoint - ellipsoid.center;

	vec3 normal = vec3(
		local.x / (a*a),
		local.y / (b*b),
		local.z / (c*c)
	);

	normal = normalize(normal);

	FragColor = vec4(normal, 1.0f);
}