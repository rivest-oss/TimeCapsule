#version 330

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
//in vec4 gl_FragCoord;

// Input uniform values
uniform sampler2D sourceImage;

// Output fragment color
out vec4 finalColor;

void main() {
	vec2 uv = fragTexCoord;
	
	vec2 tex_offset = vec2(1.0) / vec2(textureSize(sourceImage, 0));
	vec3 result = texture(sourceImage, uv).rgb * weight[0];
	
	// horizontal
	for(int i = 1; i < 5; i++) {
		result += texture(sourceImage, uv + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
		result += texture(sourceImage, uv - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
	}
	
	finalColor = vec4(result, 1.0);
};
