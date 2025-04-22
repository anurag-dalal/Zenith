#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Zenith {

class Shader {
public:
    Shader(const std::string& filepath);
    ~Shader();

    void bind() const;
    void unbind() const;

    // Set uniforms
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat4(const std::string& name, const glm::mat4& value);

    unsigned int getID() const { return m_RendererID; }

private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    int getUniformLocation(const std::string& name) const;
    
    struct ShaderProgramSource {
        std::string VertexSource;
        std::string FragmentSource;
    };
    
    ShaderProgramSource parseShader(const std::string& filepath);
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
};

} // namespace Zenith