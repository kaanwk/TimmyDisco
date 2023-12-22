#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>

#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte* pixels = new GLubyte[totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id++;
    delete[] pixels;
    fout.flush();
    fout.close();
}

//key board control
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window)
{
    //press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //press p to capture screen
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("ss", buffer_width, buffer_height);
    }
}

// obj file
std::string obj_path = "asset/floor.obj";
std::string obj_path2 = "asset/bucket.obj";
std::string obj_path3 = "asset/timmy.obj";

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "A3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("default.vert", "default.frag");

    glm::vec3 spotlightPositions[] = {
        glm::vec3( 0.0f,  200.0f,  0.0f),
        glm::vec3( 0.0f,  200.0f,  0.0f),
        glm::vec3( 0.0f,  200.0f,  0.0f)
    };

    glm::vec3 spotlightDirections[] = {
        glm::vec3(50.0f, -200.0f, -50.0f),
        glm::vec3(-50.0f, -200.0f, -50.0f),
        glm::vec3(0.0f, -200.0f, 50.0f)
    };

    // load mesh 1
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    // tinyobj load obj
    std::string warn, err;
    bool bTriangulate = true;
    bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path.c_str(), nullptr, bTriangulate);
    if (!bSuc)
    {
        std::cout << "tinyobj error: " << err.c_str() << std::endl;
        return -1;
    }

    // load mesh 2
    tinyobj::attrib_t attrib2;
    std::vector<tinyobj::shape_t> shapes2;
    std::vector<tinyobj::material_t> materials2;
    // tinyobj load obj
    std::string warn2, err2;
    bool bTriangulate2 = true;
    bool bSuc2 = tinyobj::LoadObj(&attrib2, &shapes2, &materials2, &warn2, &err2, obj_path2.c_str(), nullptr, bTriangulate2);
    if (!bSuc2)
    {
        std::cout << "tinyobj error: " << err2.c_str() << std::endl;
        return -1;
    }

    // load mesh 3
    tinyobj::attrib_t attrib3;
    std::vector<tinyobj::shape_t> shapes3;
    std::vector<tinyobj::material_t> materials3;
    // tinyobj load obj
    std::string warn3, err3;
    bool bTriangulate3 = true;
    bool bSuc3 = tinyobj::LoadObj(&attrib3, &shapes3, &materials3, &warn3, &err3, obj_path3.c_str(), nullptr, bTriangulate3);
    if (!bSuc3)
    {
        std::cout << "tinyobj error: " << err3.c_str() << std::endl;
        return -1;
    }

    // load and create texture

    unsigned int textureT, texture2, texture3;

    // texture 1
    glGenTextures(1, &textureT);
    glBindTexture(GL_TEXTURE_2D, textureT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("asset/floor.jpeg", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);//don’t forget to release the image data

    // texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data2 = stbi_load("asset/bucket.jpg", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    stbi_image_free(data2);//don’t forget to release the image data

    // texture 3
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data3 = stbi_load("asset/timmy.png", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
    stbi_image_free(data3);//don’t forget to release the image data

    std::vector<float> vbufferT;
    std::vector<float> nbufferT;
    std::vector<float> tbufferT;

    std::vector<float> vbuffer2;
    std::vector<float> nbuffer2;
    std::vector<float> tbuffer2;

    std::vector<float> vbuffer3;
    std::vector<float> nbuffer3;
    std::vector<float> tbuffer3;

// load texture coordinates mesh 1
for (const auto& shape : shapes) {
    for (auto id : shape.mesh.indices) {
        int vid = id.vertex_index;
        int nid = id.normal_index;
        int tid = id.texcoord_index;
        // fill in vertex positions
        vbufferT.push_back(attrib.vertices[vid * 3]);
        vbufferT.push_back(attrib.vertices[vid * 3 + 1]);
        vbufferT.push_back(attrib.vertices[vid * 3 + 2]);
        // normal
        nbufferT.push_back(attrib.normals[nid * 3]);
        nbufferT.push_back(attrib.normals[nid * 3 + 1]);
        nbufferT.push_back(attrib.normals[nid * 3 + 2]);
        // tex coord
        tbufferT.push_back(attrib.texcoords[tid * 2]);
        tbufferT.push_back(attrib.texcoords[tid * 2 + 1]);
    }
}

// load texture coordinates mesh 2
for (const auto& shape : shapes2) {
    for (auto id : shape.mesh.indices) {
        int vid = id.vertex_index;
        int nid = id.normal_index;
        int tid = id.texcoord_index;
        // fill in vertex positions
        vbuffer2.push_back(attrib2.vertices[vid * 3]);
        vbuffer2.push_back(attrib2.vertices[vid * 3 + 1]);
        vbuffer2.push_back(attrib2.vertices[vid * 3 + 2]);
        // normal
        nbuffer2.push_back(attrib2.normals[nid * 3]);
        nbuffer2.push_back(attrib2.normals[nid * 3 + 1]);
        nbuffer2.push_back(attrib2.normals[nid * 3 + 2]);
        // tex coord
        tbuffer2.push_back(attrib2.texcoords[tid * 2]);
        tbuffer2.push_back(attrib2.texcoords[tid * 2 + 1]);
    }
}

// load texture coordinates mesh 3
for (const auto& shape : shapes3) {
    for (auto id : shape.mesh.indices) {
        int vid = id.vertex_index;
        int nid = id.normal_index;
        int tid = id.texcoord_index;
        // fill in vertex positions
        vbuffer3.push_back(attrib3.vertices[vid * 3]);
        vbuffer3.push_back(attrib3.vertices[vid * 3 + 1]);
        vbuffer3.push_back(attrib3.vertices[vid * 3 + 2]);
        // normal
        nbuffer3.push_back(attrib3.normals[nid * 3]);
        nbuffer3.push_back(attrib3.normals[nid * 3 + 1]);
        nbuffer3.push_back(attrib3.normals[nid * 3 + 2]);
        // tex coord
        tbuffer3.push_back(attrib3.texcoords[tid * 2]);
        tbuffer3.push_back(attrib3.texcoords[tid * 2 + 1]);
    }
}

    // mesh 1
    unsigned int vertexVBO, normalVBO, tcoordVBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &tcoordVBO);

    // mesh 2
    unsigned int vertexVBO2, normalVBO2, tcoordVBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &vertexVBO2);
    glGenBuffers(1, &normalVBO2);
    glGenBuffers(1, &tcoordVBO2);

    // mesh 3
    unsigned int vertexVBO3, normalVBO3, tcoordVBO3, VAO3;
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &vertexVBO3);
    glGenBuffers(1, &normalVBO3);
    glGenBuffers(1, &tcoordVBO3);

    // load mesh 1
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vbufferT.size() * sizeof(float), &vbufferT[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, nbufferT.size() * sizeof(float), &nbufferT[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, tcoordVBO);
    glBufferData(GL_ARRAY_BUFFER, tbufferT.size() * sizeof(float), &tbufferT[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // load mesh 2
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO2);
    glBufferData(GL_ARRAY_BUFFER, vbuffer2.size() * sizeof(float), &vbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO2);
    glBufferData(GL_ARRAY_BUFFER, nbuffer2.size() * sizeof(float), &nbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    //texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, tcoordVBO2);
    glBufferData(GL_ARRAY_BUFFER, tbuffer2.size() * sizeof(float), &tbuffer2[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);


    // load mesh 3
    glBindVertexArray(VAO3);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO3);
    glBufferData(GL_ARRAY_BUFFER, vbuffer3.size() * sizeof(float), &vbuffer3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO3);
    glBufferData(GL_ARRAY_BUFFER, nbuffer3.size() * sizeof(float), &nbuffer3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    //texture coord attribute
    glBindBuffer(GL_ARRAY_BUFFER, tcoordVBO3);
    glBufferData(GL_ARRAY_BUFFER, tbuffer3.size() * sizeof(float), &tbuffer3[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    
    // render loop
    // -----------
    ourShader.use();
    ourShader.setInt("textureT", 0);
    ourShader.setInt("texture2", 1);
    ourShader.setInt("texture3", 2);

    //ourShader.setVec3("diffuseColor", 1.0f, 1.0f, 1.0f); //diffuseColor
    //ourShader.setVec3("ambientColor", 0.2f, 0.2f, 0.2f); //ambientColor

    // kc, kl, kq
    ourShader.setFloat("kc", 1.0f);
    ourShader.setFloat("kl", 0.35 * 1e-4);
    ourShader.setFloat("kq", 0.44 * 1e-4);
    
    
    float theta = 0.0f;



    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        //glm::vec3 lightPos(sin(theta) * 150, 100, cos(theta) * 150);
        //theta += 0.0f;
        //ourShader.setVec3("lightPos", lightPos);

        // set spotlight 0
        theta += 0.05f;
        glm::vec3 position0(sin(theta) * 150, 200, cos(theta) * 150);

        ourShader.setVec3("spotlights[0].position", position0);
        //ourShader.setVec3("spotlights[0].position", spotlightPositions[0]);
        ourShader.setVec3("spotlights[0].direction", spotlightDirections[0]);
        ourShader.setFloat("spotlights[0].cutoff", glm::cos(M_PI/6.0f));
        ourShader.setVec3("spotlights[0].diffuseColor", 1.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotlights[0].ambientColor", 0.2f, 0.2f, 0.2f);

        // set spotlight 1
        glm::vec3 position1(sin(theta) * 150, 200, cos(theta) * 150);
        ourShader.setVec3("spotlights[1].position", position1);
        //ourShader.setVec3("spotlights[1].position", spotlightPositions[1]);
        ourShader.setVec3("spotlights[1].direction",spotlightDirections[1]);
        ourShader.setFloat("spotlights[1].cutoff", glm::cos(M_PI/6.0f));
        ourShader.setVec3("spotlights[1].diffuseColor", 0.0f, 1.0f, 0.0f);
        ourShader.setVec3("spotlights[1].ambientColor", 0.2f, 0.2f, 0.2f);

        // set spotlight 2
        glm::vec3 position2(sin(theta) * 150, 200, cos(theta) * 150);
        ourShader.setVec3("spotlights[2].position", position2);
        //ourShader.setVec3("spotlights[2].position", spotlightPositions[2]);
        ourShader.setVec3("spotlights[2].direction", spotlightDirections[2]);
        ourShader.setFloat("spotlights[2].cutoff", glm::cos(M_PI/6.0f));
        ourShader.setVec3("spotlights[2].diffuseColor", 0.0f, 0.0f, 1.0f);
        ourShader.setVec3("spotlights[2].ambientColor", 0.2f, 0.2f, 0.2f);


        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureT);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);

        // activate shader
       // ourShader.use();
       // ourShader.setInt("textureT",0);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        view = glm::lookAt(glm::vec3(50, 100, 200), glm::vec3(0, 80, 0), glm::vec3(0, 1, 0)); // Camera is at (0,0,10), in World Space
        ourShader.setMat4("view", view);

        projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f); // Camera is at (0,0,10), in World Space
        ourShader.setMat4("projection", projection);

        // render boxes
        glUniform1i(glGetUniformLocation(ourShader.ID, "shapeID"), 0); // Set shape ID for th
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vbufferT.size() / 3);

        //ourShader.use();
       // ourShader.setInt("texture2", 1);
        glUniform1i(glGetUniformLocation(ourShader.ID, "shapeID"), 1); // Set shape ID for the first mesh
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer2.size() / 3);

        glUniform1i(glGetUniformLocation(ourShader.ID, "shapeID"), 2); // Set shape ID for the first mesh
        glBindVertexArray(VAO3);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer3.size() / 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertexVBO);
    glDeleteBuffers(1, &normalVBO);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &vertexVBO2);
    glDeleteBuffers(1, &normalVBO2);

    glDeleteVertexArrays(1, &VAO3);
    glDeleteBuffers(1, &vertexVBO3);
    glDeleteBuffers(1, &normalVBO3);



    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}