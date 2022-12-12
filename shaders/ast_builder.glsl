#version 460 core

layout(local_size_x=8,local_size_y=4,local_size_z=1)in;
// layout(rgba32f, binding = 1) writeonly uniform image2D screen;
// layout(rgba32f, binding = 2) readonly uniform image2D r_screen;
// layout(rgba32f, binding = 4) writeonly uniform image2D size;
// layout(rgba32f, binding = 0) readonly uniform image2D r_size;
layout(rgba32f,binding=3)readonly uniform image2D cst;
// layout (rgba32f, binding = 6) readonly uniform image2D r_ast_pointers;

struct parse_tree_extra{
	int operand_order[10];
	int pre_char_exclusions_counter;
	int pre_char_exclusions[30];
	int symetrical_exclusion;
};

struct reimp_screen{
	int _screen;
	int _size;
	int _ast_pointers;
};
layout(std430,binding=0)readonly buffer _screen_buffer
{
	reimp_screen _r_screen[];
};
layout(std430,binding=1)writeonly buffer screen_buffer
{
	reimp_screen _w_screen[];
};

layout(std430,binding=2)buffer codegen{
	int codegen_data[];
};
struct ast_node
{
	int node_token;
	int depth;
	int volume;
	int operand_count;
	int operands[4];
	int final_position;
};

layout(binding=6)uniform atomic_uint ast_total_count;
layout(std430,binding=4)writeonly buffer _w_ast
{
	ast_node w_ast_data[];
};

layout(std430,binding=6)readonly buffer _r_ast
{
	ast_node r_ast_data[];
};

layout(std430,binding=7)readonly buffer _parse_tree_extra
{
	parse_tree_extra ast_extra[];
};

void add_ast_node(in int node_token,in int depth,in int volume,in int operand_count,in int operands[4],out int out_new_ast_node_pointer){
	// Note: This is one of the slowest function in the entire compiler due to the atomic incrementer.
	//		 The order that the AST nodes are stored in does not matter and this was only one way of
	//		 storing them concurrently and I beleve there is a better way to do this.
	uint index=atomicCounterIncrement(ast_total_count);
	w_ast_data[index].node_token=node_token;
	w_ast_data[index].depth=depth;
	w_ast_data[index].volume=volume;
	w_ast_data[index].operand_count=operand_count;
	w_ast_data[index].operands=operands;
	out_new_ast_node_pointer=int(index);
}

void subtitute_tokens(in ivec2 pos,in ivec2 dims,in int i_offset){
	int tree_row=0;
	int internal_char=0;
	int anything_token_terminator=0;
	bool expects_anything=false;
	int final_val=0;
	int final_val_size=0;
	int final_alph=0;
	int return_pos=int((pos.x+dims.x*pos.y)*4.)+i_offset;
	int operands[10];
	int operand_pointer=0;


	// Ideally there would not have to be a limit on loop_counter but other issues in the loop cause it to 
	// loop forever in some cases and need to be fixed first.
	int loop_counter = 0;
	while((internal_char==0||tree_row!=0)&&loop_counter<100){
		int pos_c=return_pos+internal_char;
		int current_char=_r_screen[pos_c]._screen;
		int current_size=_r_screen[pos_c]._size;
		int current_ast_pointer=_r_screen[pos_c]._ast_pointers;
		//deal with ignores
		bool dont_ignore;
		if(expects_anything){
			//_w_screen[_pos]._screen=2;
			if(current_char==anything_token_terminator){
				//_w_screen[_pos]._screen=3;
				dont_ignore=true;
				expects_anything=false;
			}else{
				dont_ignore=false;
			}
			//debug_bail++;
		}else{
			dont_ignore=current_char!=0;
		}
		if(dont_ignore){
			//token is significant and needs to be an operand
			operands[operand_pointer]=current_ast_pointer;
			operand_pointer++;

			vec4 cst_pix=imageLoad(cst,ivec2(current_char+2,tree_row));
			if(cst_pix.b!=0.){
				anything_token_terminator=int(cst_pix.b*255.);
				expects_anything=true;
			}
			int jump=int(cst_pix.r*255.);
			tree_row=jump;
			int val=int(imageLoad(cst,ivec2(current_char+1,tree_row)).g*255.);
			int alph=int(imageLoad(cst,ivec2(current_char+1,tree_row)).a*255.);
			if(val!=0)
			{
				final_val_size=internal_char+current_size;
				final_val=val;
				final_alph=alph;
			}

			internal_char+=current_size;
			if(current_size==0)
			internal_char++;
		}else{
			internal_char++;
		}
		loop_counter++;
	}
	if(ast_extra[final_alph].pre_char_exclusions_counter>0){
		int pre_token = 0;
		int post_token = 0;
		bool sym = (ast_extra[final_alph].symetrical_exclusion != 0);
		if(return_pos>1){
			int _i=1;
			while(_i<100&&return_pos-_i>-1)
			{
				pre_token=_r_screen[return_pos-_i]._screen;
				if(pre_token!=0)
				break;
				_i++;
			}
			if(sym){
				_i=0;
				while(_i < 100)
				{
					post_token=_r_screen[return_pos+final_val_size+_i]._screen;
					if(post_token != 0)
						break;
					_i++;
				}
			}
		}
		for(int i=0;i<ast_extra[final_alph].pre_char_exclusions_counter;i++){
			//_r_screen[_pos].screen+=3;
			if(ast_extra[final_alph].pre_char_exclusions[i] == pre_token)
				final_val = 0;
			if(sym && ast_extra[final_alph].pre_char_exclusions[i] == post_token)
				final_val = 0;
		}
		// if(sym)
		// 	final_val = 0;
	}

	if(final_val!=0){
		int final_operands[4];
		int depth=1;
		int volume=1;
		if(codegen_data[final_val] != 0)
			volume = codegen_data[codegen_data[final_val]];
		int largest_operand_position=0;
		for(int i=0;i<10;i++){
			int ind=ast_extra[final_alph].operand_order[i];
			if(ind>1){
				largest_operand_position=ind;
				final_operands[ind-2]=operands[i];
				volume+=r_ast_data[operands[i]].volume;
			}
		}
		//final_operands[0]=ast_extra[final_alph].operand_order[0];

		// for(int _i=0;_i<4;_i++){
			// 	if(operands[_i]>0){
				// 		volume+=r_ast_data[operands[_i]].volume;
				// 		if(r_ast_data[operands[_i]].depth>=depth){
					// 			depth=r_ast_data[operands[_i]].depth+1;
				// 		}
			// 	}else{
				// 		volume+=1;
			// 	}
		// }
		_w_screen[return_pos]._screen=final_val;
		_w_screen[return_pos]._size=final_val_size;
		int new_ast_node_pointer;
		add_ast_node(final_val,depth,volume,largest_operand_position,final_operands,new_ast_node_pointer);
		_w_screen[return_pos]._ast_pointers=new_ast_node_pointer;
	}else{
		_w_screen[return_pos]=_r_screen[return_pos];
	}
}

void main(){
	vec4 pixel=vec4(50./255.);
	ivec2 pixel_coords=ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims=ivec2(500);

	// float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
	// float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]
	int t_pos=int((pixel_coords.x+dims.x*pixel_coords.y));
	w_ast_data[t_pos]=r_ast_data[t_pos];
	barrier();
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	for(int i=0;i<4;i++){
		subtitute_tokens(pixel_coords,dims,i);
	}
	//pixel = imageLoad(cst, pixel_coords)*500.;
	barrier();
	//imageStore(screen, pixel_coords,( sizes /vec4(255.)) + vec4(48./255.));
	//imageStore(size, pixel_coords, pixel);

	int _pos=int((pixel_coords.x+dims.x*pixel_coords.y))*4;
	barrier();
	//_r_screen[_pos].screen+=3;
	// imageStore(screen,pixel_coords,
		// 	vec4(float(_r_screen[_pos]._screen+48)/255.,float(_r_screen[_pos+1]._screen+48)/255.,float(_r_screen[_pos+2]._screen+48)/255.,float(_r_screen[_pos+3]._screen+48)/255.)
	// );
	// imageStore(screen,pixel_coords,
		// 	vec4(float(ast_extra[_pos].pre_char_exclusions[1]+48)/255.,0.,0.,0.)
	// );
	// imageStore(size, pixel_coords, sizes);
	// imageStore(ast_pointers,pixel_coords,_ast_points);

	//imageStore(screen,pixel_coords,imageLoad(cst,ivec2(pixel_coords.x,pixel_coords.y))*255.);
}