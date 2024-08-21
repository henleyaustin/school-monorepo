/*
 * Shader source code for project
 */

#ifndef SHADER_SOURCE_H
#define SHADER_SOURCE_H

// Macro for defining shaders
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

/* Vertex Shader Source Code
 *
 * 6/2 : Added vertex texture coordinate controls
 *
 */
const GLchar* vertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexFragmentNormal;
out vec3 vertexFragmentPos;
out vec2 vertexTextureCoordinate;
out vec3 eyeLightPos1;  // New
out vec3 eyeLightPos2;  // New

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 light1Position;  // New
uniform vec3 light2Position;  // New

void main()
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0f);

    vertexFragmentPos = vec3(model * vec4(vertexPosition, 1.0f));

    vertexFragmentNormal = mat3(transpose(inverse(model))) * vertexNormal;
    vertexTextureCoordinate = textureCoordinate;

    eyeLightPos1 = vec3(view * vec4(light1Position, 1.0f));  // New
    eyeLightPos2 = vec3(view * vec4(light2Position, 1.0f));  // New
}
);


/* Fragment Shader Source Code
 *
 * 6/2 : Added texture abilities
 *      ~ allows usage of multiple textures and single textures
 *        Also kept in color capabilities for simple objects
 *
 * 6/10 : Added lighting abilities
 *
 */
const GLchar* fragmentShaderSource = GLSL(440,

    in vec3 vertexFragmentNormal;
in vec3 vertexFragmentPos;
in vec2 vertexTextureCoordinate;
in vec3 eyeLightPos1;
in vec3 eyeLightPos2;

out vec4 fragmentColor;

uniform sampler2D uTextureBase;
uniform sampler2D uTextureExtra;
uniform bool multipleTextures;
uniform bool singleTexture;
uniform vec4 objectColor;
uniform vec3 ambientColor;
uniform vec3 light1Color;
uniform vec3 light1Position;
uniform vec3 light2Color;
uniform vec3 light2Position;
uniform vec3 light3Color;
uniform vec3 light3Direction;
uniform vec3 viewPosition;
uniform vec2 uvScale;
uniform float ambientStrength;
uniform float specularIntensity;
uniform float highlightSize = 16.0f;
uniform float light1Intensity = 1.0f;
uniform float light2Intensity = 1.0f;
uniform float light3Intensity = 0.6f;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting
    vec3 ambient = ambientStrength * ambientColor; // Generate ambient light color

    //**Calculate Diffuse lighting**
    vec3 norm = normalize(vertexFragmentNormal); // Normalize vectors to 1 unit

    vec3 light1Direction = normalize(eyeLightPos1 - vertexFragmentPos); // Adjusted
    float impact1 = max(dot(norm, light1Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse1 = impact1 * light1Color * light1Intensity; // Generate diffuse light color

    vec3 light2Direction = normalize(eyeLightPos2 - vertexFragmentPos); // Adjusted
    float impact2 = max(dot(norm, light2Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse2 = impact2 * light2Color * light2Intensity;

    //**Calculate Specular lighting**
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir1 = reflect(-light1Direction, norm);// Calculate reflection vector

    //Calculate specular component
    float specularComponent1 = pow(max(dot(viewDir, reflectDir1), 0.4), highlightSize);
    vec3 specular1 = specularIntensity * specularComponent1 * light1Color * light1Intensity;
    vec3 reflectDir2 = reflect(-light2Direction, norm);// Calculate reflection vector

    //Calculate specular component
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.1), highlightSize);
    vec3 specular2 = specularIntensity * specularComponent2 * light2Color * light2Intensity;

    vec3 light3DirectionN = normalize(light3Direction);
    float impact3 = max(dot(norm, light3DirectionN), 0.0);
    vec3 diffuse3 = impact3 * light3Color * light3Intensity;
    vec3 reflectDir3 = reflect(-light3DirectionN, norm);
    float specularComponent3 = pow(max(dot(viewDir, reflectDir3), 0.1), highlightSize);
    vec3 specular3 = specularIntensity * specularComponent3 * light3Color * light3Intensity;

    //**Calculate phong result**
    //Texture holds the color to be used for all three components
    vec4 usedColor = vec4(0.0, 0.0, 0.0, 0.0);

    if (multipleTextures)
    {
        vec4 extraTexture = texture(uTextureExtra, vertexTextureCoordinate);
        if (extraTexture.a != 0.0)
            usedColor = extraTexture;
        else
            usedColor = texture(uTextureBase, vertexTextureCoordinate);
    }
    else if (singleTexture)
    {
        usedColor = texture(uTextureBase, vertexTextureCoordinate);
    }
    else
    {
        usedColor = objectColor;
    }

    vec3 phong1 = (ambient + diffuse1 + specular1) * usedColor.rgb;
    vec3 phong2 = (ambient + diffuse2 + specular2) * usedColor.rgb;
    vec3 phong3 = (ambient + diffuse3 + specular3) * usedColor.rgb;


    fragmentColor = vec4(phong1 + phong2 + phong3, usedColor.a);
}
);
/* light Object Shader Source Code*/
const GLchar* lightVertexShaderSource = GLSL(330,
    layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
);

/* Lamp Fragment Shader Source Code*/
const GLchar* lightFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor;

uniform vec4 lampColor;

void main()
{
    fragmentColor = lampColor;
}
);

#endif // SHADER_SOURCE_H
