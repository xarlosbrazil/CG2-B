#include "System.h"
#include "loadObj.h"
#include "Camera.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f)); // Start the camera at (0,0,10) looking towards the origin

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cooldown = 0.8f;

System::System()
{
}

System::~System()
{
}

int System::GLFWInit()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Editor de Pistas GB", nullptr, nullptr);

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	glfwSetWindowUserPointer(window, this); // Seta o ponteiro para a instância atual de System

	glfwSetMouseButtonCallback(window, MouseCallback); // Configura o callback de mouse

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed no init GLEW." << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	return EXIT_SUCCESS;

}

int System::OpenGLSetup()
{

	glEnable(GL_BLEND);	// Enables blending ( glBlendFunc )
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	return EXIT_SUCCESS;
}

int System::SystemSetup()
{

	// coreShader = Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	lineShader = Shader("Shaders/Line/line.vert", "Shaders/Line/line.frag");

	lineShader.Use();

	coreShader.LoadTexture((char*)"Textures/wood.jpg", (char*)"texture1", "woodTexture");
	coreShader.UseTexture("woodTexture");

	setupCurva();

	return EXIT_SUCCESS;
}

void processInput(GLFWwindow* window, float deltaTime)

{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

void System::Run()

{
	// Matriz MVP -- Model, View, Projection
	GLuint viewLoc = glGetUniformLocation(lineShader.ID, "view");
	GLuint projLoc = glGetUniformLocation(lineShader.ID, "projection");
	GLuint modelLoc = glGetUniformLocation(lineShader.ID, "model");
	GLuint colorLoc = glGetUniformLocation(lineShader.ID, "lineColor");

	while (!glfwWindowShouldClose(window)) {

		// Check and call events
		// float currentFrame = glfwGetTime();
		// deltaTime = currentFrame - lastFrame;
		// lastFrame = currentFrame;

		glfwPollEvents();
		processInput(window, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{

			salvarOBJ("pista.obj");
			salvarAnimacao("rota.txt");
			salvarCena("cena.txt");
		}

		// Background Fill Color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		//coreShader.Use();
		lineShader.Use();

		//glUniform3f(glGetUniformLocation(lineShader.ID, "lightPos"), 0.0f, 5.0f, 0.0f);
		//glUniform3f(glGetUniformLocation(lineShader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);

		//glUniform3f(glGetUniformLocation(lineShader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		//glUniform3f(glGetUniformLocation(lineShader.ID, "objectColor"), 1.0f, 0.5f, 0.31f);

		//coreShader.UseTexture("woodTexture");

		// Onde o objeto está no mundo? (Matriz Model)
		// glm::mat4 view = camera.GetViewMatrix(); // De onde estamos olhando para o mundo?
		// glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f); // Como se fosse a "lente" da câmera

		glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::mat4(1.0f);  // Câmera parada
		glm::mat4 model = glm::mat4(1.0f); // Objeto parado

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Cor Amarela
		if (colorLoc != -1) glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f);

		if (!pontosDaCurva.empty())
		{
			glBindVertexArray(this->curvaVAO);
			glDrawArrays(GL_LINE_STRIP, 0, this->pontosDaCurva.size());
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window); // Shows what was drawn in this frame
	}

	// Onde o objeto está no mundo? (Matriz Model)
	// De onde estamos olhando para este mundo ? (Matriz View)
	// Qual a perspectiva da nossa "lente" ? (A visão é "olho de peixe" ou "zoom" ? ) (Matriz Projection)
}

void System::Finish()
{
	lineShader.Delete();

	for (Mesh* mesh : this->meshes) // Limpa a memória dos meshes criados
	{
		delete mesh;
	}
	this->meshes.clear();

	glfwTerminate();
}

void System::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)

	{

		System* sistema = (System*)glfwGetWindowUserPointer(window); // 1. Pegar o ponteiro para a instância atual de System

		double x, y;
		glfwGetCursorPos(window, &x, &y); // 2. Pegar a posição do clique

		sistema->adicionarPontoDeControle(x, y);
	}
}

void System::adicionarPontoDeControle(double xpos, double ypos)
{
	// Por enquanto, vamos apenas guardar o dado bruto (pixel da tela)
	// O Z fica em 0.0f pois estamos clicando numa tela 2D.
	this->pontosDeControle.push_back(glm::vec3(xpos, ypos, 0.0f));

	// Print para confirmarmos que funcionou
	std::cout << "Clique capturado! X: " << xpos << " | Y: " << ypos << std::endl;
	std::cout << "Total de pontos: " << this->pontosDeControle.size() << std::endl;

	if (this->pontosDeControle.size() >= 4)
	{
		calcularBSpline();
		gerarTriangulosPista();
		gerarMalhaPista();

		updateCurveBuffers();
		std::cout << ">>> Curva Recalculada! Total de Pontos Suaves: "
			<< this->pontosDaCurva.size() << std::endl;
	}
}

void System::calcularBSpline()
{

	this->pontosDaCurva.clear();


	if (this->pontosDeControle.size() < 4) return;

	for (size_t i = 0; i < this->pontosDeControle.size() - 3; ++i)
	{
		// Janela de 4 pontos de controle
		glm::vec3 p0 = pontosDeControle[i]; // Ponto i
		glm::vec3 p1 = pontosDeControle[i + 1]; // Ponto i+1
		glm::vec3 p2 = pontosDeControle[i + 2]; // Ponto i+2
		glm::vec3 p3 = pontosDeControle[i + 3]; // Ponto i+3

		for (float t = 0.0f; t < 1.0f; t += 0.01f) // Cálculo da curva suave entre os 4 pontos
		{
			float t2 = t * t;  // t^2
			float t3 = t * t * t; // t^3


			float b0 = (-t3 + 3 * t2 - 3 * t + 1.0f) / 6.0f; // Influência do Ponto 0 (Cai no fim): -t^3 + 3t^2 - 3t + 1

			float b1 = (3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f; // Influência do Ponto 1 (O presente): 3t^3 - 6t^2 + 4

			float b2 = (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f; // Influência do Ponto 2 (O passado): -3t^3 + 3t^2 + 3t + 1

			float b3 = t3 / 6.0f; // Influência do Ponto 3 (Cai no começo): t^3

			float x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
			float y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
			float z = b0 * p0.z + b1 * p1.z + b2 * p2.z + b3 * p3.z;

			this->pontosDaCurva.push_back(glm::vec3(x, y, z));
		}
	}
}

void System::setupCurva()
{
	// Criação dos buffers OpenGL para a curva B-Spline

	glGenVertexArrays(1, &this->curvaVAO);
	glGenBuffers(1, &this->curvaVBO);
	glBindVertexArray(this->curvaVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->curvaVBO);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void System::updateCurveBuffers()
{

	if (this->pontosDaCurva.empty()) return; // Nada a atualizar

	// Atualiza os dados do VBO com os novos pontos da curva
	glBindBuffer(GL_ARRAY_BUFFER, this->curvaVBO);
	glBufferData(GL_ARRAY_BUFFER, this->pontosDaCurva.size() * sizeof(glm::vec3), this->pontosDaCurva.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void System::gerarMalhaPista()
{
	pistaInterna.clear();
	pistaExterna.clear();

	if (pontosDaCurva.size() < 2) return;

	float larguraPista = 10.0f;
	float m = larguraPista / 2.0f; // Metade para cada lado (o 'm' da Figura 2)

	// Percorre todos os pontos da linha central
	for (size_t i = 0; i < pontosDaCurva.size(); ++i) // O que é o size_t? É um tipo de dado sem sinal usado para representar tamanhos e índices.
	{
		glm::vec3 A = pontosDaCurva[i];
		glm::vec3 B;


		// Direção do vetor (Ponto seguinte)
		if (i < pontosDaCurva.size() - 1)
		{
			B = pontosDaCurva[i + 1];
		}
		else
		{
			// No último ponto, usamos a direção do ponto anterior.
			B = pontosDaCurva[0];
		}

		// Delta -- Para onde apontar?
		float w = B.x - A.x; // Quanto ando para o lado (x)
		float h = B.y - A.y; // Quanto ando para cima (y)
		float theta;

		theta = std::atan(h / w); // Qual o ângulo exato da direção no mundo?

		float alpha;

		if (w < 0) {
			// Indo para a esquerda
			alpha = theta - glm::pi<float>()/2;
		}
		else {
			// Indo para a direita
			alpha = theta + glm::pi<float>()/2;
		}

		// Borda interna
		float Intx = A.x + m * std::cos(alpha);
		float Inty = A.y + m * std::sin(alpha);

		// Rotaciona 180 graus para a borda externa
		float Extx = A.x + m * std::cos(alpha + glm::pi<float>()); // Onde estou + o quanto ando na direção X -- É cosseno porque x é o "adjacente"
		float Exty = A.y + m * std::sin(alpha + glm::pi<float>()); // Onde estou + o quanto ando na direção Y -- É seno porque y é o "oposto"

		// Guardar nos vetores (Z=0 pois é 2D)
		pistaInterna.push_back(glm::vec3(Intx, Inty, 0.0f));
		pistaExterna.push_back(glm::vec3(Extx, Exty, 0.0f));
	}
}

void System::gerarTriangulosPista()
{
	malhaPistaVertices.clear();

	if (pistaInterna.size() < 2 || pistaExterna.size() < 2) return; // Minimo de 2 pontos em cada lado

	// Loop até o penúltimo ponto
	for (size_t i = 0; i < pistaInterna.size() - 1; ++i)
	{
		// Construção de quadrado em quadrado (Duas faces triangulares por vez)
		glm::vec3 E_atual = pistaInterna[i];
		glm::vec3 E_prox = pistaInterna[i + 1];

		glm::vec3 D_atual = pistaExterna[i];
		glm::vec3 D_prox = pistaExterna[i + 1];

		// Como as informações de um triângulo é usado no outro, vamos criar dois triângulos por vez:

		// --- TRIÂNGULO 1 (E_atual -> D_atual -> E_prox) ---
		malhaPistaVertices.push_back(E_atual.x);
		malhaPistaVertices.push_back(E_atual.y);
		malhaPistaVertices.push_back(E_atual.z);
		malhaPistaVertices.push_back(D_atual.x);
		malhaPistaVertices.push_back(D_atual.y);
		malhaPistaVertices.push_back(D_atual.z);
		malhaPistaVertices.push_back(E_prox.x);
		malhaPistaVertices.push_back(E_prox.y);
		malhaPistaVertices.push_back(E_prox.z);

		// --- TRIÂNGULO 2 (D_atual -> D_prox -> E_prox) ---
		malhaPistaVertices.push_back(D_atual.x);
		malhaPistaVertices.push_back(D_atual.y);
		malhaPistaVertices.push_back(D_atual.z);
		malhaPistaVertices.push_back(D_prox.x);
		malhaPistaVertices.push_back(D_prox.y);
		malhaPistaVertices.push_back(D_prox.z);
		malhaPistaVertices.push_back(E_prox.x);
		malhaPistaVertices.push_back(E_prox.y);
		malhaPistaVertices.push_back(E_prox.z);
	}
}

void System::salvarOBJ(const char* filename)
{

	std::ofstream arquivo(filename);

	// 1. HEADER (Comentários)
	arquivo << "# Editor de Pistas Grau B\n";
	arquivo << "# Pista\n";

	// 2. VÉRTICES

	// Pontos internos (Índices 1 a N)
	for (const auto& p : pistaInterna) {
		arquivo << "v " << p.x << " " << 0.0f << " " << p.y << "\n";
	}

	// Pontos externos (Índices N+1 a 2N)
	for (const auto& p : pistaExterna) {
		arquivo << "v " << p.x << " " << 0.0f << " " << p.y << "\n";
	}

	// 3. COORDENADAS DE TEXTURA (vt)
	arquivo << "vt 0.0 0.0\n"; // Índice 1
	arquivo << "vt 1.0 0.0\n"; // Índice 2
	arquivo << "vt 1.0 1.0\n"; // Índice 3
	arquivo << "vt 0.0 1.0\n"; // Índice 4

	// 4. NORMAIS (vn)

	int totalVertices = pistaInterna.size() + pistaExterna.size();
	for (int i = 0; i < totalVertices; ++i) {
		arquivo << "vn 0.0 1.0 0.0\n"; // Normal sempre fica para cima em todas as faces
	}

	// 5. FACES (f v/vt/vn)

	int N = pistaInterna.size(); // Número de pontos totais pela metade (internos ou externos)

	for (int i = 0; i < N - 1; ++i)
	{

		// Índices dos vértices
		int i_atual = i + 1; // Início dos internos
		int i_prox = i + 2;

		int e_atual = N + i + 1; // Começa na pista externa (Total de internos + 1)
		int e_prox = N + i + 2;

		// Triângulo 1: InternoAtual -> ExternoAtual -> InternoProx
		// Mapeamento VT: (0,0) -> (1,0) -> (0,1) = vt 1 -> 2 -> 4
		// Normal: sempre 1
		arquivo << "f " << i_atual << "/1/1 " << e_atual << "/2/1 " << i_prox << "/4/1\n";

		// Triângulo 2: ExternoAtual -> ExternoProx -> InternoProx
		// Mapeamento VT: (1,0) -> (1,1) -> (0,1) = vt 2 -> 3 -> 4
		arquivo << "f " << e_atual << "/2/1 " << e_prox << "/3/1 " << i_prox << "/4/1\n";
	}

	int i_ultimo = N;          // Último vértice interno
	int e_ultimo = 2 * N;      // Último vértice externo

	// Índices do Primeiro ponto da lista
	int i_primeiro = 1;        // Primeiro vértice interno
	int e_primeiro = N + 1;    // Primeiro vértice externo

	// Face 1: Último Interno -> Último Externo -> Primeiro Interno
	arquivo << "f " << i_ultimo << "/1/" << i_ultimo << " "
		<< e_ultimo << "/2/" << e_ultimo << " "
		<< i_primeiro << "/4/" << i_primeiro << "\n";

	// Face 2: Último Externo -> Primeiro Externo -> Primeiro Interno
	arquivo << "f " << e_ultimo << "/2/" << e_ultimo << " "
		<< e_primeiro << "/3/" << e_primeiro << " "
		<< i_primeiro << "/4/" << i_primeiro << "\n";

	arquivo.close();
	std::cout << "Sucesso! OBJ salvo." << std::endl;
}

void System::salvarAnimacao(const char* filename) // Salvar a B-Spline como caminho do carro
{

	std::ofstream arquivo(filename);

	for (const auto& ponto : pontosDaCurva) {

		arquivo << ponto.x << " " << 0.0f << " " << ponto.y << "\n"; // Z=0 para manter carro na pista 2D
	}
	arquivo.close();
}

void System::salvarCena(const char* filename)
{
	std::ofstream arquivo(filename);
	if (!arquivo.is_open()) return;

	// Formato sugerido:
	// Linha 1: Arquivo da Pista (OBJ)
	// Linha 2: Arquivo da Curva (Pontos para animação)
	// Linha 3: Arquivo do Carro (OBJ) - Esse vamos arranjar depois
	// Linha 4: Textura da Pista

	arquivo << "pista.obj\n";
	arquivo << "rota.txt\n";
	arquivo << "carro.obj\n";     // Nome fictício por enquanto
	arquivo << "Textures/road.jpg\n"; // Textura da estrada

	arquivo.close();
	std::cout << "Arquivo de Cena salvo!" << std::endl;
}