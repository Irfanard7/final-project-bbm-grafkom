#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

uniform vec3 lightPos2; 
uniform vec3 viewPos2; 
uniform vec3 lightColor2;

uniform vec3 lightPos3; 
uniform vec3 viewPos3; 
uniform vec3 lightColor3;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 lightDir2 = normalize(lightPos2 - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb * objectColor;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.5);
    // ambient
    vec3 ambient = 0.5 * color;
    vec3 ambient2 = 0.3 * color;
    vec3 ambient3 = 0.3 * color;

    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor * 0.5;

    vec3 lightDir2 = normalize(lightPos2 - fs_in.FragPos);
    float diff2 = max(dot(lightDir2, normal), 0.0);
    vec3 diffuse2 = diff2 * lightColor2;

    vec3 lightDir3 = normalize(lightPos3 - fs_in.FragPos);
    float diff3 = max(dot(lightDir3, normal), 0.0);
    vec3 diffuse3 = diff3 * lightColor3;
    
	// specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = spec * lightColor; 

    vec3 viewDir2 = normalize(viewPos2 - fs_in.FragPos);
    vec3 reflectDir2 = reflect(-lightDir2, normal);
    float spec2 = 0.0;
    spec2 = pow(max(dot(viewDir2, reflectDir2), 0.0), 64.0);
    vec3 specular2 = spec2 * lightColor2; 

    vec3 viewDir3 = normalize(viewPos3 - fs_in.FragPos);
    vec3 reflectDir3 = reflect(-lightDir3, normal);
    float spec3 = 0.0;
    spec3 = pow(max(dot(viewDir3, reflectDir3), 0.0), 64.0);
    vec3 specular3 = spec3 * lightColor3; 

    //senter
    float theta = dot(lightDir2, normalize(-viewPos2));
    float epsilon = 0.00001;
    float intensity = clamp((theta - 0.6) / epsilon, 0.1, 1);

    diffuse2 *= intensity;
    specular2 *= intensity;
    diffuse3 *= intensity;
    specular3 *= intensity;


    float distance    = length(lightPos2 - fs_in.FragPos);
    float attenuation = 1.0f / (0.1 + 1 * distance + 1 * (distance * distance));
    ambient2  *= attenuation;
    diffuse2  *= attenuation;
    specular2 *= attenuation;
    ambient3  *= attenuation;
    diffuse3  *= attenuation;
    specular3 *= attenuation;
    
	// calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    
	vec3 lighting = (ambient3 +  (1.0 - shadow) * (diffuse3 + specular3) + ambient2 +  (1.0 - shadow) * (diffuse2 + specular2) + ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}