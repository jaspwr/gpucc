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


layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
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
	int operand_count;
	int operands[4];
};

layout(std430,binding=4)readonly buffer ast
{
	ast_node ast_data[];
};
layout(std430,binding=5)writeonly buffer outp
{
	int outp_data[];
};

layout(std430,binding=6)readonly buffer organised_tree
{
	organised_node organised_tree_data[];
};

layout(std430,binding=2)readonly buffer codegen{
	int codegen_data[];
};

void main(){
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = ivec2(500);
	int _pos = int((pixel_coords.x + dims.x * pixel_coords.y)); 
	int own_node_token = organised_tree_data[_pos].node_token;
	
    
	int _token = 0;
	int debug_bail = 0;
	if (true) {
        barrier();
		int in_intruction_index = 0;
		int codegen_pointer = codegen_data[own_node_token]+1;
		_token = codegen_data[codegen_pointer];
		int len = codegen_data[codegen_data[own_node_token]]-1;
		while(codegen_data[codegen_pointer] > 0 && debug_bail < 300) {
	        barrier();

            int out_pos = ((pixel_coords.x + in_intruction_index) - len) + pixel_coords.y* dims.x;
			int pre_token = _token;
            _token = codegen_data[codegen_pointer];

			if (pre_token == REFERENCE_IDENTIFIER) {
                int ori_pos = _pos;

                while(debug_bail < 300){
					int ast_loc = ast_data[organised_tree_data[_pos].own_organised_location].operands[_token-1];
					if(ast_loc < 0){
						// For code position reference
						outp_data[out_pos - 1] = IDENTIFIER_IDENTIFIER;
						outp_data[out_pos] = -ast_loc;
						break;

					}
                    int v = ast_data[ast_loc].final_position;


                    outp_data[out_pos] = v;
                    if (codegen_data[codegen_pointer+1] == INTERNAL_ELEMENT) {
                        codegen_pointer += 2;
                        _token = codegen_data[codegen_pointer];
                        in_intruction_index += 2;
                        debug_bail += 2;
                        _pos = v;
                    } else {
                        break;
                    }
                }
                _pos = ori_pos;

			} else if (_token == SELF_REGISTER_VALUE) {
                outp_data[out_pos] = _pos;
            }
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
}