#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform values
uniform sampler2D depthTexture;
uniform sampler2D focusedTexture;
uniform sampler2D blurryTexture;

const float nearPlane = 0.05;
const float farPlane = 4000.0;

// Output fragment color
out vec4 finalColor;

void main() {
	vec2 texCoord = fragTexCoord;
	vec4 focusedColor = texture(focusedTexture, texCoord);

	// Sample depth
	float depth = texture(depthTexture, texCoord).r;

	vec4 blurryColor = texture(blurryTexture, vec2(texCoord.x, 1.0 - texCoord.y));
	finalColor = mix(focusedColor, blurryColor, 0.75);

	/*
	if(depth >= 1.0) {
		finalColor = focusedColor;
	} else {
		vec4 blurryColor = texture(blurryTexture, vec2(texCoord.x, 1.0 - texCoord.y));

		// Linearize depth value
		float linearDepth = (2.0*nearPlane)/(farPlane + nearPlane - depth*(farPlane - nearPlane));
		if(linearDepth < 0.0) linearDepth = 0.0;
		if(linearDepth > 1.0) linearDepth = 1.0;

		if(linearDepth < 0.5) linearDepth = 0.0;
		else linearDepth = (linearDepth - 0.5) / 0.5;

		// Output final color
		//finalColor = vec4(vec3(linearDepth), 1.0);

		finalColor = mix(focusedColor, blurryColor, linearDepth);
	}
	*/
}
