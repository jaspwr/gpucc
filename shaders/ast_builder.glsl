#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
// layout(rgba32f, binding = 1) writeonly uniform image2D screen;
// layout(rgba32f, binding = 2) readonly uniform image2D r_screen;
// layout(rgba32f, binding = 4) writeonly uniform image2D size;
// layout(rgba32f, binding = 0) readonly uniform image2D r_size;
layout (rgba32f, binding = 3) readonly uniform image2D cst;
layout (rgba32f, binding = 5) writeonly uniform image2D screen;
// layout (rgba32f, binding = 6) readonly uniform image2D r_ast_pointers;


struct reimp_screen{
	int _screen;
	int _size;
	int _ast_pointers;
};
layout(std430, binding = 0) readonly buffer _screen_buffer
{
    reimp_screen _r_screen[];
};
layout(std430, binding = 1) writeonly buffer screen_buffer
{
    reimp_screen _w_screen[];
};
struct ast_node
{
    int node_token;
	int depth;
	int volume;
	int operand_count;
	int operands[4];
};

layout(binding = 6) uniform atomic_uint ast_total_count;
layout(std430, binding = 4) buffer ast
{
    ast_node ast_data[];
};

void add_ast_node(in int node_token, in int depth, in int volume, in int operand_count, in int operands[4], out int out_new_ast_node_pointer){
	uint index = atomicCounterIncrement(ast_total_count);
	//uint index = 1;
 	ast_data[index].node_token = node_token;
 	ast_data[index].depth = depth;
 	ast_data[index].volume = volume;
 	ast_data[index].operand_count = operand_count;
 	ast_data[index].operands = operands;
	out_new_ast_node_pointer = int(index);
}

void subtitute_tokens(in ivec2 pos, in ivec2 dims){
	int startx = pos.x;
	//vec4 seg_origin = imageLoad(r_screen, ivec2(startx,pos.y));
	//vec4 seg_origin_sizes = imageLoad(r_size, ivec2(startx,pos.y));
	//vec4 seg_origin_ast_pointers = imageLoad(r_ast_pointers, ivec2(startx,pos.y));
	int i_s = 0;
	int _c_s = 0;
	int start_c = 0;
	int current_char;
	int current_char_ast_pointer;
	int token_len;
	int tree_row = 0;
	int operands[4];
	int operand_pointer = 0;
	//token_ids = seg_origin + vec4(48./255.);
	//token_ids = seg_origin;
	//out_sizes = seg_origin_sizes;
	while((i_s < 4 || tree_row != 0) && i_s < 400){

		int current_char_token_len;
		int _pos = int((pos.x+dims.x*pos.y)*4 + _c_s);	

		current_char = _r_screen[_pos]._screen;
		current_char_token_len = _r_screen[_pos]._size;
		current_char_ast_pointer = _r_screen[_pos]._ast_pointers;
		
		if(tree_row == 0){
			start_c = _c_s;
			operands[0] = 0;
			operands[1] = 0;
			operands[2] = 0;
			operands[3] = 0;
			operand_pointer = 0;
			token_len = current_char_token_len;
		}
		if(current_char != 0)
		{
			int jump = int(imageLoad(cst, ivec2(current_char+2,tree_row)).r*255.);
			tree_row = jump;
			if(current_char > 30){
				//token is significant and needs to be an operand
				operands[operand_pointer] = current_char_ast_pointer;
				operand_pointer++;
			}
			if(current_char_token_len > 0){
				_c_s += current_char_token_len;
				i_s += current_char_token_len;
			}
			if(jump != 0){
				int val = int(imageLoad(cst, ivec2(current_char+1,tree_row)).g*255.);
				int new_ast_node_pointer;
				int depth = 1;
				int volume = 1;
				if(val != 0){
				int _pos = int((pos.x+dims.x*pos.y)*4 + start_c);	






				_w_screen[_pos]._screen = val;
				_w_screen[_pos]._size = i_s-start_c - 1;
				add_ast_node(int(val),depth,volume,operand_pointer,operands, new_ast_node_pointer);
				_w_screen[_pos]._ast_pointers = new_ast_node_pointer;
						
				}
					//token_ids.r = val;
				//token_ids.g = 96. /255.;
			}
		}
		//token_len--;
		if(!(current_char_token_len > 0 && current_char != 0)){
			_c_s++;
			i_s++;
		}
		if(_c_s > 3){
			int div_four = int(float(_c_s)/4.);
			if(div_four < 1)
				div_four = 1;
			_c_s -= div_four*4;
			startx += div_four;
			//seg_origin = imageLoad(r_screen, ivec2(startx,pos.y));
			//seg_origin_sizes = imageLoad(r_size, ivec2(startx,pos.y));
			//seg_origin_ast_pointers = imageLoad(r_ast_pointers, ivec2(startx,pos.y));
		}
	}
}


void main(){
	vec4 pixel = vec4(50./255.);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	// float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
	// float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	subtitute_tokens(pixel_coords,dims);
	//pixel = imageLoad(cst, pixel_coords)*500.;
	barrier();
	//imageStore(screen, pixel_coords,( sizes /vec4(255.)) + vec4(48./255.));
	//imageStore(size, pixel_coords, pixel);

	int _pos = int((pixel_coords.x+dims.x*pixel_coords.y)*4);

	// imageStore(screen, pixel_coords, 
	// vec4(float(_r_screen[_pos]._ast_pointers + 48)/255.,float(_r_screen[_pos+1]._ast_pointers + 48)/255.,float(_r_screen[_pos+2]._ast_pointers + 48)/255.,float(_r_screen[_pos+3]._ast_pointers + 48)/255.)
	// );

	// imageStore(screen, pixel_coords, 
	// vec4(float(ast_data[_pos].node_token + 10)/255.,0.,0.,0.)
	// );
	// imageStore(size, pixel_coords, sizes);
	// imageStore(ast_pointers,pixel_coords,_ast_points);
}