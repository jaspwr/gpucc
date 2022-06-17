WORKING
	int startx = pos.x;
	vec4 seg_origin = imageLoad(r_screen, ivec2(startx,pos.y));
	int i_s = 0;
	int _c_s = 0;
	int start_c = 0;
	int current_char;
	int tree_row = 0;

	current_char = int(seg_origin.r*255.);

	token_ids = seg_origin + vec4(48./255.);
	int jump = int(imageLoad(cst, ivec2(current_char+2,tree_row)).r*255.);
	tree_row = jump;
	if(jump != 0){
		float val = imageLoad(cst, ivec2(current_char+1,tree_row)).g;
		if(val != 0.)
			token_ids.r = val;
		//token_ids.g = 96. /255.;
	}