#include"shader.h"
#include"../wrapper/checkError.h"

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	//����װ��shader�����ַ���������string
	std::string vertexCode;
	std::string fragmentCode;

	try {
		vertexCode = loadShader(vertexPath);
		fragmentCode = loadShader(fragmentPath);
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR: Shader File Error: " << e.what() << std::endl;
	}

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();
	//1 ����Shader����vs��fs��
	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	//2 Ϊshader��������shader����
	glShaderSource(vertex, 1, &vertexShaderSource, NULL);
	glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

	//3 ִ��shader������� 
	glCompileShader(vertex);
	//���vertex������
	checkShaderErrors(vertex, "COMPILE");
	
	glCompileShader(fragment);
	//���fragment������
	checkShaderErrors(fragment, "COMPILE");
	
	//4 ����һ��Program����
	mProgram = glCreateProgram();

	//6 ��vs��fs����õĽ���ŵ�program���������
	glAttachShader(mProgram, vertex);
	glAttachShader(mProgram, fragment);

	//7 ִ��program�����Ӳ������γ����տ�ִ��shader����
	glLinkProgram(mProgram);

	//������Ӵ���
	checkShaderErrors(mProgram, "LINK");

	//����
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
Shader::~Shader() {

}

std::string Shader::loadShader(const std::string& filePath) {
	std::ifstream file(filePath);
	std::stringstream shaderStream;
	std::string line;

	while (std::getline(file, line)) {
		//�ж��Ƿ���#include
		if (line.find("#include") != std::string::npos) {
			//�ҵ�include�������ļ�·��
			auto start = line.find("\"");
			auto end = line.find_last_of("\"");
			std::string includeFile = line.substr(start + 1, end - start - 1);

			//�ҵ���ǰ�ļ����ļ�Ŀ¼
			auto lastSlashPos = filePath.find_last_of("/\\");
			auto folder = filePath.substr(0, lastSlashPos + 1);
			auto totalPath = folder + includeFile;
			shaderStream << loadShader(totalPath);
		}
		else {
			shaderStream << line << "\n";
		}
	}

	return shaderStream.str();
}

void Shader::begin() {
	GL_CALL(glUseProgram(mProgram));
}

void Shader::end() {
	GL_CALL(glUseProgram(0));
}

void Shader::setFloat(const std::string& name, float value) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	//2 ͨ��Location����Uniform������ֵ
	GL_CALL(glUniform1f(location, value));
}

void Shader::setVector3(const std::string& name, float x, float y, float z) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	
	//2 ͨ��Location����Uniform������ֵ
	GL_CALL(glUniform3f(location, x, y, z));
}

//���� overload
void Shader::setVector3(const std::string& name, const float* values) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	//2 ͨ��Location����Uniform������ֵ
	//�ڶ����������㵱ǰҪ���µ�uniform������������飬��������������ٸ�����vec3
	GL_CALL(glUniform3fv(location, 1, values));
}

void Shader::setVector3(const std::string& name, const glm::vec3 value) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	//2 ͨ��Location����Uniform������ֵ
	GL_CALL(glUniform3f(location, value.x, value.y, value.z));
}

void Shader::setInt(const std::string& name, int value) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	//2 ͨ��Location����Uniform������ֵ
	glUniform1i(location, value);
}

void Shader::setMatrix4x4(const std::string& name, glm::mat4 value) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));
	
	//2 ͨ��Location����Uniform������ֵ
	//transpose��������ʾ�Ƿ�Դ����ȥ�ľ������ݽ���ת��
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMatrix4x4Array(const std::string& name, glm::mat4* value, int count) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	//2 ͨ��Location����Uniform������ֵ
	glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value[0]));
}

void Shader::setMatrix3x3(const std::string& name, glm::mat3 value) {
	//1 ͨ�������õ�Uniform������λ��Location
	GLint location = GL_CALL(glGetUniformLocation(mProgram, name.c_str()));

	//2 ͨ��Location����Uniform������ֵ
	//transpose��������ʾ�Ƿ�Դ����ȥ�ľ������ݽ���ת��
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}




void Shader::checkShaderErrors(GLuint target, std::string type) {
	int success = 0;
	char infoLog[1024];

	if (type == "COMPILE") {
		glGetShaderiv(target, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error: SHADER COMPILE ERROR" << "\n" << infoLog << std::endl;
		}
	}
	else if (type == "LINK") {
		glGetProgramiv(target, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(target, 1024, NULL, infoLog);
			std::cout << "Error: SHADER LINK ERROR " << "\n" << infoLog << std::endl;
		}
	}
	else {
		std::cout << "Error: Check shader errors Type is wrong" << std::endl;
	}
}