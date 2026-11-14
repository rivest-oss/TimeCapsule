#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform values
uniform sampler2D depthTexture;
uniform sampler2D focusedTexture;
uniform sampler2D blurryTexture;

const float nearPlane = 0.1;
//const float farPlane = 100.0;
const float farPlane = 1.0;

// Output fragment color
out vec4 finalColor;

void main() {
	// Handle potential Y-flipping
	vec2 texCoord = fragTexCoord;
	//if (flipY) texCoord.y = 1.0 - texCoord.y;

	// Sample depth
	float depth = texture(depthTexture, texCoord).r;

	vec4 focusedColor = texture(focusedTexture, texCoord);
	vec4 blurryColor = texture(blurryTexture, vec2(texCoord.x, 1.0 - texCoord.y));

	// Linearize depth value
	float linearDepth = (2.0*nearPlane)/(farPlane + nearPlane - depth*(farPlane - nearPlane));
	linearDepth *= 1666.0;

	// Output final color
	//finalColor = vec4(vec3(linearDepth), 1.0);

	finalColor = mix(focusedColor, blurryColor, linearDepth);
}
