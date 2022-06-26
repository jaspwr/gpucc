R"(
// This is a prototype RGBA_32F inplementation

#version 460 core
//precision highp image2D;
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 5) writeonly uniform image2D screen;
//layout(rgba32f, binding = 1) readonly uniform image2D r_screen;
//layout(rgba32f, binding = 4) writeonly uniform image2D size;
//layout(rgba32f, binding = 4) readonly uniform image2D r_size;
layout (rgba32f, binding = 0) readonly uniform image2D c;
layout (rgba32f, binding = 2) readonly uniform image2D token_tree;
layout (rgba32f, binding = 3) readonly uniform image2D cst;
//layout (rgba32f, binding = 5) writeonly uniform image2D ast_pointers;

struct reimp_screen{
	int _screen;
	int _size;
	int _ast_pointers;
};
layout(std430, binding = 1) writeonly buffer screen_buffer
{
    reimp_screen _w_screen[];
};

void classify(in uint cha, out int _class){
	//TODO: optimise with frequency analysis
	if(cha < 33)
		_class = 1; //empty
	else if(cha > 47 && cha < 58)
		_class = 0; //alphanum
	else if(cha == 59)
		_class = 3; //semicolon
	else if(cha > 64 && cha < 91)
		_class = 0; //alphanum
	else if(cha < 96 && cha < 122)
		_class = 0; //alphanum
	else
		_class = 2; //punctuation
}

void fclassify(in float cha, out int _class){
	//TODO: optimise with frequency analysis
	if(cha < 33./255.)
		_class = 1; //empty
	else if(cha > 47./255. && cha < 58./255.)
		_class = 0; //alphanum
	else if(cha == 0.23137254901960785)
		_class = 3; //semicolon
	else if(cha > 64./255. && cha < 91./255.)
		_class = 0; //alphanum
	else if(cha < 96./255. && cha < 122./255.)
		_class = 0; //alphanum
	else
		_class = 2; //punctuation
}


void parse_tokens(in ivec2 pos, in ivec2 dims){
	//token_ids = vec4(0.);
	//out_sizes = vec4(0.);
	//vec_ast_pointers = vec4(0.);
	vec4 seg_origin = imageLoad(c, ivec2(pos.x+dims.x*pos.y,0));
	vec4 seg_pre = imageLoad(c, ivec2(pos.x+dims.x*pos.y-1,0));
	//token_ids = seg_origin;
	bool start_chars_0[4];
	int c1;
	int c2;
	fclassify(seg_pre.a,c1);
	fclassify(seg_origin.r,c2);
	start_chars_0[0] = (c1 != c2);
	fclassify(seg_origin.g,c1);
	start_chars_0[1] = (c1 != c2);
	fclassify(seg_origin.b,c2);
	start_chars_0[2] = (c1 != c2);
	fclassify(seg_origin.a,c1);
	start_chars_0[3] = (c1 != c2);
	
	bool in_token = false;
	int i = 0;
	int _c = 0;
	int seg = 0;
	int tree_row = 0;
	int jump_x = 0;
	int rets = 0;
	int jump_x_total = 0;
	while((in_token || i < 4) && i < 50){
		//check for start of token
		if(start_chars_0[_c])
		{
			in_token = true;
			rets = i;
		}

		int current_char;
		if(in_token){
			//advance through token
			switch(_c){
				case 0:
					current_char = int(seg_origin.r*255.);
					break;
				case 1:
					current_char = int(seg_origin.g*255.);
					break;
				case 2:
					current_char = int(seg_origin.b*255.);
					break;
				case 3:
					current_char = int(seg_origin.a*255.);
					break;
			}
			jump_x = int(imageLoad(token_tree, ivec2(current_char+2,tree_row)).r*255.);
			jump_x_total += jump_x;
		}
		
		i++;
		_c++;
		if(_c > 3)
		{
			seg++;
			seg_pre = seg_origin;
			seg_origin = imageLoad(c, ivec2(pos.x+dims.x*pos.y + seg,0));
			

			fclassify(seg_pre.a,c1);
			fclassify(seg_origin.r,c2);
			start_chars_0[0] = (c1 != c2);
			fclassify(seg_origin.g,c1);
			start_chars_0[1] = (c1 != c2);
			fclassify(seg_origin.b,c2);
			start_chars_0[2] = (c1 != c2);
			fclassify(seg_origin.a,c1);
			start_chars_0[3] = (c1 != c2);
			_c = 0;
		}
		//check for end of token
		tree_row = jump_x;
		if(in_token)
		{
			if(start_chars_0[_c])
			{
				
				in_token = false;
				//finalise return token identifier
				//token_ids.r = imageLoad(token_tree, ivec2(current_char,tree_row)).g + 48./255.;
				//token_ids.r = 72./255.;
				//token_ids.g = float(tree_row+48)/255.;
				
				int val = int(imageLoad(token_tree, ivec2(current_char+1,tree_row)).g*255.);
				int _pos = int((pos.x+dims.x*pos.y)*4 + rets);

				_w_screen[_pos]._screen = val;
				_w_screen[_pos]._size = i-rets;
				_w_screen[_pos]._ast_pointers = -_pos;
				
				tree_row = 0;
				//rets++;
			}else{

				
				// if(jump_x == 0)
				// {
				// 	//token_ids.r = 0.23137254901960785;
				// 	in_token = false;
				// 	tree_row = 0;
				// }
			}

			
		}
	}

}

	
void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	// float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
	// float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]
	parse_tokens(pixel_coords,dims);
	//pixel = vec4(imageLoad(cst, pixel_coords).r,0,0.,1.);
	//pixel = imageLoad(token_tree, pixel_coords);

	int _pos = int((pixel_coords.x+dims.x*pixel_coords.y)*4);

	barrier();
	// imageStore(screen, pixel_coords, 
	// vec4(float(-_w_screen[_pos]._ast_pointers + 48)/255.,float(_w_screen[_pos+1]._screen + 48)/255.,float(_w_screen[_pos+2]._screen + 48)/255.,float(_w_screen[_pos+3]._screen + 48)/255.)
	// );
	//imageStore(screen, pixel_coords, imageLoad(c, ivec2(pixel_coords.x+dims.x*pixel_coords.y,0)));
	//imageStore(size, pixel_coords, sizes);
	//imageStore(ast_pointers, pixel_coords, vec_ast_pointers);
})"
