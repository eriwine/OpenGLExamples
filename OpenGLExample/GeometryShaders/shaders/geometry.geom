#version 450 core

//Takes in a single point and outputs 4 vertices
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//Passed from vertex shader
in vec4 ourColor[];

//Passed to fragment shader
out vec4 vertexColor;
out vec2 texCoord;

uniform float size;

void main(void){

	//Vertex positions relative to point
	const vec4 offsets[4] = vec4[4](
		vec4(-0.5,0.5,0.0,0.0),
		vec4(-0.5,-0.5,0.0,0.0),
		vec4(0.5,0.5,0.0,0.0),
		vec4(0.5,-0.5,0.0,0.0)
	);
	const vec3 colors[4] = vec3[4](
		vec3(0.0f,1.0f,0.0f),
		vec3(0.0f),
		vec3(1.0f,1.0f,0.0f),
		vec3(1.0f,0.0f,0.0f)
	);
	
	for(int i = 0; i < 4; i++){
		vec4 vertexPos = offsets[i] * size + gl_in[0].gl_Position;
		gl_Position = vertexPos;
		texCoord = offsets[i].xy + vec2(0.5f);
		vertexColor = ourColor[0];
		EmitVertex();
	}
	EndPrimitive();
}

