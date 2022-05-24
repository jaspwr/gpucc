R"(
#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 1) writeonly uniform image2D screen;
layout (rgba32f, binding = 0) readonly uniform image2D c;
void main()
{
	vec4 pixel = vec4(0.075, 0.133, 0.173, 1.0);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
	float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]
    //vec4(x,y,0.0,0.0)
	pixel = vec4(imageLoad(c, ivec2(gl_GlobalInvocationID.x+dims.x*gl_GlobalInvocationID.y,0)));
	barrier();
	imageStore(screen, pixel_coords, pixel);
}
)"