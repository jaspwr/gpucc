#version 460 core

layout(local_size_x=8,local_size_y=4,local_size_z=1)in;
layout(rgba32f,binding=5)writeonly uniform image2D screen;
layout(binding=6)uniform atomic_uint ast_total_count;

struct ast_node
{
	int node_token;
	int depth;
	int volume;
	int operand_count;
	int operands[4];
};

struct organised_node
{
	int node_token;
	int own_organised_location;
	int operand_count;
	int operands[4];
};

layout(std430,binding=4)buffer ast
{
	ast_node ast_data[];
};

layout(std430,binding=6)buffer organised_tree
{
	organised_node organised_tree_data[];
};

void main(){
	ivec2 pixel_coords=
	ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims=imageSize(screen);
	
	int _pos=int((pixel_coords.x+dims.x*pixel_coords.y));
	uint entry_node=atomicCounter(ast_total_count)-1;
	int wokring_volume=0;
	int working_node=int(entry_node);
	
	bool found=false;
	int iter=0;
	wokring_volume=ast_data[working_node].volume;
	while(!found&&iter<100){
		iter++;
		int lower_vol=0;
		//TODO: sort out orderings
		if(wokring_volume==_pos)
		{
			found=true;
			break;
		}
		wokring_volume-=1;
		for(int i=0;i<ast_data[working_node].operand_count;i++){
			
			int current_operand=ast_data[working_node].operands[i];
			if(current_operand>=0){
				
				int current_operand_volume=ast_data[current_operand].volume;
				
				int lower_vol=wokring_volume-current_operand_volume;
				if(_pos>=lower_vol&&_pos<wokring_volume){
					//wokring_volume=lower_vol;
					working_node=current_operand;
					break;
				}
				wokring_volume=lower_vol;
			}
		}
	}
	organised_tree_data[_pos].node_token=ast_data[working_node].node_token;
	if(!found)
	organised_tree_data[_pos].node_token=0;
	barrier();
	imageStore(screen,pixel_coords,
		vec4(float(ast_data[_pos].node_token+48)/255.,0.,0.,0.)
	);
}