#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 lightDir = vec3(-0.577f, -0.577f, 0.577f);
	vec3 color_rgb = fragColor.rgb;
	float color_a = fragColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(fragNormal, -lightDir);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = clamp(diffuseStrength,0.0,1.0);
	color_rgb = color_rgb * diffuseStrength;
	
    outColor = vec4( color_rgb , color_a );
}