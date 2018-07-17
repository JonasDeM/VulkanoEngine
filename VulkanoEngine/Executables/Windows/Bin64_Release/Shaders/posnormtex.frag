#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 lightDir = vec3(-0.577f, -0.577f, 0.577f);
	vec4 color = texture(texSampler, fragTexCoord);
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(fragNormal, -lightDir);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = clamp(diffuseStrength,0.0,1.0);
	
    outColor = vec4( color.rgb * diffuseStrength , color.a );
}