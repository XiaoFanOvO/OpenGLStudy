#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 uv;


void main()
{
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);//这里得到的还不是NDC坐标,需要在后续做其次除法之后才是
	uv = aUV;
}