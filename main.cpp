#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include "Model Loading\meshLoaderObj.h"
#include "GameObjects\Ship\ship.h"
#include <math.h>

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"


int checkCollision(glm::vec3 pos1, glm::vec3 pos2);
int checkEligibility(glm::vec3 pos, int x1, int x2, int y1, int y2, int z1, int z2);

void processKeyboardInput ();

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int cameraMode = 0;
int gameLevel = 0;
int cinematic = 0;
int started = 0;
int blocked = 1;

Window window("Game Engine", 800, 800);
Camera camera;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

float skyboxVertices[] =
{
    //   Coordinates
    -10000000.0f, -10000000.0f,  10000000.0f,//        7--------6
     10000000.0f, -10000000.0f,  10000000.0f,//       /|       /|
     10000000.0f, -10000000.0f, -10000000.0f,//      4--------5 |
    -10000000.0f, -10000000.0f, -10000000.0f,//      | |      | |
    -10000000.0f,  10000000.0f,  10000000.0f,//      | 3------|-2
     10000000.0f,  10000000.0f,  10000000.0f,//      |/       |/
     10000000.0f,  10000000.0f, -10000000.0f,//      0--------1
    -10000000.0f,  10000000.0f, -10000000.0f
};

unsigned int skyboxIndices[] =
{
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
};

int main()
{
	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
	Shader planeShader("Shaders/vertex_shader2.glsl", "Shaders/fragment_shader2.glsl");
    Shader skyboxShader("Shaders/skybox_vertex_shader.glsl", "Shaders/skybox_fragment_shader.glsl");

	//Textures
	GLuint tex = loadBMP("Resources/Textures/nebula1.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/rock.bmp");
	GLuint tex3 = loadBMP("Resources/Textures/island.bmp");
	GLuint tex4 = loadBMP("Resources/Textures/ship.bmp");
    GLuint tex5 = loadBMP("Resources/Textures/asteroids.bmp");
    GLuint tex6 = loadBMP("Resources/Textures/down.bmp");
    GLuint tex7 = loadBMP("Resources/Textures/up.bmp");
    GLuint tex8 = loadBMP("Resources/Textures/cloud.bmp");
    GLuint tex9 = loadBMP("Resources/Textures/tree.bmp");
    GLuint tex10 = loadBMP("Resources/Textures/mountain.bmp");
    GLuint tex11 = loadBMP("Resources/Textures/purple_crystal.bmp");
    GLuint tex12 = loadBMP("Resources/Textures/mage.bmp");
    //GLuint tex13 = loadBMP("Resources/Textures/robomech.bmp");
    GLuint tex14 = loadBMP("Resources/Textures/portal.bmp");
    GLuint tex15 = loadBMP("Resources/Textures/drone.bmp");
    GLuint tex16 = loadBMP("Resources/Textures/mission.bmp");
    GLuint tex17 = loadBMP("Resources/Textures/flower.bmp");
    GLuint tex18 = loadBMP("Resources/Textures/corridor.bmp");
    GLuint tex19 = loadBMP("Resources/Textures/planet.bmp");
    GLuint tex20 = loadBMP("Resources/Textures/redcrys.bmp");
    GLuint tex21 = loadBMP("Resources/Textures/bluecrys.bmp");

	glEnable(GL_DEPTH_TEST);

	//Test custom mesh loading
	std::vector<Vertex> vert;
	vert.push_back(Vertex());
	vert[0].pos = glm::vec3(10.5f, 10.5f, 0.0f);
	vert[0].textureCoords = glm::vec2(1.0f, 1.0f);

	vert.push_back(Vertex());
	vert[1].pos = glm::vec3(10.5f, -10.5f, 0.0f);
	vert[1].textureCoords = glm::vec2(1.0f, 0.0f);

	vert.push_back(Vertex());
	vert[2].pos = glm::vec3(-10.5f, -10.5f, 0.0f);
	vert[2].textureCoords = glm::vec2(0.0f, 0.0f);

	vert.push_back(Vertex());
	vert[3].pos = glm::vec3(-10.5f, 10.5f, 0.0f);
	vert[3].textureCoords = glm::vec2(0.0f, 1.0f);

	vert[0].normals = glm::normalize(glm::cross(vert[1].pos - vert[0].pos, vert[3].pos - vert[0].pos));
	vert[1].normals = glm::normalize(glm::cross(vert[2].pos - vert[1].pos, vert[0].pos - vert[1].pos));
	vert[2].normals = glm::normalize(glm::cross(vert[3].pos - vert[2].pos, vert[1].pos - vert[2].pos));
	vert[3].normals = glm::normalize(glm::cross(vert[0].pos - vert[3].pos, vert[2].pos - vert[3].pos));

	std::vector<int> ind = { 0, 1, 3,   
		1, 2, 3 };

	std::vector<Texture> textures;
	textures.push_back(Texture());
	textures[0].id = tex;
	textures[0].type = "texture_diffuse";

	std::vector<Texture> textures2;
	textures2.push_back(Texture());
	textures2[0].id = tex2;
	textures2[0].type = "texture_diffuse";

	std::vector<Texture> textures3;
	textures3.push_back(Texture());
	textures3[0].id = tex3;
	textures3[0].type = "texture_diffuse";

	std::vector<Texture> textures4;
	textures4.push_back(Texture());
	textures4[0].id = tex4;
	textures4[0].type = "texture_diffuse";

    std::vector<Texture> textures5;
    textures5.push_back(Texture());
    textures5[0].id = tex5;
    textures5[0].type = "texture_diffuse";

    std::vector<Texture> textures6;
    textures6.push_back(Texture());
    textures6[0].id = tex6;
    textures6[0].type = "texture_diffuse";

    std::vector<Texture> textures7;
    textures7.push_back(Texture());
    textures7[0].id = tex7;
    textures7[0].type = "texture_diffuse";

    std::vector<Texture> textures8;
    textures8.push_back(Texture());
    textures8[0].id = tex8;
    textures8[0].type = "texture_diffuse";

    std::vector<Texture> textures9;
    textures9.push_back(Texture());
    textures9[0].id = tex9;
    textures9[0].type = "texture_diffuse";

    std::vector<Texture> textures10;
    textures10.push_back(Texture());
    textures10[0].id = tex10;
    textures10[0].type = "texture_diffuse";

    std::vector<Texture> textures11;
    textures11.push_back(Texture());
    textures11[0].id = tex11;
    textures11[0].type = "texture_diffuse";

    std::vector<Texture> textures12;
    textures12.push_back(Texture());
    textures12[0].id = tex12;
    textures12[0].type = "texture_diffuse";


    /*std::vector<Texture> textures13;
    textures13.push_back(Texture());
    textures13[0].id = tex13;
    textures13[0].type = "texture_diffuse";*/

    std::vector<Texture> textures14;
    textures14.push_back(Texture());
    textures14[0].id = tex14;
    textures14[0].type = "texture_diffuse";

    std::vector<Texture> textures15;
    textures15.push_back(Texture());
    textures15[0].id = tex15;
    textures15[0].type = "texture_diffuse";

    std::vector<Texture> textures16;
    textures16.push_back(Texture());
    textures16[0].id = tex16;
    textures16[0].type = "texture_diffuse";

    std::vector<Texture> textures17;
    textures17.push_back(Texture());
    textures17[0].id = tex17;
    textures17[0].type = "texture_diffuse";

    std::vector<Texture> textures18;
    textures18.push_back(Texture());
    textures18[0].id = tex18;
    textures18[0].type = "texture_diffuse";

    std::vector<Texture> textures19;
    textures19.push_back(Texture());
    textures19[0].id = tex19;
    textures19[0].type = "texture_diffuse";

    std::vector<Texture> textures20;
    textures20.push_back(Texture());
    textures20[0].id = tex20;
    textures20[0].type = "texture_diffuse";

    std::vector<Texture> textures21;
    textures21.push_back(Texture());
    textures21[0].id = tex21;
    textures21[0].type = "texture_diffuse";


	Mesh mesh(vert, ind, textures3);

	// Create Obj files - easier :)
	// we can add here our textures :)
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh box = loader.loadObj("Resources/Models/box.obj", textures);
	Mesh ship = loader.loadObj("Resources/Models/ship.obj", textures4);
    Mesh asteroids = loader.loadObj("Resources/Models/asteroids.obj", textures5);
    Mesh down = loader.loadObj("Resources/Models/down.obj", textures6);
	Mesh up = loader.loadObj("Resources/Models/up.obj", textures7);
    Mesh cloud = loader.loadObj("Resources/Models/cloud.obj", textures8);
    Mesh tree = loader.loadObj("Resources/Models/tree.obj", textures9);
    Mesh mountain = loader.loadObj("Resources/Models/mountain.obj", textures10);
    Mesh purpleCrystal = loader.loadObj("Resources/Models/purple_crystal.obj", textures11);
    Mesh mage = loader.loadObj("Resources/Models/mage.obj", textures12);
    //Mesh robomech = loader.loadObj("Resources/Models/robomech.obj", textures13);
    Mesh portal = loader.loadObj("Resources/Models/portal.obj", textures14);
    Mesh drone = loader.loadObj("Resources/Models/drone.obj", textures15);
    Mesh mission = loader.loadObj("Resources/Models/mission.obj", textures16);
    Mesh flower = loader.loadObj("Resources/Models/flower.obj", textures17);
    Mesh corridor = loader.loadObj("Resources/Models/corridor.obj", textures18);
    Mesh planet = loader.loadObj("Resources/Models/planet.obj", textures19);
    Mesh redcrys = loader.loadObj("Resources/Models/crystal.obj", textures20);
    Mesh bluecrys = loader.loadObj("Resources/Models/crystal.obj", textures21);

    skyboxShader.use();
    glUniform1i(glGetUniformLocation(skyboxShader.getId(), "skybox"), 0);

    // Create VAO, VBO, and EBO for the skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // All the faces of the cubemap (make sure they are in this exact order)
    std::string facesCubemap[6] =
    {
        "Resources/Textures/right.bmp",
        "Resources/Textures/left.bmp",
        "Resources/Textures/top.bmp",
        "Resources/Textures/bottom.bmp",
        "Resources/Textures/front.bmp",
        "Resources/Textures/back.bmp"
    };

    // Creates the cubemap texture object
    unsigned int cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++)
    {
        printf("Reading image %s\n", facesCubemap[i].c_str());

        unsigned char header[54];
        unsigned int dataPos;
        unsigned int imageSize;
        unsigned int width, height;

        unsigned char* data;

        FILE* file;
        errno_t err = fopen_s(&file, facesCubemap[i].c_str(), "rb");
        if (err)
        {
            printf("%s could not be opened.", facesCubemap[i].c_str()); getchar(); return 0;
        }

        if (fread(header, 1, 54, file) != 54) {
            printf("Not a correct BMP file\n");
            return 0;
        }

        // Parsing BMP file
        if (header[0] != 'B' || header[1] != 'M') {
            printf("Not a correct BMP file\n");
            return 0;
        }

        if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
        if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

        dataPos = *(int*)&(header[0x0A]);
        imageSize = *(int*)&(header[0x22]);
        width = *(int*)&(header[0x12]);
        height = *(int*)&(header[0x16]);

        if (imageSize == 0)    imageSize = width * height * 3;
        if (dataPos == 0)      dataPos = 54;


        data = new unsigned char[imageSize];

        // Read data into buffer
        fread(data, 1, imageSize, file);

        fclose(file);

        glTexImage2D
        (
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            data
        );

        delete[] data;
    }

	glm::vec3 positions[] =
	{
        glm::vec3(400.0f, 1200.0f, 1020.0f),
        glm::vec3(-300.0f, 1500.0f, 1020.0f),

        glm::vec3(-1050.0f, 600.0f, 820.0f),
        glm::vec3(1250.0f, 800.0f, 820.0f),

        glm::vec3(-1250.0f, 50.0f, 820.0f),
        glm::vec3(1050.0f, 300.0f, 1120.0f),

        glm::vec3(1000.0f, -20.0f, 130.0f),
        glm::vec3(-1000.0f, 20.0f, -180.0f),

        glm::vec3(-830.0, 680.0f, -1090.0f),
        glm::vec3(940.0f, 165.0f, -1015.0f),

        glm::vec3(-590.0f, 1065.0f, 556.0f),
        glm::vec3(110.0f, 170.0f, 502.0f),

        glm::vec3(440.0f, 775.0f, 475.0f),
        glm::vec3(-544.0f, -760.0f, 478.0f),

        glm::vec3(0.0f, -360.0f, -72.0f),
        glm::vec3(-321.0f, 460.0f, 455.0f),

        glm::vec3(0.0f, 0.0f, -1315.0f),

        glm::vec3(1271.5f, -1177.0f, 615.0f),
        glm::vec3(-1462.5f, -1008.0f, 585.0f),

        glm::vec3(626.5f, -838.0f, 161.0f),
        glm::vec3(-1796.5f, 105.0f, -961.0f),

	};

    int positionsCollision[] =
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,

    };

    glm::vec3 treePositions[] =
    {
        glm::vec3(0.0f, -10.0f, 50.0f),
        glm::vec3(-150.0f, 3.5f, 318.0f),
        glm::vec3(-320.0f, -10.0f, 300.0f),
        glm::vec3(145.0f, 3.5f, 416.0f),
    };


    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    int myDialogue1 = 1;
    int dialogue3 = 1;
    int dialogue32 = 1;
    int dialogue6 = 1;

    Ship starship = Ship(glm::vec3(0, 0, 2000), 0.05f);
    
    std::string text = "TASK: Reach the Teleporter Drone to beam to planet surface.";
    std::string instruction = " ";
    std::string dialogue = " ";
    std::string myDialogue = " ";
    std::string buttonText = "START";


    int quest2 = 0;
    int quest3 = 0;
    int quest4 = 0;
    int quest5 = 0;
    int quest6 = 1;

    glm::vec3 droneQuest = glm::vec3(0.0f, 30.0f, -2500.0f);
    glm::vec3 droneQuest2 = glm::vec3(100.0f, 70.0f, 380.0f);
    glm::vec3 mageQuest = glm::vec3(-50, 10, 250);
    
    Ship ship2 = Ship(glm::vec3(0.0f, -10.0f, -500.0f), 0.05f);

    int questUp = 1;
    int questDown = 0;

    int checkCrouch = 0;
    int checkSpawn1 = 0;
    int checkSpawn2 = 0;

	//check if we close the window or press the escape button
    while (!window.isPressed(GLFW_KEY_ESCAPE) &&
        glfwWindowShouldClose(window.getWindow()) == 0)
    {
        window.clear();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyboardInput();

        // Tell OpenGL a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        //test mouse input
        if (window.isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            std::cout << "Pressing mouse button" << std::endl;
        }
        //// Code for the light ////

        sunShader.use();

        glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
        glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

        GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

        //Test for one Obj loading = light source

        glm::mat4 ModelMatrix = glm::mat4(1.0);
        ModelMatrix = glm::translate(ModelMatrix, lightPos);
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.001f));
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        sun.draw(sunShader);

        skyboxShader.use();
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        // We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
        // The last row and column affect the translation of the skybox (which we don't want to affect)
        view = glm::mat4(glm::mat3(glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp())));
        projection = glm::perspective(45.0f, (float)window.getWidth() / window.getHeight(), 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
        // where an object is present (a depth of 1.0f will always fail against any object's depth value)
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Switch back to the normal depth function
        glDepthFunc(GL_LESS);


        //// End code for the light ////

        ///// Test Obj files for box ////
        if (gameLevel == 0)
        {
            shader.use();

            GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
            GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");


            //...........ship...........//

            ModelMatrix = glm::mat4(1.0);

            for (int i = 0; i < 21; i++)
                if (!checkCollision(starship.getPosition(), positions[i]))
                {
                    if(!blocked)
                    {
                        if (window.isPressed(GLFW_KEY_W) && starship.getPosition().z > -2300)
                            starship.setPosition(starship.getPosition() + glm::vec3(0, 0, -starship.getSpeed()));
                        if (window.isPressed(GLFW_KEY_S) && starship.getPosition().z < 2000)
                            starship.setPosition(starship.getPosition() + glm::vec3(0, 0, starship.getSpeed()));
                        if (window.isPressed(GLFW_KEY_A))
                            starship.setPosition(starship.getPosition() + glm::vec3(-starship.getSpeed(), 0, 0));
                        if (window.isPressed(GLFW_KEY_D))
                            starship.setPosition(starship.getPosition() + glm::vec3(starship.getSpeed(), 0, 0));
                        if (window.isPressed(GLFW_KEY_R))
                            starship.setPosition(starship.getPosition() + glm::vec3(0, starship.getSpeed(), 0));
                        if (window.isPressed(GLFW_KEY_F))
                            starship.setPosition(starship.getPosition() + glm::vec3(0, -starship.getSpeed(), 0));
                    }
                }
                else
                {
                    //starship.setPosition(glm::vec3(0, 0, 0));

                    positionsCollision[i] = 1;
                }


            if (checkEligibility(starship.getPosition(), -80, 120, -130, 35, -2400, -2100))
            {
                instruction = "INSTRUCTION: Press E to interact.";
                dialogue = "DRONE: Hello captain! Are you ready to teleport to surface?";
                if (window.isPressed(GLFW_KEY_E))
                {
                    gameLevel = 1;
                    cameraMode = 1;
                    quest2 = 1;
                    camera.setCameraPosition(glm::vec3(-2, 65, 485));
                    questUp = 1;
                    questDown = 0;

                    instruction = "";
                    text = "";
                    dialogue = "";
                }
            }
            if (gameLevel == 0)
                camera.setCameraPosition(starship.getPosition() + glm::vec3(0.0f, 100.0f, 250.0f));

            //ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, starship.getPosition());
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            ship.draw(shader);


            //...........asteroids...........//

            for (int i = 0; i < 21; i++)
            {
                ModelMatrix = glm::mat4(1.0);
                ModelMatrix = glm::translate(positions[i]);

                if (positionsCollision[i] && positions[i].y >= -10000)
                    positions[i].y -= 0.1;

                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
                glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
                glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
                glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
                asteroids.draw(shader);
            }

            //....................planet..............//

            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 000.0f, -13000.0f));
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(25.0f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            planet.draw(shader);

            //....................clouds..............//
            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -2000.0f, -6500.0f));
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(7.2f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            cloud.draw(shader);

            //........................drone............//

            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, -2500.0f));
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(7.2f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            drone.draw(shader);

            //........................mission............//

            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, droneQuest);

            if (droneQuest.y <= 60 && questUp)
                droneQuest.y += 0.05;
            if (droneQuest.y >= 60)
            {
                questDown = 1;
                questUp = 0;
            }

            if (droneQuest.y >= 30 && questDown)
                droneQuest.y -= 0.05;
            if (droneQuest.y <= 30)
            {
                questDown = 0;
                questUp = 1;
            }

            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            mission.draw(shader);

            //...............island..................//

            planeShader.use();

            MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
            ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

            ModelMatrix = glm::mat4(1.0);

            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(planeShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(planeShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(planeShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -3000.0f, -7000.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            down.draw(planeShader);

            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -3000.0f, -7000.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            up.draw(planeShader);

        }

        if (gameLevel == 1)
        {
            //camera.setCameraPosition(glm::vec3(-2, 65, 525));


            planeShader.use();

            GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
            GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

            ModelMatrix = glm::mat4(1.0);

            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(planeShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(planeShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(planeShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            down.draw(planeShader);

            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            up.draw(planeShader);

            //...............tree.................//

            shader.use();

            for (int i = 0; i < 4; i++)
            {
                ModelMatrix = glm::mat4(1.0);
                //std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << '\n';
                //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));
                ModelMatrix = glm::translate(ModelMatrix, treePositions[i]);
                if (i == 1 || i == 3)
                    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
                glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
                glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
                glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

                tree.draw(shader);
            }

            //....................planet..............//

            ModelMatrix = glm::mat4(1.0);
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(5000.0f, 1000.0f, -10000.0f));
            ModelMatrix = glm::rotate(ModelMatrix, (float)glfwGetTime() * 10, glm::vec3(0.0f, 1.0f, 0.0f));
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(25.0f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            planet.draw(shader);

            //...........mountain.........//

            ModelMatrix = glm::mat4(1.0);
            //std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << '\n';
            //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, 0, 0));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            mountain.draw(shader);

            //...........purple crystal.........//

            ModelMatrix = glm::mat4(1.0);
            //std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << '\n';
            //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-180, 0, 100));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            purpleCrystal.draw(shader);

            //...........mage.........//

            ModelMatrix = glm::mat4(1.0);
            //std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << '\n';
            //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));
            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-50, 10, 250));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            mage.draw(shader);

            //...........portal.........//

            ModelMatrix = glm::mat4(1.0);
            //std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y << " " << camera.getCameraPosition().z << '\n';
            //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));

            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-10, 0, 350));

            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            portal.draw(shader);

            //...........clouds...........//

            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-30.0f, 50.0f, 120.0f));
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

            cloud.draw(shader);

            //........................drone............//

            ModelMatrix = glm::mat4(1.0);

            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 70.0f, 380.0f));
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f));

            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            drone.draw(shader);

            //..............flower................//
            ModelMatrix = glm::mat4(1.0);

            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(230.0f, 25.0f, 130.0f));
            //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
            //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));

            MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
            glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

            flower.draw(shader);

            /*std::cout << camera.getCameraViewDirection().x << " " << camera.getCameraViewDirection().y <<
                    " " << camera.getCameraViewDirection().z << '\n';*/

            if (quest2)
            {
                //............quest.................//
                if (myDialogue1)
                {
                    myDialogue = "YOU: Where... Where am I?";
                    instruction = "Press ENTER to continue.";
                }

                if (window.isPressed(GLFW_KEY_ENTER))
                {
                    text = "TASK: Talk to the Drone to see what happened.";
                    instruction = " ";
                    myDialogue = " ";
                    myDialogue1 = 0;
                }

                ModelMatrix = glm::mat4(1.0);
                ModelMatrix = glm::translate(ModelMatrix, droneQuest2 + glm::vec3(0, 5, 0));

                if (droneQuest2.y <= 95 && questUp)
                    droneQuest2.y += 0.03;
                if (droneQuest2.y >= 95)
                {
                    questDown = 1;
                    questUp = 0;
                }

                if (droneQuest2.y >= 75 && questDown)
                    droneQuest2.y -= 0.03;
                if (droneQuest2.y <= 75)
                {
                    questDown = 0;
                    questUp = 1;
                }

                //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.7f));
                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

                mission.draw(shader);
            }

            if (quest3)
            {
                //............quest.................//

                text = "TASK: Talk to the Lizard Wizard.";

                ModelMatrix = glm::mat4(1.0);
                ModelMatrix = glm::translate(ModelMatrix, mageQuest + glm::vec3(0, 50, -10));

                if (mageQuest.y <= 70 && questUp)
                    mageQuest.y += 0.03;
                if (mageQuest.y >= 70)
                {
                    questDown = 1;
                    questUp = 0;
                }

                if (mageQuest.y >= 55 && questDown)
                    mageQuest.y -= 0.03;
                if (mageQuest.y <= 55)
                {
                    questDown = 0;
                    questUp = 1;
                }

                //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.7f));
                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

                mission.draw(shader);
            }

            if (quest4)
            {
                if (window.isPressed(GLFW_KEY_LEFT_CONTROL))
                    checkCrouch = 1;
            }

            if (quest5)
            {
                //............quest.................//

                ModelMatrix = glm::mat4(1.0);
                ModelMatrix = glm::translate(ModelMatrix, mageQuest + glm::vec3(0, 50, -10));

                if (mageQuest.y <= 70 && questUp)
                    mageQuest.y += 0.03;
                if (mageQuest.y >= 70)
                {
                    questDown = 1;
                    questUp = 0;
                }

                if (mageQuest.y >= 55 && questDown)
                    mageQuest.y -= 0.03;
                if (mageQuest.y <= 55)
                {
                    questDown = 0;
                    questUp = 1;
                }

                //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.7f));
                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

                mission.draw(shader);
            }

            //................player................//

            if (checkEligibility(camera.getCameraPosition(), 40, 110, 65, 65, 305, 360) && quest2)
            {
                text = "";
                myDialogue = "";
                dialogue = "DRONE: Captain! Thank the stars you're fine! You were beamed\nwithout the ship! Talk to the Lizard Wizard, he may know more!";
                instruction = "INSTRUCTION: Press E to accept mission.";
                if ((-0.5f <= camera.getCameraViewDirection().x && camera.getCameraViewDirection().x <= 0.5f)
                    && (0.2f <= camera.getCameraViewDirection().z && camera.getCameraViewDirection().z <= 1.5f))
                    if (window.isPressed(GLFW_KEY_E))
                    {
                        quest2 = 0;
                        quest3 = 1;

                        questUp = 1;
                        questDown = 0;
                        dialogue = "";
                        text = "";
                        instruction = "";
                    }
            }


            if (checkEligibility(camera.getCameraPosition(), -70, 50, 65, 65, 260, 320) && quest3)
            {
                text = "";
                if (dialogue3)
                {
                    myDialogue = "YOU: Hello, mighty sorcerer! I got lost on this floating island\nand cannot find my ship. Can you help me?";
                    dialogue = "LIZARD WIZARD: Greetings, stranger! Worry not! That happens all\nthe time! The portal you came through can be powered up by these\ncrystals here. Would you like to turn the portal back up?";
                    instruction = "INSTRUCTION: Press ENTER to confirm.";
                }

                if (!dialogue3 && dialogue32)
                {
                    myDialogue = "YOU: Yes, please. How can we do that?";
                    dialogue = "LIZARD WIZARD: Well, you need to gather nectar from that flower\nover there, to imbue it with your wishes. Come back to me once\nyou are done and I shall give you what you need.";
                    instruction = "INSTRUCTION: Press E to accept mission.";
                }

                if (window.isPressed(GLFW_KEY_ENTER))
                {
                    dialogue3 = 0;
                }


                if ((-1.5f <= camera.getCameraViewDirection().x && camera.getCameraViewDirection().x <= 1.5f)
                    && (-1.8f <= camera.getCameraViewDirection().z && camera.getCameraViewDirection().z <= -0.8f))
                    if (window.isPressed(GLFW_KEY_E))
                    {
                        quest2 = 0;
                        quest3 = 0;
                        quest4 = 1;

                        questUp = 1;
                        questDown = 0;
                        dialogue32 = 0;
                        text = "TASK: Collect the flower nectar.";
                        myDialogue = "";
                        dialogue = "";

                        instruction = "";

                    }
            }



            if (checkEligibility(camera.getCameraPosition(), 141, 250, 65, 65, 150, 200) && quest4)
            {
                instruction = "INSTRUCTION: Press E to collect\nHINT: Hold CTRL before collecting.";
                if ((-0.2f <= camera.getCameraViewDirection().x && camera.getCameraViewDirection().x <= 0.9f)
                    && (-0.9f <= camera.getCameraViewDirection().z && camera.getCameraViewDirection().z <= -0.1f))
                    if (checkCrouch)
                        if (window.isPressed(GLFW_KEY_E))
                        {
                            quest2 = 0;
                            quest3 = 0;
                            quest4 = 0;
                            quest5 = 1;

                            questUp = 1;
                            questDown = 0;
                            text = "TASK: Return to the Lizard Wizard.";
                            instruction = "";
                        }

            }


            if (checkEligibility(camera.getCameraPosition(), -70, 50, 65, 65, 260, 320) && quest5)
            {
                text = "";
                myDialogue = "YOU: I have imbued the nectar with my wishes, sorcerer. What now?";
                dialogue = "LIZARD WIZARD: Excellent! Here: place these 2 crystals at the\nsides of the portal and... voila! Safe travels, stranger!";
                instruction = "INSTRUCTION: Press E to accept mission.";
                if ((-1.5f <= camera.getCameraViewDirection().x && camera.getCameraViewDirection().x <= 1.5f)
                    && (-1.8f <= camera.getCameraViewDirection().z && camera.getCameraViewDirection().z <= -0.8f))
                    if (window.isPressed(GLFW_KEY_E))
                    {
                        quest2 = 0;
                        quest3 = 0;
                        quest4 = 0;
                        quest5 = 0;
                        quest6 = 1;

                        myDialogue = "";
                        dialogue = "";
                        instruction = "";

                        text = "TASK: Plant the crystals.";

                        questUp = 1;
                        questDown = 0;
                    }

            }

            /*std::cout << camera.getCameraPosition().x << " " << camera.getCameraPosition().y <<
                " " << camera.getCameraPosition().z << '\n';*/

            if (checkEligibility(camera.getCameraPosition(), -70, 100, 65, 65, 320, 370) && quest6)
            {
                if (!quest2 && !quest3 && !quest4 && !quest5)
                    instruction = "INSTRUCTION: Press 1 and 2 to plant the crystals.";
                if ((-1.0f <= camera.getCameraViewDirection().x && camera.getCameraViewDirection().x <= 0.6f)
                    && (0.0f <= camera.getCameraViewDirection().z && camera.getCameraViewDirection().z <= 1.0f))
                {
                    if (window.isPressed(GLFW_KEY_1))
                    {
                        checkSpawn1 = 1;
                    }

                    if (window.isPressed(GLFW_KEY_2))
                    {
                        checkSpawn2 = 1;
                    }

                    if (checkSpawn1 && checkSpawn2)
                    {
                        quest6 = 0;
                        text = "TASK: Travel through the portal.";
                        instruction = "INSTRUCTION: PRESS E to travel.";
                    }
                }
            }


                if (checkSpawn1)
                {
                    ModelMatrix = glm::mat4(1.0);

                    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(70.0f, 25.0f, 440.0f));
                    //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                    //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));

                    MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

                    redcrys.draw(shader);
                }

                if (checkSpawn2)
                {
                    ModelMatrix = glm::mat4(1.0);

                    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-40.0f, 25.0f, 390.0f));
                    //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                    //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));

                    MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

                    bluecrys.draw(shader);
                }

                if (!quest2 && !quest3 && !quest4 && !quest5 && !quest6 && window.isPressed(GLFW_KEY_E) && gameLevel == 1)
                {
                    gameLevel = 2;
                    camera.setCameraPosition(glm::vec3(-2.05f, 35.0f, 663.4f));
                    text = "";
                    instruction = "";
                }

            
        }


            if (gameLevel == 2)
            {
                GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
                GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");
                
                if (dialogue6)
                {
                    dialogue = "INTERCOM: Welcome back, Captain! Good to see you again. You are\nexpected on planet surface.";
                    instruction = "INSTRUCTION: Press ENTER to continue.";
                }

                if (window.isPressed(GLFW_KEY_ENTER))
                {
                    dialogue6 = 0;
                    dialogue = "";
                    instruction = "";
                }

                if (!dialogue6 && !cinematic)
                    text = "TASK: Approach your ship.";

                    //std::cout << cinematic << '\n';

                camera.setCameraPosition(glm::vec3(camera.getCameraPosition().x, 35, camera.getCameraPosition().z));
                planeShader.use();

                ModelMatrix = glm::mat4(1.0);

                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
                glUniform3f(glGetUniformLocation(planeShader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
                glUniform3f(glGetUniformLocation(planeShader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
                glUniform3f(glGetUniformLocation(planeShader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


                ModelMatrix = glm::mat4(1.0);
                ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

                corridor.draw(planeShader);

                shader.use();

                //...........clouds...........//

                ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-30.0f, -20.0f, 800.0f));
                //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
                MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
                glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
                glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
                glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

                cloud.draw(shader);

                if (!cinematic)
                {
                    //..........ship.............//
                    ModelMatrix = glm::mat4(1.0);
                    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -10.0f, -500.0f));
                    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.8f));
                    //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                    //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));
                    MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
                    glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
                    glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
                    glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

                    ship.draw(shader);
                }
                else
                {
                    //..........ship.............//
                    ModelMatrix = glm::mat4(1.0);
                    if (ship2.getPosition().z > -1000)
                        ship2.setPosition(ship2.getPosition() + glm::vec3(0, 0, -0.1));
                    else
                        ship2.setPosition(ship2.getPosition() + glm::vec3(0, 0.1, -0.1));
                    ModelMatrix = glm::translate(ModelMatrix, ship2.getPosition());
                    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.8f));
                    //std::cout << starship.getPosition().x << " " << starship.getPosition().y << " " <<starship.getPosition().z << '\n';
                    //ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));
                    MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
                    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
                    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
                    glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
                    glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
                    glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

                    ship.draw(shader);
                   
                }

                if (checkEligibility(camera.getCameraPosition(), -55, 55, 35, 35, -400, -330))
                {
                    if (!cinematic)
                    {
                        text = "TASK: Board your ship.";
                        instruction = "INSTRUCTION: Press E to board.";
                    }
                    
                    if (window.isPressed(GLFW_KEY_E))
                    {
                        text = "";
                        dialogue = "";
                        instruction = "";

                        cinematic = 1;
                    }
                }
            
            }

            ImGui::Begin("Quest Log");

            if (started)
            {
                // Text that appears in the window
                ImGui::Text(text.c_str());
                ImGui::Text("");

                //ImGui::Text(myDialogue.c_str());
                ImGui::TextColored(ImVec4(255, 0, 100, 255), myDialogue.c_str());

                //ImGui::Text(dialogue.c_str());
                ImGui::TextColored(ImVec4(0, 255, 0, 255), dialogue.c_str());
                ImGui::Text("");

                ImGui::Text(instruction.c_str());
                // Ends the window
            }

            if (ImGui::Button(buttonText.c_str()) && !started)
            {
                started = 1;// ImGUI window creation
                buttonText = "Playing...";
                blocked = 0;
            }

            ImGui::End();

            // Renders the ImGUI elements
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            window.update();
        }

        // Deletes all ImGUI instances
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
   
}

void processKeyboardInput()
{
	float cameraSpeed = 100 * deltaTime;
   
        if (!cinematic)
        {
            if (window.isPressed(GLFW_KEY_W))
                camera.keyboardMoveFront(cameraSpeed);
            if (window.isPressed(GLFW_KEY_S))
                camera.keyboardMoveBack(cameraSpeed);
            if (window.isPressed(GLFW_KEY_A))
                camera.keyboardMoveLeft(cameraSpeed);
            if (window.isPressed(GLFW_KEY_D))
                camera.keyboardMoveRight(cameraSpeed);
            if (gameLevel == 0)
            {
                if (window.isPressed(GLFW_KEY_R))
                    camera.keyboardMoveUp(cameraSpeed);
                if (window.isPressed(GLFW_KEY_F))
                    camera.keyboardMoveDown(cameraSpeed);
            }

            if (window.isPressed(GLFW_KEY_LEFT))
                camera.rotateOy(cameraSpeed);
            if (window.isPressed(GLFW_KEY_RIGHT))
                camera.rotateOy(-cameraSpeed);


            if (gameLevel == 0)
            {
                if (window.isPressed(GLFW_KEY_UP))
                    camera.rotateOx(cameraSpeed);
                if (window.isPressed(GLFW_KEY_DOWN))
                    camera.rotateOx(-cameraSpeed);
            }

            if (gameLevel == 1)
            {
                if (window.isPressed(GLFW_KEY_LEFT_CONTROL))
                    camera.setCameraPosition(glm::vec3(camera.getCameraPosition().x, 45.0f, camera.getCameraPosition().z));
                if (!window.isPressed(GLFW_KEY_LEFT_CONTROL))
                    camera.setCameraPosition(glm::vec3(camera.getCameraPosition().x, 65.0f, camera.getCameraPosition().z));

            }
        }
}

int checkCollision(glm::vec3 pos1, glm::vec3 pos2)
{
    int x = abs(abs(pos1.x) - abs(pos2.x));
    int y = abs(abs(pos1.y) - abs(pos2.y));
    int z = abs(abs(pos1.z) - abs(pos2.z));

    if (x <= 20 && y <= 100 && z <= 60)
        return 1;
    return 0;
}

int checkEligibility(glm::vec3 pos, int x1, int x2, int y1, int y2, int z1, int z2)
{
    if ((x1 <= pos.x && pos.x <= x2) && (y1 <= pos.y && pos.y <= y2)&& (z1 <= pos.z && pos.z <= z2))
        return 1;
    return 0;
}
