#version 460 core

#define CODEGEN_END 0
#define DEFAULT_CONDITION 1
#define CONDITIAL_EQAULS_TO 2
#define END_CONDITION 3
#define END_INSTRUCTION 4
#define INTERNAL_ELEMENT 5
#define REFERENCE_IDENTIFIER 6
#define REFERENCE_IMPLICIT_TYPE_IDENTIFIER 7
#define INSERSION_IDENTIFIER 8
#define INSERSION_IMPLICIT_TYPE_IDENTIFIER 9
#define SELF_REGISTER_IDENTIFIER 10
#define SELF_REGISTER_VALUE 11
#define SELF_LABEL_IDENTIFIER 12
#define LANGUAGE_TOKEN_IDENTIFIER 13
#define NEWLINE 14
#define IDENTIFIER_IDENTIFIER 15


layout(local_size_x=32,local_size_y=1,local_size_z=1)in;
//layout(binding = 3) uniform atomic_uint ast_total_count;


struct ast_node
{
	int node_token;
	int depth;
	int volume;
	int operand_count;
	int operands[4];
	int final_position;
};

struct organised_node
{
	int node_token;
	int own_organised_location;
};

layout(std430,binding=6)readonly coherent buffer ast
{
	ast_node ast_data[];
};
layout(std430,binding=5)volatile buffer outp
{
	int outp_data[];
};

layout(std430,binding=3)readonly coherent buffer organised_tree
{
	organised_node organised_tree_data[];
};

layout(std430,binding=2)readonly coherent buffer codegen {
	int codegen_data[];
};

void main(){
	barrier();
	memoryBarrier();

    //ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//ivec2 dims = ivec2(500);
	int _pos = int(gl_GlobalInvocationID.x); 
	int own_node_token = organised_tree_data[_pos].node_token;
	if(own_node_token == -1 || codegen_data[own_node_token] == 0) {
		return;
	}
    
	int _token = 0;
	int debug_bail = 0;
	int actual_pos = _pos;
	if (true) {

		int in_intruction_index = 0;
		int codegen_pointer = codegen_data[own_node_token]+1;
		_token = codegen_data[codegen_pointer]-1;
		int len = codegen_data[codegen_data[own_node_token]]-1;
		while(codegen_data[codegen_pointer] != CODEGEN_END && debug_bail < 300) {

			if(in_intruction_index >= len) {
				return;
			}
            int out_pos = ((actual_pos + in_intruction_index) - len);

			// There was an issue with overlapping codegen instructions.
			// This will make it noticable if it happens again.
			
			if(outp_data[out_pos] != 0 && codegen_pointer < 2) {
				outp_data[out_pos] = -2;
				return;
			}

			int pre_token = _token;
            _token = codegen_data[codegen_pointer];


			if (pre_token == REFERENCE_IDENTIFIER) {
                int ori_pos = _pos;
                while(debug_bail < 300){
					//outp_data[out_pos] = 2; break;
					int ast_loc = ast_data[organised_tree_data[_pos].own_organised_location].operands[_token-1];
                    int v = ast_data[ast_loc].final_position;

					// if(ast_loc < 0 && v == 0) {
					// 	// For code position reference
					// 	outp_data[out_pos - 1] = IDENTIFIER_IDENTIFIER; // Very potentially the cause of race conditions
					// 	outp_data[out_pos] = -ast_loc; // Works, none of the weird stuff comes from here
					// 	break;

					// }


					// if(v == 15) { // This is a slightly scuffed fix for a weird issue where radom REFERENCE_IDENTIFIERs ending up in the output
	                //     outp_data[out_pos - 1] = REFERENCE_IDENTIFIER;
					// }



					
                    if (codegen_data[codegen_pointer+1] == INTERNAL_ELEMENT) {
                        codegen_pointer += 2;
                        _token = codegen_data[codegen_pointer];
                        //in_intruction_index += 2;
                        debug_bail += 2;
                        _pos = v;
                    } else {
						int init_ast_loc = ast_loc;

						// Delete this else case it causes issues
						while (debug_bail < 300 && codegen_data[ast_data[ast_loc].node_token] == 0) {
							// Decend through passthrough nodes

							debug_bail += 1;
							int ori_loc = ast_loc;
							ast_loc = ast_data[ast_loc].operands[0];
							if (ast_loc == 0) {
								// TODO
								// I don't know why but sometimes the child of a passthrough node
								// ends up as operand 1 instead of 0.
								// Probably an issue in ast_builder with ast_extras.
								// Should fix properly as it could cause heaps of issues later.
								ast_loc = ast_data[ori_loc].operands[1];
								if (ast_loc < 0) {
									init_ast_loc = ast_loc;
								}
								v = ast_data[ast_loc].final_position;
								break;
							}
							v = ast_data[ast_loc].final_position;
							if (ast_loc < 0) {
								init_ast_loc = ast_loc;
								break;
							}

						}
						if (init_ast_loc < 0) {
							v = init_ast_loc;
						} 
				
						// If there is no internal element, then we are done
	                    outp_data[out_pos] = v;
                        break;
                    }
                }
                _pos = ori_pos;

			} else if (_token == SELF_REGISTER_VALUE) {
                outp_data[out_pos] = _pos;
            } 
			// else if (pre_token == IDENTIFIER_IDENTIFIER){
			// 	outp_data[out_pos] = 1; //What...
			// }
            else {
				outp_data[out_pos] = _token;
			}


			in_intruction_index++;
			codegen_pointer++;
			debug_bail++;
		}
	}
	// imageStore(screen,ivec2(pixel_coords.x, pixel_coords.y),
	// 	vec4(float(organised_tree_data[_pos].node_token+48)/255.,48./255.,0.,0.)
	// 	);
	memoryBarrierBuffer();
	barrier();
}