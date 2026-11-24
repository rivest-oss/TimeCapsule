#version 330

//const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform float weight[5];

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform values
uniform sampler2D sourceImage;

// Output fragment color
out vec4 finalColor;

void main() {
	vec2 tex_offset = vec2(1.0) / vec2(textureSize(sourceImage, 0));
	vec3 result = texture(sourceImage, fragTexCoord).rgb * weight[0];
	
	// rolled:
	// for(int i = 1; i < 5; i++) {
	// 	result += texture(sourceImage, fragTexCoord + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
	// 	result += texture(sourceImage, fragTexCoord - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
	// }

	// unrolled
	result += texture(sourceImage, fragTexCoord + vec2(tex_offset.x * 1.0, 0.0)).rgb * weight[1];
	result += texture(sourceImage, fragTexCoord - vec2(tex_offset.x * 1.0, 0.0)).rgb * weight[1];
	result += texture(sourceImage, fragTexCoord + vec2(tex_offset.x * 2.0, 0.0)).rgb * weight[2];
	result += texture(sourceImage, fragTexCoord - vec2(tex_offset.x * 2.0, 0.0)).rgb * weight[2];
	result += texture(sourceImage, fragTexCoord + vec2(tex_offset.x * 3.0, 0.0)).rgb * weight[3];
	result += texture(sourceImage, fragTexCoord - vec2(tex_offset.x * 3.0, 0.0)).rgb * weight[3];
	result += texture(sourceImage, fragTexCoord + vec2(tex_offset.x * 4.0, 0.0)).rgb * weight[4];
	result += texture(sourceImage, fragTexCoord - vec2(tex_offset.x * 4.0, 0.0)).rgb * weight[4];
	
	finalColor = vec4(result, 1.0);
};
