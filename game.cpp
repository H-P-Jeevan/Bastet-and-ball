#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iterator>
#include <list>
using namespace std;

// window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int key_presseed = 0;
float position = 0.0f;

// vertex Shader
const char* vertexShaderSource = "#version 330 core\n"                        // version
"layout (location = 0) in vec3 aPos;\n"      // vertex data pointer
"layout (location = 1) in vec2 atexCoord;\n" // texture pointer
"out vec2 TexCoord;\n"                       // for fragment shader
"uniform mat4 transform;\n"                  // object movement
"uniform mat4 model;\n"                      // coordinate view
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0f);\n" // vertex shader output
"   TexCoord = atexCoord;\n"
"}\0";
// fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"           // fragment shader output
"in vec2 TexCoord;\n"             // texture coordinates
"uniform sampler2D ourTexture;\n" // texture
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture, TexCoord) * vec4(0.5f, 0.0f, 0.0f, 1.0f); \n" // fragment shader output
"}\n\0";

// vertex Shader 2
const char* vertexShaderSource2 = "#version 330 core\n"                     // version
"layout (location = 0) in vec2 aPos;\n"   // vertex data pointer
"layout (location = 1) in vec3 aColor;\n" // color pointer
"out vec3 ourColor;\n"                    // for fragment shader
"uniform mat4 transform;\n"               // object movement
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 0.0f, 1.0f);\n" // vertex shader output
"   ourColor = aColor;\n"
"}\0";

// fragment Shader 2
const char* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 FragColor;\n" // fragment shader output
"in vec3 ourColor;\n"   // color values
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0); \n" // fragment shader output
"}\n\0";

class snow
{
private:
    float size = 0.02f;
    float hyp = sqrt(size * size * 3 / 4);

public:
    unsigned int VBO, VAO, EBO;
    float vertices[30];
    unsigned int indices[12];
    void create_VAO(float x, float color);
    void delete_VAO();
    float y = 0.0f;
};

void snow::create_VAO(float x, float color)
{
    // define vetices and index
    float vertices[30] = {
        // positions  // color
        size / 2 + x, hyp + 0.9, color, color, color,   // top right
        size / 2 + x, -hyp + 0.9, color, color, color,  // bottom right
        -size / 2 + x, hyp + 0.9, color, color, color,  // top left
        -size / 2 + x, -hyp + 0.9, color, color, color, // bottom left
        -size + x, 0.9, color, color, color,            // left
        size + x, 0.9, color, color, color              // right
    };
    unsigned int indices[12] = {
        0, 2, 4,
        1, 3, 5,
        4, 5, 0,
        4, 5, 3 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);                                                          // bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                              // bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);       // pass the vetices and frequency of drawing
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);                                      // bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // pass the indices and frequency of drawing
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);   // vertex attrib pointes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
void snow::delete_VAO()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // window size changing
void processInput(GLFWwindow* window);                                     // input from user

int main()
{
    // initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // creating window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL); // width, height, window name, null, null
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);                                    // set window to current context
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // callback function call

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // vertex shader object
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // create object of type GL_VERTEX_SHADER
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);   // attach the code to it
    glCompileShader(vertexShader);                                // compile
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // create object
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);   // attach the code
    glCompileShader(fragmentShader);                                  // compile
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram(); // create shader program object
    glAttachShader(shaderProgram, vertexShader);    // attach shaders
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // link
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << std::endl;
    }
    glDeleteShader(vertexShader); // delete shaders once shader program is created
    glDeleteShader(fragmentShader);

    // define vetices and index
    float vertices[] = {
        // positions         // texture
        0.1, -0.8f, 0.0f, 1.0f, 1.0f,   // top right
        0.1f, -0.9f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.1f, -0.9f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.1f, -0.8f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    // VBO, VAO, EBO
    unsigned int VBO, VAO, EBO; // create objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);                                                          // bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                              // bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);       // pass the vetices and frequency of drawing
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);                                      // bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // pass the indices and frequency of drawing
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);   // vertex attrib pointes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram); // start using the shader Program
    // texture define
    unsigned int texture; // create object
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);                        // bind texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // set filtering type for min and mag
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("D:\\OpenGL_learn\\OpenGL1\\wall.jpg", &width, &height, &nrChannels, 0); // load image
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // read image in RGB
        glGenerateMipmap(GL_TEXTURE_2D);                                                          // create mipmap
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // delete image data

    // vertex shader object 2
    unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER); // create object of type GL_VERTEX_SHADER
    glShaderSource(vertexShader2, 1, &vertexShaderSource2, NULL);  // attach the code to it
    glCompileShader(vertexShader2);                                // compile
    glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << std::endl;
    }
    // fragment shader 2
    unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER); // create object
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);  // attach the code
    glCompileShader(fragmentShader2);                                  // compile
    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram2 = glCreateProgram(); // create shader program object
    glAttachShader(shaderProgram2, vertexShader2);   // attach shaders
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2); // link
    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << std::endl;
    }
    glDeleteShader(vertexShader2); // delete shaders once shader program is created
    glDeleteShader(fragmentShader2);

    // define coordiante transformation matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    list<snow> all_gains, all_loss;
    list<snow>::iterator it;
    snow obj;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // clear the color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);                            // check depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // cleat color and depth data for every frame

        // activate the textures
        glActiveTexture(GL_TEXTURE0); // texture data stored at 0 is set to texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // use the shader Program
        glUseProgram(shaderProgram); // use the shader program

        // get locations of coordiante transformations and change them
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // create the transfomation matrix
        glm::mat4 trans = glm::mat4(1.0f);

        if (key_presseed == 1)
        {
            position -= 0.004;
            if (position < -0.9f)
                position = -0.9f;
        }
        else if (key_presseed == 2)
        {
            position += 0.004f;
            if (position > 0.9f)
                position = 0.9f;
        }
        trans = glm::translate(trans, glm::vec3(position, 0.0f, 0.0f));
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform"); // get location and change the transformation
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        // bind the VAO
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draw the triangles

        float rand_num = rand() % 100;
        float r = rand_num / 50 - 1;
        int randd = rand() % 100;
        if ((int)((float)glfwGetTime() * 100) % 91 == 0 && randd % 91 == 0)
        {
            all_gains.push_back(obj);
            all_gains.back().create_VAO(r, 1.0);
        }
        else if ((int)((float)glfwGetTime() * 100) % 99 == 0 && randd % 99 == 0)
        {
            all_loss.push_back(obj);
            all_loss.back().create_VAO(r, 0.0);
        }

        // use the shader Program 2
        glUseProgram(shaderProgram2); // use the shader program
        // create the transfomation matrix
        for (it = all_gains.begin(); it != all_gains.end(); ++it)
        {
            (*it).y -= 0.0005;
            glm::mat4 trans2 = glm::mat4(1.0f);
            trans2 = glm::translate(trans2, glm::vec3(0.0f, (*it).y, 0.0f));
            unsigned int transformLoc = glGetUniformLocation(shaderProgram2, "transform"); // get location and change the transformation
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

            // bind the VAO2
            glBindVertexArray((*it).VAO);
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0); // draw the triangles
        }
        for (it = all_loss.begin(); it != all_loss.end(); ++it)
        {
            (*it).y -= 0.0005;
            glm::mat4 trans2 = glm::mat4(1.0f);
            trans2 = glm::translate(trans2, glm::vec3(0.0f, (*it).y, 0.0f));
            unsigned int transformLoc = glGetUniformLocation(shaderProgram2, "transform"); // get location and change the transformation
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

            // bind the VAO2
            glBindVertexArray((*it).VAO);
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0); // draw the triangles
        }
        // cout << all_gains.size() << endl;
        if ((!all_gains.empty()) && all_gains.back().y < -1.85)
        {
            all_gains.back().delete_VAO();
            all_gains.pop_back();
        }
        if ((!all_loss.empty()) && all_loss.back().y < -1.85)
        {
            all_loss.back().delete_VAO();
            all_loss.pop_back();
        }
        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    obj.delete_VAO();

    // glfw: terminate
    glfwTerminate();
    return 0;
}

// process all input:
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        key_presseed = 1;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        key_presseed = 2;
    else
        key_presseed = 0;
}

// window size changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}