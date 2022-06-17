// This is a prototype RGBA_32F inplementation

#version 460 core
precision highp image2D;
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 1) writeonly uniform image2D screen;
layout(rgba32f, binding = 1) readonly uniform image2D r_screen;
layout(rgba32f, binding = 4) writeonly uniform image2D size;
layout(rgba32f, binding = 4) readonly uniform image2D r_size;
layout (rgba32f, binding = 0) readonly uniform image2D c;
layout (rgba32f, binding = 2) readonly uniform image2D token_tree;
layout (rgba32f, binding = 3) readonly uniform image2D cst;

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

void subtitute_tokens(in ivec2 pos, in ivec2 dims, out vec4 token_ids, out vec4 out_sizes){
	int startx = pos.x;
	vec4 seg_origin = imageLoad(r_screen, ivec2(startx,pos.y));
	vec4 seg_origin_sizes = imageLoad(r_size, ivec2(startx,pos.y));
	int i_s = 0;
	int _c_s = 0;
	int start_c = 0;
	int current_char;
	int token_len;
	int tree_row = 0;
	token_ids = seg_origin + vec4(48./255.);
	out_sizes = seg_origin_sizes;
	while((i_s < 4 || tree_row != 0) && i_s < 500){

		int current_char_token_len;
		switch(_c_s){
			case 0:
				current_char = int(seg_origin.r*255.);
				current_char_token_len = int(seg_origin_sizes.r);
				break;
			case 1:
				current_char = int(seg_origin.g*255.);
				current_char_token_len = int(seg_origin_sizes.g);
				break;
			case 2:
				current_char = int(seg_origin.b*255.);
				current_char_token_len = int(seg_origin_sizes.b);
				break;
			case 3:
				current_char = int(seg_origin.a*255.);
				current_char_token_len = int(seg_origin_sizes.a);
				break;
		}
		if(tree_row == 0){
			start_c = _c_s;
			token_len = current_char_token_len;
		}
		if(current_char != 0)
		{
			int jump = int(imageLoad(cst, ivec2(current_char+2,tree_row)).r*255.);
			tree_row = jump;
			_c_s += current_char_token_len-1;
			i_s += current_char_token_len-1;
			if(jump != 0){
				float val = imageLoad(cst, ivec2(current_char+1,tree_row)).g;
				if(val != 0.){
				switch(start_c){
					case 0:
						token_ids.r = val;
						out_sizes.r = float(i_s-start_c);
						break;
					case 1:
						token_ids.g = val;
						out_sizes.g = float(i_s-start_c);
						break;
					case 2:
						token_ids.b = val;
						out_sizes.b = float(i_s-start_c);
						break;
					case 3:
						token_ids.a = val;
						out_sizes.a = float(i_s-start_c);
						break;
					}
				}
					//token_ids.r = val;
				//token_ids.g = 96. /255.;
			}
		}
		//token_len--;
		_c_s++;
		i_s++;
		if(_c_s > 3){
			_c_s -= (_c_s/4)*4;
			startx += int(_c_s/4)+1;
			seg_origin = imageLoad(r_screen, ivec2(startx,pos.y));
			seg_origin_sizes = imageLoad(r_size, ivec2(startx,pos.y));
		}
	}
}

void parse_tokens(in ivec2 pos, in ivec2 dims, out vec4 token_ids, out vec4 out_sizes){
	token_ids = vec4(0.);
	out_sizes = vec4(0.);
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
				
				float val = imageLoad(token_tree, ivec2(current_char+1,tree_row)).g;
				
				switch(rets){
					case 0:
						token_ids.r = val;
						out_sizes.r = float(i-rets);
						//token_ids.r = float(jump_x_total+48)/255.;
						break;
					case 1:
						token_ids.g = val;
						out_sizes.g = float(i-rets);
						//token_ids.g = float(jump_x_total+48)/255.;
						break;
					case 2:
						token_ids.b = val;
						out_sizes.b = float(i-rets);
						//token_ids.b = float(jump_x_total+48)/255.;
						break;
					case 3:
						token_ids.a = val;
						out_sizes.a = float(i-rets);
						//token_ids.a = float(jump_x_total+48)/255.;
						break;
				
				}
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
	vec4 pixel = vec4(170./255.);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	// float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
	// float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]
	vec4 sizes;
	parse_tokens(pixel_coords,dims, pixel, sizes);
	//pixel = vec4(imageLoad(cst, pixel_coords).r,0,0.,1.);
	//pixel = imageLoad(token_tree, pixel_coords);
	barrier();
	imageStore(screen, pixel_coords, pixel);
	imageStore(size, pixel_coords, sizes);
	barrier();
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	subtitute_tokens(pixel_coords,dims,pixel, sizes);
	//pixel = imageLoad(cst, pixel_coords)*500.;
	barrier();
	// imageStore(screen, pixel_coords,( sizes /vec4(255.)) + vec4(48./255.));
	// imageStore(size, pixel_coords, pixel);

	imageStore(screen, pixel_coords, pixel);
	imageStore(size, pixel_coords, sizes);
	

}