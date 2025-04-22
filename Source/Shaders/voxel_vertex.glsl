#version 330 core
layout(location = 0) in vec3 aPos;        // Vertex position
layout(location = 1) in vec3 aNormal;     // Vertex normal
layout(location = 2) in vec2 aTexCoord;   // Texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;  // For shadow mapping

out vec2 TexCoord;
out vec3 FragPos;     
out vec3 Normal;      
out vec4 FragPosLightSpace;

void main() {
    // Calculate position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Calculate normal in world space (for lighting)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Calculate position in light space (for shadow mapping)
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    
    // Calculate final position in clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
    
    // Pass texture coordinates to fragment shader directly
    TexCoord = aTexCoord;
}
