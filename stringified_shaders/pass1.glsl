R"(
// This is a prototype RGBA_32F inplementation

#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 1) writeonly uniform image2D screen;
layout (rgba32f, binding = 0) readonly uniform image2D c;
layout (rgba32f, binding = 2) readonly uniform image2D token_tree;

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

void parse_tokens(in ivec2 pos, in ivec2 dims, out vec4 token_ids){
	token_ids = vec4(32./255.);
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
			in_token = true;

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
				
				float val = imageLoad(token_tree, ivec2(current_char+1,tree_row)).g + 48./255.;
				
				switch(rets){
					case 0:
						token_ids.r = val;
						//token_ids.r = float(jump_x_total+48)/255.;
						break;
					case 1:
						token_ids.g = val;
						//token_ids.g = float(jump_x_total+48)/255.;
						break;
					case 2:
						token_ids.b = val;
						//token_ids.b = float(jump_x_total+48)/255.;
						break;
					case 3:
						token_ids.a = val;
						//token_ids.a = float(jump_x_total+48)/255.;
						break;
				
				}
				tree_row = 0;
				rets++;
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
	vec4 pixel = vec4(170./255.);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	// float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
	// float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]
	parse_tokens(pixel_coords,dims, pixel);
	//pixel = vec4(imageLoad(token_tree, ivec2(gl_GlobalInvocationID.x,gl_GlobalInvocationID.y)).r+48./255.,0,0,0);

	barrier();
	imageStore(screen, pixel_coords, pixel);
})"
