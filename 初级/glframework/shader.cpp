#include "shader.h"
#include "../wrapper/checkError.h"
#include <string>
#include <fstream>
#include <sstream>	
#include <iostream>


Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	//最终的目的是绑定了两个shader的program对象 可以直接通过mprogram调用
	//声明装入shader字符串的两个string
	std::string vertexCode;
	std::string fragmentCode;

	//声明用于读取vs跟fs文件的inFileStream
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	
	//保证ifstream遇到问题时可以抛出异常
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); //文件读取失败和文件损坏
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); //文件读取失败和文件损坏

	try
	{
		//1 打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		//2 将文件输入流当中的字符串输入到stringStream里面
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();  //从文件流缓存中读出字符串 并灌入到vShaderStream这个流里面
		fShaderStream << fShaderFile.rdbuf(); 

		//3 关闭文件
		vShaderFile.close();
		fShaderFile.close();

		//4 将字符串从stringStream当中读取出来 转化到code String当中
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR: SHADER File Error: " << e.what() << std::endl;
	}

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();
	//1 创建shader程序(vs fs)
	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	//2 为shader程序输入shader代码
	glShaderSource(vertex, 1, &vertexShaderSource, NULL); //以\0自然结尾的字符串 不需要告诉他长度系统自己就知道在哪里结尾
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

	//3 执行shader代码编译
	glCompileShader(vertex);
	checkShaderErrors(vertex, "COMPILE");

	glCompileShader(fragment);
	checkShaderErrors(fragment, "COMPILE");
	//4 创建一个Program壳子
	mProgram = glCreateProgram();

	//5 将vs与fs编译好的结果放到program这个壳子里
	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);

	//6 执行program的链接操作,形成最终可执行shader程序
	glLinkProgram(mProgram);
	//检查链接错误
	checkShaderErrors(mProgram, "LINK");
	//清理
	glDeleteShader(vertex);//已经编译进program了 所以这里直接清理掉
	glDeleteShader(fragment);
}

Shader::~Shader() {}

void Shader::begin() {
	GL_CALL(glUseProgram(mProgram));
}

void Shader::end() {
	GL_CALL(glUseProgram(0));
}

//调用修改uniform方法之前一定要先useprogram!
void Shader::setFloat(const std::string& name, float value) {
	//1 通过名称拿到Uniform变量的位置Location
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	//这个方法查找的范围是整个program 不管是vs还是fs 只要有都行 
	//如果重名的话两个都会发生作用 相当于一个 
	//所以如果需要同时修改vs和fs里的同名uniform变量 只需要调用一次即可

	//2 通过Location更新Uniform变量的值
	GL_CALL(glUniform1f(location, value));
}

void Shader::setVector3(const std::string& name, float x, float y, float z) {
	//1 通过名称拿到Uniform变量的位置Location
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	//2 通过Location更新Uniform变量的值
	glUniform3f(location, x, y, z);
}

//重载
void Shader::setVector3(const std::string& name, const float* values) {
	//1 通过名称拿到Uniform变量的位置Location
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	//2 通过Location更新Uniform变量的值
	//第二个参数:当前要更新的uniform变量如果是数组,数组里面包括了多少个向量vec3
	glUniform3fv(location, 1, values);
}


void Shader::setInt(const std::string& name, int value) {
	//1 通过名称拿到Uniform变量的位置Location
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	//2 通过Location更新Uniform变量的值
	//第二个参数:当前要更新的uniform变量如果是数组,数组里面包括了多少个向量vec3
	glUniform1i(location, value);

}


void Shader::setMatrix4x4(const std::string& name, glm::mat4 value) {
	//1 通过名称拿到Uniform变量的位置Location
	GLuint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	//2 通过Location更新Uniform变量的值
	//transpose参数:是否对传输进去的矩阵数据进行转置
	//在opengl中使用的是列存储,GLM中也是列存储,所以这里不需要转置
	//value_ptr:可以获得矩阵对象对应的数据的指针
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

}

void Shader::checkShaderErrors(GLuint target, std::string type) {
	int success = 0;
	char infoLog[1024];
	if (type == "COMPILE")
	{
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error:SHADER COMPILE ERROR" << "\n" << infoLog << std::endl;
		}
	}
	else if (type == "LINK")
	{
		glGetProgramiv(target, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error:SHADER LINK ERROR" << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		std::cout << "Error: Check shader errors Type is wrong" << std::endl;
	}
}

