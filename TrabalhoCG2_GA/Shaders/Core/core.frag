#version 330 core
out vec4 FragColor;

// Entradas que vêm do Vertex Shader
in vec3 FragPos;  // A posição do pixel no mundo 3D
in vec3 Normal;   // A direção que a superfície aponta
in vec2 TexCoord; // Coordenada da textura

// Uniforms (Variáveis que vêm do C++)
uniform sampler2D texture1; // A imagem da textura (madeira)
uniform vec3 lightPos;      // Onde está a luz?
uniform vec3 viewPos;       // Onde está a câmera/olho?
uniform vec3 lightColor;    // Qual a cor da luz? (Geralmente branco 1,1,1)
uniform vec3 objectColor;   // Cor base do objeto (se não tiver textura)

void main()
{
    // --- 1. LUZ AMBIENTE ---
    // Uma luz fraquinha que ilumina tudo por igual
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  
    // --- 2. LUZ DIFUSA (A mais importante) ---
    vec3 norm = normalize(Normal); // Garante que a normal tenha tamanho 1
    vec3 lightDir = normalize(lightPos - FragPos); // Direção da luz até o objeto
    
    // Calcula o ângulo entre a normal e a luz (dot product)
    // Se for negativo (luz vindo de trás), usamos 0.0
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // --- 3. LUZ ESPECULAR (O brilho) ---
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos); // Direção do objeto até a câmera
    // Calcula a direção do reflexo da luz na superfície
    vec3 reflectDir = reflect(-lightDir, norm);  
    
    // Calcula o brilho baseado em o quanto a câmera está alinhada com o reflexo
    // O número 32 é a "shininess" (quanto maior, mais focado é o brilho)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    // --- RESULTADO FINAL ---
    // Soma as três luzes e multiplica pela cor da textura (ou do objeto)
    vec3 result = (ambient + diffuse + specular) * vec3(texture(texture1, TexCoord));
    
    FragColor = vec4(result, 1.0);
}