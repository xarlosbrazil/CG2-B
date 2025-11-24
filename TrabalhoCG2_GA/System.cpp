#include "System.h"
#include "loadObj.h"
#include "Camera.h"
#include "Bullet.h"
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
