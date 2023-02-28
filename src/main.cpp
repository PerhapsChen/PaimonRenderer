#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "global.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "mesh.h"
#include "skybox.h"
#include "ModleFromProgram.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;
bool blinn = true;
bool shadow = true;

//Camera camera(glm::vec3(1.98434,2.08286,2.12496));
Camera camera(-0.374708,0.893665,0.927288, 0.0f, 1.0f, 0.0f, -71, -23.2);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

extern vector<float> skyboxVertices;
extern vector<std::string> faces;
extern vector<float> planeVertices;

int main()
{
	glfwInit();
    const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "0.0", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
    // - 操控相机的回调函数，使用ImGui时暂时禁用。
	// glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);
	// glfwSetScrollCallback(window, scroll_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    //stbi_set_flip_vertically_on_load(true);

    // -- IMGUI --- Init and Binding------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); 
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    io.Fonts->AddFontFromFileTTF("../../3rdParty/imgui/misc/fonts/Cousine-Regular.ttf", 25.0f);

    // -- IMGUI --- Panel Info
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    float outlineScale = 0.004;
    // -- IMGUI --- END

    
	glEnable(GL_DEPTH_TEST);
    extern std::string projectPath; // -defined in global.h

    Shader skyboxShader((projectPath + "shader/skybox.vs").c_str(), 
                        (projectPath + "shader/skybox.fs").c_str());
    Skybox skybox(faces, skyboxVertices);

    Shader shader((projectPath + "shader/shadow_mapping.vs").c_str(),
                  (projectPath + "shader/shadow_mapping.fs").c_str());
    Shader simpleDepthShader((projectPath + "shader/shadow_mapping_depth.vs").c_str(),
                             (projectPath + "shader/shadow_mapping_depth.fs").c_str());

    Shader backOutline((projectPath + "shader/back_outline.vs").c_str(),
                       (projectPath + "shader/back_outline.fs").c_str());

    Model paimon((projectPath + "assets/paimon/paimon.obj").c_str());
    
    ModelFromProgram plane(planeVertices);
    
    
    glm::vec3 lightPos(0.66*1.3,1.7*1.3,1.35*1.3);//! Light Position


    //- Configure depth map FBO
    const GLuint SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    //- Create depth texture
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader.use();
    // shader.setInt("diffuseTexture", 0);
    // shader.setInt("shadowMap", 1);

    // render loop
    while (!glfwWindowShouldClose(window))
    {	
        // cout << camera.Position.x << camera.Position.y << camera.Position.z << endl;
        // cout << camera.Pitch << endl;
        // cout << camera.Yaw << endl;
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------------------------------------------------
        //! 1. Render depth of scene to texture (from light's perspective)
        // - Get light projection/view matrix.
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("model", model);
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); //bind
        glClear(GL_DEPTH_BUFFER_BIT);

        simpleDepthShader.use();
        glActiveTexture(GL_TEXTURE0);
        for(auto& mesh: paimon.m_meshes){
            mesh.Draw();
        }
        plane.Draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);           // unbind
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);        // set back
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //-------------------------------------------------------------------

        //- cal mvp
        shader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::rotate(model,glm::radians(0.00f), glm::vec3(0.0, 1.0, 0.0));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        //- transform(mvp) info to Shader
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        //- light info to Shader
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        // shader.setInt("shadow", shadow);

        shader.setVec3("ambient_color", glm::vec3(clear_color.x,clear_color.y,clear_color.z));

        backOutline.use();
        backOutline.setMat4("projection", projection);
        backOutline.setMat4("view", view);
        backOutline.setMat4("model", model);
        backOutline.setVec3("viewPos", camera.Position);
        backOutline.setVec3("lightPos", lightPos);
        backOutline.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        backOutline.setFloat("outlineScale", outlineScale);


        //! 如果某个Mesh需要添加额外的Texture 使用mesh.addTexture(Texture&)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        for(auto& mesh: paimon.m_meshes){
            backOutline.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(glGetUniformLocation(backOutline.ID, "shadowMap"), 0);
            unsigned int nextLoc = mesh.bindTexture(backOutline, 1);
            mesh.Draw();    
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        for(auto& mesh: paimon.m_meshes){
            shader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(glGetUniformLocation(shader.ID, "shadowMap"), 0);
            unsigned int nextLoc = mesh.bindTexture(shader, 1);
            mesh.Draw();    
        }
		//----------------------- plane------------------------

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        plane.Draw();

        //----------------------- draw skybox----------------------------------------------
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skybox.Draw(skyboxShader);

        
        glfwPollEvents();

        //! IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow(&show_demo_window);
        {
            // static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Panel");                         

            ImGui::Text("Welcome to PerhapsChen's App.");               
            //ImGui::Checkbox("Demo Window", &show_demo_window);      
            //ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("Outline Scale", &outlineScale, 0.0f, 0.1f);            
            ImGui::ColorEdit3("Ambient Color", (float*)&clear_color); 

            // if (ImGui::Button("Button"))      
            // ImGui::SameLine();
            // ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // push the ImGui rendered buffer data to OpenGL
        
        //! IMGUI END
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
