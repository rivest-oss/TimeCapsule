#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec4 gl_FragCoord;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add your custom variables here

#define MAX_LIGHTS 4
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

struct Light {
	int enabled;
	int type;
	vec3 position;
	vec3 target;
	vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform vec3 viewPos;

float random(in vec2 p) {
	return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453).x;
};

void main() {
	vec3 lightPos = vec3(7.5116, 11.8093, 7.8237);
	vec3 lightTarget = vec3(5.1272, 8.0162, 4.8359);

	// Texel color fetching from texture sampler
	vec4 texelColor = texture(texture0, fragTexCoord);
	vec3 lightDot = vec3(1.0);
	vec3 normal = normalize(fragNormal);
	vec3 viewD = normalize(viewPos - fragPosition);
	vec3 specular = vec3(1.0);

	vec4 tint = colDiffuse*fragColor;

	//light = -normalize(lights[i].target - lights[i].position);

	vec3 lightDiff = normalize(lightPos - fragPosition);
	float lightAngle = clamp(dot(normal, lightDiff), 0.25, 1.0);

	finalColor = texelColor;
	finalColor *= vec4(vec3(lightAngle), 1.0);

	float texelNoise = random(vec2(gl_FragCoord.x, gl_FragCoord.y) * vec2(0.1));
	finalColor = vec4(
		finalColor.r * 0.97 + texelNoise * 0.03 * finalColor.r,
		finalColor.g * 0.97 + texelNoise * 0.03 * finalColor.g,
		finalColor.b * 0.97 + texelNoise * 0.03 * finalColor.b,
		finalColor.a
	);
};
