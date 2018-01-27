#version 330


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;


//in vec3 position;
out vec4 outColor;
out vec3 world_position;
out vec3 world_normal;

uniform mat4 Model_View_Projection;




//applying only one color at a time
vec4 monochromatic(){
    float r=0.6, g=0.2, b=0.3;
    
    return vec4(r,g,b,1);
}

void main()
{
    //The current vertex position is transformed to eye space
    world_position = mat3(Model_View_Projection) * position;
    world_normal = normalize(mat3(Model_View_Projection) * normal);
    
    
	//Output position
	gl_Position =  Model_View_Projection * vec4(position, 1.0);
    
    //passing the monochromatic color
    vec4 mono_color =monochromatic();
	outColor = mono_color;
}

