#shader vertex
#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float faceId;

out vec4 v_Color;
out vec2 v_TexCoord;
flat out int v_FaceId;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP *  vec4(position.x, position.y, position.z, 1.0);
	v_Color = vec4(color.x, color.y, color.z, 1.0);
	v_TexCoord = texCoord;
	v_FaceId = int(faceId + 0.5);
}

#shader fragment
#version 330

layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in int v_FaceId;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform int u_Picking;
uniform vec3 u_FaceColors[6];

void main()
{
	if (u_Picking == 1)
	{
		FragColor = u_Color;
	}
	else
	{
		float mask = texture(u_Texture, v_TexCoord).r;
		vec3 sticker = u_FaceColors[v_FaceId];
		vec3 finalColor = mix(vec3(0.0f), sticker, mask);
		FragColor = vec4(finalColor, 1.0f);
	}
}
