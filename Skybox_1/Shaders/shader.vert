#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

//in vec3 jaw_vp;
//in vec3 kiss_vp;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

//uniform float jaw_wt;
//uniform float kiss_wt;

void main()
{
/*
	float neutral_w = 1.0 - jaw_wt - kiss_wt;
	clamp(neutral_w, 0.f, 1.0f); //setting limits

	// sum of weights
	float sum_w = neutral_w + jaw_wt + kiss_wt;
	float kiss_f = kiss_f / sum_w;
	float jaw_f = jaw_f / sum_w;

*/

	gl_Position = projection * view * model * vec4(pos, 1.0);
	
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
	
	TexCoord = tex;
	
	Normal = mat3(transpose(inverse(model))) * norm;
	
	FragPos = (model * vec4(pos, 1.0)).xyz; 
}