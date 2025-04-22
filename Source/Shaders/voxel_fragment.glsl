#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

// Texture samplers for each face of the cube
uniform sampler2D textureFace0; // TOP
uniform sampler2D textureFace1; // BOTTOM
uniform sampler2D textureFace2; // FRONT
uniform sampler2D textureFace3; // BACK
uniform sampler2D textureFace4; // LEFT
uniform sampler2D textureFace5; // RIGHT

// Shadow mapping
uniform sampler2D shadowMap;

// Lighting uniforms
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;

float ShadowCalculation(vec4 fragPosLightSpace) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Check if fragment is beyond shadow map bounds
    if(projCoords.x < 0.0 || projCoords.x > 1.0 || 
       projCoords.y < 0.0 || projCoords.y > 1.0 ||
       projCoords.z < 0.0 || projCoords.z > 1.0) {
        return 0.0; // No shadow outside bounds
    }
    
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // Get current depth
    float currentDepth = projCoords.z;
    
    // Calculate bias based on surface angle relative to light
    float bias = max(0.025 * (1.0 - dot(Normal, -normalize(lightDir))), 0.0025);
    
    // PCF (Percentage Closer Filtering) for softer shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int pcfRadius = 2;
    
    for(int x = -pcfRadius; x <= pcfRadius; ++x) {
        for(int y = -pcfRadius; y <= pcfRadius; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    
    shadow /= ((2 * pcfRadius + 1) * (2 * pcfRadius + 1));
    
    // Keep the shadow at 0.0 when outside the light's far plane
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {
    // Determine which face we're rendering based on the normal vector
    vec3 absNormal = abs(normalize(Normal));
    vec4 texColor;
    
    // Choose the appropriate texture based on the dominant normal component
    if(absNormal.y > absNormal.x && absNormal.y > absNormal.z) {
        // Top or bottom face
        if(Normal.y > 0.0) {
            texColor = texture(textureFace0, TexCoord); // TOP
        } else {
            texColor = texture(textureFace1, TexCoord); // BOTTOM
        }
    } else if(absNormal.z > absNormal.x && absNormal.z > absNormal.y) {
        // Front or back face
        if(Normal.z > 0.0) {
            texColor = texture(textureFace2, TexCoord); // FRONT
        } else {
            texColor = texture(textureFace3, TexCoord); // BACK
        }
    } else {
        // Left or right face
        if(Normal.x < 0.0) {
            texColor = texture(textureFace4, TexCoord); // LEFT
        } else {
            texColor = texture(textureFace5, TexCoord); // RIGHT
        }
    }
    
    // Discard transparent pixels (if using transparency)
    if(texColor.a < 0.1)
        discard;
    
    // Calculate lighting
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular lighting
    float specularStrength = 0.3;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);
    
    // Combine lighting components with shadow
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor.rgb;
    
    // Output final color
    FragColor = vec4(result, texColor.a);
}
