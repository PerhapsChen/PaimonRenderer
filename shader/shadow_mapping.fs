#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform sampler2D texture_base_diffuse;

uniform vec3 ambient_color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool hasDiffuse;

#define SHADOW_MAP_SIZE 8192.
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define NUM_SAMPLES 50
#define NUM_RINGS 10
#define FILTER_SCALE 15

float rand_1to1(float x) //- 由种子生成[-1,1]的随机数
{
    return fract(sin(x)*10000.0);
}
float rand_2to1(vec2 uv) //- 由两个种子组成生成[0,1]的随机数
{
    float a = 12.9898, b= 78.233, c = 43758.5453;
    float dt = dot(uv.xy, vec2(a,b));
    float sn = mod(dt, PI);
    return fract(sin(sn) * c);
}
vec2 poissonDisk[NUM_SAMPLES];
void poissonDiskSample(vec2 randomSeed)
{
    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );
    float angle = rand_2to1( randomSeed ) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;    
    for( int i = 0; i < NUM_SAMPLES; i ++ ) 
    {
        poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

float findBlocker(vec2 uv, float currentDepth)
{
    poissonDiskSample(uv);
    float totalDepth = 0.0;
    int blockerCount = 0;
    for(int i=0; i<NUM_SAMPLES; i++)
    {
        vec2 simpleUV = uv+poissonDisk[i]/float(SHADOW_MAP_SIZE) * 50;
        float closestDepth = texture(shadowMap, simpleUV).r;
        if(currentDepth > closestDepth + 0.03){
            totalDepth += closestDepth;
            blockerCount++;
        }
    }
    if(blockerCount==0)
        return -1.0;
    if(blockerCount==NUM_SAMPLES)
        return 2.0;
    return totalDepth/float(blockerCount);
}

float Bias()
{
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.007*(1.0 - dot(normal, lightDir)), 0.007);
    return bias;
}

float useShadowMap(vec4 projCoords, float biasC, float filterSize)
{
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = Bias();
    if(currentDepth-bias >= closestDepth)
        return 0.0;
    else return 1.0; 
}

float PCF(vec4 projCoords)
{
    float bias = Bias();
    float visibility = 0.0;
    float currentDepth = projCoords.z;
    float filterSize = 1.0/SHADOW_MAP_SIZE * FILTER_SCALE;
    poissonDiskSample(projCoords.xy);
    
    for(int i=0; i<NUM_SAMPLES; i++){
        vec2 texCoords = poissonDisk[i] * filterSize + projCoords.xy;
        float closestDepth = texture(shadowMap,texCoords.xy).r;
        visibility += (closestDepth < currentDepth-bias)?0.0:1.0;
    }
    return visibility/float(NUM_SAMPLES);
}

float PCSS(vec4 projCoords)
{
    float bias = Bias();
    float visibility = 0.0;
    float currentDepth = projCoords.z;
    float blockerDepth = findBlocker(projCoords.xy, currentDepth);
    
    float penumbra = (currentDepth - blockerDepth) / blockerDepth * FILTER_SCALE;
    float filterSize = 1.0/SHADOW_MAP_SIZE * penumbra;

    for(int i=0; i<NUM_SAMPLES; i++){
        vec2 texCoords = poissonDisk[i] * filterSize + projCoords.xy;
        float closestDepth = texture(shadowMap,texCoords.xy).r;
        visibility += (closestDepth < currentDepth-bias)?0.0:1.0;
    }

    return visibility/float(NUM_SAMPLES);
}


void main()
{           
    vec3 color = texture(texture_base_diffuse, fs_in.TexCoords).rgb;
    color = pow(color, vec3(2.2));
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.9);
    // ambient
    vec3 ambient = 0.6 * ambient_color;//lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir); 

    //--- 卡通渲染---- 高光
    spec = dot(normal, halfwayDir);
    float w = fwidth(spec) * 2.0; 
    float scale = 0.05;
    spec = mix(0,1,smoothstep(-w,w,spec+scale-1));// * step(0.000001, scale);
    //spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;  
    
    // calculate shadow
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float visibility = PCSS(vec4(projCoords,1.0));                      
    vec3 lighting = (ambient + visibility * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}