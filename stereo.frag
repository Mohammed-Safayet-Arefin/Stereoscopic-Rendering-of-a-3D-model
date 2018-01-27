#version 330


// Any uniforms you have go here

uniform vec3 light_position;



uniform int material_shininess;
uniform float material_kd;
uniform float material_ks;
uniform float material_ka;
uniform float material_ke;


uniform float La;		//Ambient light intensity
uniform float Ld;		//Diffuse light intensity
uniform float Ls;		//Specular light intensity



in vec4 outColor;
in vec3 world_position;
in vec3 world_normal;

// The output-Always a color
layout(location=0) out vec4 fragColor;

void main()
{
    
    /*********Phong Lighting*****************************************************
     Reference:Assignment 4
     ****************************************************************************/
    
    //light direction
    vec3 L = normalize( light_position - world_position);
    //EyePos is (0,0,0) and we are in Eye Coordinates
    vec3 V = normalize(-world_position);//view direction
    //Reflection
    vec3 R = -normalize(reflect(L, world_normal));
    //Same reflection coefficient for each of the light for each of R,G,B
    
    //computing ambient light
    float ambient = La * material_ka;
    
    //computing diffuse light
    float LdotN = max(0, dot(L,world_normal));
    float diffuse = Ld * material_kd * LdotN;
    //computing specular color
    // specular light is white everywhere
    float specular = 0;
    if(LdotN > 0.0)
    {
        specular = material_ks * pow(max(0, dot(R, V)), material_shininess);
    }
    
    //emissive lighting = 0
    float emissive = material_ke;
    
    
    //total lighting
    float light =  ambient+ diffuse + specular+emissive;
    
    //finalize the output-monochromatic color-- with lighting
    fragColor = vec4(outColor[0]+light,outColor[1]+light,outColor[2]+light, 1);
    
    
	//finalize the output-monochromatic color-- without lighting
	//fragColor = outColor;
}







