#pragma once
#include <vector>
#include <string>
using namespace std;

std::string projectPath {"C:/Users/45162.CPHXR9000K/Desktop/PackageTutorial/RenderSomething/"};

vector<float> skyboxVertices = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};

vector<std::string> faces{
    // sky box textures
    projectPath + "assets/skybox_universe/PurpleNebula2048_right.jpg",
    projectPath + "assets/skybox_universe/PurpleNebula2048_left.jpg",
    projectPath + "assets/skybox_universe/PurpleNebula2048_top.jpg",
    projectPath + "assets/skybox_universe/PurpleNebula2048_bottom.jpg",
    projectPath + "assets/skybox_universe/PurpleNebula2048_front.jpg",
    projectPath + "assets/skybox_universe/PurpleNebula2048_back.jpg",
};

vector<float> planeVertices = {
   1.0f, 0.0f,  1.0f-0.5,  0.0f, 1.0f, 0.0f, 
   1.0f, 0.0f, -1.0f-0.5,  0.0f, 1.0f, 0.0f, 
  -1.0f, 0.0f, -1.0f-0.5,  0.0f, 1.0f, 0.0f,   
   1.0f, 0.0f,  1.0f-0.5,  0.0f, 1.0f, 0.0f,  
  -1.0f, 0.0f, -1.0f-0.5,  0.0f, 1.0f, 0.0f,
  -1.0f, 0.0f,  1.0f-0.5,  0.0f, 1.0f, 0.0f,  
};

// unsigned int indices[] = {
//     0, 1, 3, // first triangle
//     3, 2, 0  // second triangle
// };

// cout << camera.Position.x << camera.Position.y << camera.Position.z << endl;
// cout << camera.Pitch << endl;
// cout << camera.Yaw << endl;