#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\euler_angles.hpp>

#include <GLFW/glfw3.h>
#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include "Material.h"
#include "Model.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imgui/ImGuizmo.h>
#include <imgui/ImSequencer.h>

#include "imgui/ImCurveEdit.h"
#include "imgui/imgui_internal.h"

#include <vector>



using namespace glm;
//using namespace Eigen;


const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0, uniformFarPlane = 0;

Window mainWindow;

std::vector<Shader> shaderList;

Camera camera;

//Texture dirtTexture;

Material shinyMaterial;
Material dullMaterial;

Model sphere, kiss, jaw_open;

glm::mat4 model;
glm::mat4 newModel(1);
glm::mat4 fpCamera(1);
glm::mat4 projection;

Texture planeTexture;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//weights
float w_jaw, w_kiss;

glm::mat4 mat_cast;

GLfloat timer;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

bool wireframe;
glm::mat4 thirdPerson;
glm::vec3 offset;

bool fPerson = false, tPerson = false, freelook = true;

glm::mat4 inverseModel = glm::mat4();

vec3 intersectionPoint = vec3(0.f,0.f,0.f);
vec3 finalVertex = vec3(0.f, 0.f, 0.f);

std::vector<Mesh*> meshList;
Texture dirtTexture;


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void RenderScene()
{
	model = glm::mat4();
	//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 2.5f));
	//model = glm::scale(model, glm::vec3(2.f, 2.f, 2.f));
	

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();
	
	sphere.RenderModel();

}


void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetTexture(1);
	shaderList[0].Validate();

	RenderScene();
}


void bind_keys(bool* keys, GLfloat deltaTime)
{
	//GLfloat velocity = deltaTime *;
	if (keys[GLFW_KEY_U]) {
		glfwSetInputMode(mainWindow.mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}



int main() 
{
	glewExperimental = true;

	mat_cast = glm::mat4();

	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateShaders();

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-20.0f, 0.0f, -20.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		20.0f, 0.0f, -20.0f,	20.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-20.0f, 0.0f, 20.0f,	0.0f, 20.0f,	0.0f, -1.0f, 0.0f,
		20.0f, 0.0f, 20.0f,		20.0f, 20.0f,	0.0f, -1.0f, 0.0f
	};


	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	camera = Camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 7.0f, 0.5f);

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	//Loading models
	sphere = Model();
	sphere.LoadModel("C:\\Users\\avich\\source\\repos\\Animation\\Model Loading+Skybox - Cleaned\\High-res Blendshape Model\\neutral.obj");

	std::vector<Model> blendshapeModels;

	kiss = Model();
	kiss.LoadModel("C:\\Users\\avich\\source\\repos\\Animation\\Model Loading+Skybox - Cleaned\\High-res Blendshape Model\\Mery_kiss.obj");

	jaw_open = Model();
	jaw_open.LoadModel("C:\\Users\\avich\\source\\repos\\Animation\\Model Loading+Skybox - Cleaned\\High-res Blendshape Model\\Mery_jaw_open.obj");

	
	//Load Textures
	//planeTexture = Texture("C:\\Users\\avich\\source\\repos\\Animation\\Model Loading+Skybox - Cleaned\\Facial Animation\\Textures\\blue-tex.jpg");
	//planeTexture.LoadTextureA();

	dirtTexture = Texture("Textures/chess.png");
	dirtTexture.LoadTextureA();

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	const char* glsl_version = "#version 330";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(mainWindow.mainWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = true;
	//bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//main loop
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); 
		deltaTime = now - lastTime; 
		lastTime = now;

		glfwPollEvents();

		bind_keys(mainWindow.getsKeys(), deltaTime);
		
		//glfwSetMouseButtonCallback(mainWindow.mainWindow, mouse_Callbacks);

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			static int counter = 0;
			
			ImGui::Begin("Flight Deck");                          

			ImGui::Text("Controls");  
			ImGui::SliderFloat("Jaw Open", &w_jaw, 0.f,1.f);
			ImGui::SliderFloat("kiss", &w_kiss,0.f,1.f);
			
			ImGui::Checkbox("Wireframe", &wireframe);

			if (wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			ImGui::Text("Camera");
			ImGui::Checkbox("Free Look", &freelook);


			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			ImGui::End();
		}

		RenderPass(camera.calculateViewMatrix(), projection);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

		mainWindow.swapBuffers();
	}

	return 0;
}