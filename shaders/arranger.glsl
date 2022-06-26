#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout (rgba32f, binding = 5) writeonly uniform image2D screen;

struct ast_node
{
    int node_token;
	int depth;
	int volume;
	int operand_count;
	int operands[4];
};

layout(std430, binding = 4) buffer ast
{
    ast_node ast_data[];
};

void main(){
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	int _pos = int((pixel_coords.x+dims.x*pixel_coords.y));




    barrier();
	imageStore(screen, pixel_coords, 
	vec4(float(ast_data[_pos].operands[0]+48)/255.,0.,0.,0.)
	);
}