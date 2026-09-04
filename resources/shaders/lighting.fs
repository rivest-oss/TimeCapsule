#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

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

void main() {
	/*
	// Texel color fetching from texture sampler
	vec4 texelColor = texture(texture0, fragTexCoord);
	vec3 lightDot = vec3(0.0);
	vec3 normal = normalize(fragNormal);
	vec3 viewD = normalize(viewPos - fragPosition);
	vec3 specular = vec3(0.0);

	vec4 tint = colDiffuse*fragColor;

	// NOTE: Implement here your fragment shader code

	for(int i = 0; i < MAX_LIGHTS; i++) {
		if(lights[i].enabled == 1) {
			vec3 light = vec3(0.0);

			if(lights[i].type == LIGHT_DIRECTIONAL) {
				light = -normalize(lights[i].target - lights[i].position);
			}

			if(lights[i].type == LIGHT_POINT) {
				light = normalize(lights[i].position - fragPosition);
			}

			float NdotL = max(dot(normal, light), 0.0);
			lightDot += lights[i].color.rgb*NdotL;

			float specCo = 0.0;
			if(NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16.0); // 16 refers to shine
			specular += specCo;
		}
	}

	finalColor = (texelColor*((tint + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
	finalColor += texelColor*(ambient/10.0)*tint;

	// Gamma correction
	finalColor = pow(finalColor, vec4(1.0/2.2));
	*/

	/*
	// Texel color fetching from texture sampler
	vec4 texelColor = texture(texture0, fragTexCoord);
	vec3 lightDot = vec3(1.0);
	vec3 normal = normalize(fragNormal);
	vec3 viewD = normalize(viewPos - fragPosition);
	vec3 specular = vec3(1.0);

	vec4 tint = colDiffuse*fragColor;

	// NOTE: Implement here your fragment shader code

	finalColor = texelColor;
	*/

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
}