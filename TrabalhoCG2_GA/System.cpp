#include "System.h"
#include "loadObj.h"
#include "Camera.h"
#include "Bullet.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f)); // Start the camera at (0,0,10) looking towards the origin

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cooldown = 0.8f;

std::vector<Bullet> bullets; // Vector to hold active bullets

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

	window = glfwCreateWindow(WIDTH, HEIGHT, "TrabalhoGrauA", nullptr, nullptr);

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
	std::vector<float> teapotVertices;



	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);

	return EXIT_SUCCESS;
}

int System::SystemSetup()
{

	coreShader = Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	coreShader.Use();

	coreShader.LoadTexture((char*)"Textures/wood.jpg", (char*)"texture1", "woodTexture");
	coreShader.UseTexture("woodTexture");

	setupBullet();

	std::vector<float> teapotVertices;
	if (loadObj("teapot.obj", teapotVertices) && !teapotVertices.empty())
	{
	
		Mesh* teapotMesh = new Mesh(teapotVertices); // Mesh a ser reutilizado

		GameObject teapot1(teapotMesh);

		teapot1.transform = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, 0.0f)); // Manipulando as 3 transformações básicas
		teapot1.transform = glm::scale(teapot1.transform, glm::vec3(0.2f)); // Escala uniforme
		teapot1.isEliminable = true;
		this->sceneObjects.push_back(teapot1);

		GameObject teapot2(teapotMesh);

		teapot2.transform = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, 0.0f));
		teapot2.transform = glm::scale(teapot2.transform, glm::vec3(0.2f));
		teapot2.isEliminable = false;
		this->sceneObjects.push_back(teapot2);

	}

	return EXIT_SUCCESS;
}

void System::setupBullet()
{
	// Vértices de um cubo pequeno
	float vertices[] = { 
		-0.05f,-0.05f,-0.05f, 0.05f,-0.05f,-0.05f, 0.05f, 0.05f,-0.05f, 0.05f, 0.05f,-0.05f,-0.05f, 0.05f,-0.05f,-0.05f,-0.05f,-0.05f,
		-0.05f,-0.05f, 0.05f, 0.05f,-0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f,-0.05f, 0.05f, 0.05f,-0.05f,-0.05f, 0.05f,
		-0.05f, 0.05f, 0.05f,-0.05f, 0.05f,-0.05f,-0.05f,-0.05f,-0.05f,-0.05f,-0.05f,-0.05f,-0.05f,-0.05f, 0.05f,-0.05f, 0.05f, 0.05f,
		0.05f, 0.05f, 0.05f, 0.05f, 0.05f,-0.05f, 0.05f,-0.05f,-0.05f, 0.05f,-0.05f,-0.05f, 0.05f,-0.05f, 0.05f, 0.05f, 0.05f, 0.05f,
		-0.05f,-0.05f,-0.05f, 0.05f,-0.05f,-0.05f, 0.05f,-0.05f, 0.05f, 0.05f,-0.05f, 0.05f,-0.05f,-0.05f, 0.05f,-0.05f,-0.05f,-0.05f,
		-0.05f, 0.05f,-0.05f, 0.05f, 0.05f,-0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f,-0.05f, 0.05f, 0.05f,-0.05f, 0.05f,-0.05f
	};

	// Setup do tiro
	GLuint VBO;
	glGenVertexArrays(1, &this->bulletVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->bulletVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void processInput(GLFWwindow* window, float deltaTime)

{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard('W', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard('S', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard('L', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard('R', deltaTime);

	static float lastShotTime = 0.0f;
	

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (glfwGetTime() - lastShotTime) > cooldown)

	{

		Bullet newBullet(camera.Position, camera.Front); // Cria uma nova bala na posição da câmera
		bullets.push_back(newBullet);
		lastShotTime = glfwGetTime();
	}
}

bool checkCollision(const AABB& box, const glm::vec3& point)
{
	// O ponto está colidindo se, E SOMENTE SE, sua posição
	// estiver entre os limites min e max da caixa em TODOS os três eixos (X, Y e Z).
	return (point.x >= box.min.x && point.x <= box.max.x
		&& point.y >= box.min.y && point.y <= box.max.y
		&& point.z >= box.min.z && point.z <= box.max.z);
}

void System::Run()

{
	// Matriz MVP -- Model, View, Projection
	GLuint viewLoc = glGetUniformLocation(coreShader.ID, "view");
	GLuint projLoc = glGetUniformLocation(coreShader.ID, "projection");
	GLuint modelLoc = glGetUniformLocation(coreShader.ID, "model");

	while (!glfwWindowShouldClose(window)) {

		// Check and call events
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		processInput(window, deltaTime);

		// Background Fill Color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		coreShader.Use();
		glUniform3f(glGetUniformLocation(coreShader.ID, "lightPos"), 0.0f, 5.0f, 0.0f);
		glUniform3f(glGetUniformLocation(coreShader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);

		glUniform3f(glGetUniformLocation(coreShader.ID, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		glUniform3f(glGetUniformLocation(coreShader.ID, "objectColor"), 1.0f, 0.5f, 0.31f);
		//coreShader.UseTexture("woodTexture");

		// Onde o objeto está no mundo? (Matriz Model)
		glm::mat4 view = camera.GetViewMatrix(); // De onde estamos olhando para o mundo?
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f); // Como se fosse a "lente" da câmera

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		for (auto& object : this->sceneObjects)
		{
			if (object.active)

			{
				// Animação opcional: rotacionar os objetos
				object.transform = glm::rotate(object.transform, glm::radians(2.0f) * deltaTime * 50.f, glm::vec3(0.2f, 1.0f, 0.0f));

				// Manda cada objeto se desenhar
				object.draw(coreShader);
			}
		}


		glBindVertexArray(bulletVAO); // Ativa o "molde" de desenho da bala

		for (auto& bullet : bullets)
		{
			// Manda a bala se atualizar (posição e tempo de vida)
			bullet.update(deltaTime);

			// Se a bala ainda estiver ativa, desenhe-a
			if (bullet.active)
			{

				for (auto& object : sceneObjects) // Verifica colisão com todos os objetos da cena
				{
					if (object.active)
					{
						AABB objectbox = object.getAABB();

						if (checkCollision(objectbox, bullet.position))
						{
							
							if (object.isEliminable && bullet.checkingCollision)

							{
								std::cout << "Object eliminated!" << std::endl;

								object.active = false; // Marca o objeto para remoção
								bullet.active = false; // Desativa a bala
							}

							else
							{
								if (bullet.checkingCollision)
								{
									std::cout << "Object hit but not eliminable!" << std::endl;

									bullet.direction = -bullet.direction;
									bullet.position += bullet.direction * 0.1f;

									bullet.checkingCollision = false; // Evita múltiplas colisões
								}
							}
							
							break;
						}
					}
				}

				// Cria uma matriz 'model' específica para a posição DESTA bala
				glm::mat4 bulletModel = glm::mat4(1.0f);
				bulletModel = glm::translate(bulletModel, bullet.position);

				// Envia esta matriz para o shader (reutilizando a modelLoc)
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bulletModel));

				// Desenha o cubo da bala
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		glBindVertexArray(0);

		bullets.erase(
			std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }),
			bullets.end()
		);


		glfwSwapBuffers(window); // Shows what was drawn in this frame
	}

	// Onde o objeto está no mundo? (Matriz Model)
	// De onde estamos olhando para este mundo ? (Matriz View)
	// Qual a perspectiva da nossa "lente" ? (A visão é "olho de peixe" ou "zoom" ? ) (Matriz Projection)
}

void System::Finish()
{
	coreShader.Delete();

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
}

// System.cpp (Adicionar no final)

void System::calcularBSpline()
{

	this->pontosDaCurva.clear();

	if (this->pontosDeControle.size() < 4) return;

	for (size_t i = 0; i < this->pontosDeControle.size() - 3; ++i)
	{
		// Identificamos os 4 atores da cena atual para ficar legível
		glm::vec3 p0 = pontosDeControle[i];     // Ponto i
		glm::vec3 p1 = pontosDeControle[i + 1]; // Ponto i+1
		glm::vec3 p2 = pontosDeControle[i + 2]; // Ponto i+2
		glm::vec3 p3 = pontosDeControle[i + 3]; // Ponto i+3

		for (float t = 0.0f; t < 1.0f; t += 0.05f) // 4. Parâmetro 't' varia de 0 a 1 para cada segmento
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

	glBindBuffer(GL_ARRAY_BUFFER, this->curvaVBO);
	glBufferData(GL_ARRAY_BUFFER, this->pontosDaCurva.size() * sizeof(glm::vec3), this->pontosDaCurva.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}