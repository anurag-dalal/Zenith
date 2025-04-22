#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace Zenith {

Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = parseShader(filepath);
    m_RendererID = createShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

Shader::ShaderProgramSource Shader::parseShader(const std::string& filepath) {
    std::ifstream stream(filepath);
    
    if (!stream.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return { "", "" };
    }
    
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else if (type != ShaderType::NONE) {
            ss[(int)type] << line << '\n';
        }
    }
    
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " 
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    
    return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cerr << "Failed to link shader program!" << std::endl;
        std::cerr << message << std::endl;
    }
    
    glValidateProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}

void Shader::bind() const {
    glUseProgram(m_RendererID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

int Shader::getUniformLocation(const std::string& name) const {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cerr << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    
    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

} // namespace Zenith