#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D blockTexture;

void main() {
    // Simple diffuse lighting calculation
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 normal = normalize(Normal);
    
    // Calculate diffuse lighting
    float diff = max(dot(normal, lightDir), 0.3); // 0.3 minimum for ambient light
    
    // Sample texture
    vec4 texColor = texture(blockTexture, TexCoord);
    
    // Apply lighting to texture color
    FragColor = vec4(diff * texColor.rgb, texColor.a);
}