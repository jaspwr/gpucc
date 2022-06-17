WORKING 2
void subtitute_tokens(in ivec2 pos, in ivec2 dims, out vec4 token_ids, out vec4 out_sizes){
	int startx = pos.x;
	vec4 seg_origin = imageLoad(r_screen, ivec2(startx,pos.y));
	vec4 seg_origin_sizes = imageLoad(r_size, ivec2(startx,pos.y));
	int i_s = 0;
	int _c_s = 0;
	int start_c = 0;
	int current_char;
	int tree_row = 0;
	token_ids = seg_origin + vec4(48./255.);
	out_sizes = seg_origin_sizes+ vec4(48./255.);
	while((i_s < 4 || tree_row != 0) && i_s < 500){
		if(tree_row == 0)
			start_c = _c_s;
		switch(_c_s){
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
		if(current_char != 0)
		{
			int jump = int(imageLoad(cst, ivec2(current_char+2,tree_row)).r*255.);
			tree_row = jump;
			if(jump != 0){
				float val = imageLoad(cst, ivec2(current_char+1,tree_row)).g;
				if(val != 0.){
				switch(start_c){
					case 0:
						token_ids.r = val;
						break;
					case 1:
						token_ids.g = val;
						break;
					case 2:
						token_ids.b = val;
						break;
					case 3:
						token_ids.a = val;
						break;
					}
				}
					//token_ids.r = val;
				//token_ids.g = 96. /255.;
			}
		}
		_c_s++;
		i_s++;
		if(_c_s > 3){
			_c_s = 0;
			startx += 1;
			seg_origin = imageLoad(r_screen, ivec2(startx,pos.y));
			seg_origin_sizes = imageLoad(r_size, ivec2(startx,pos.y));
		}
	}
}