#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm.hpp>
#include <ext/matrix_transform.hpp>
#include <ext.hpp>
#include <math.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"

const unsigned int width = 800;
const unsigned int height = 800;


GLfloat vertices[] =
{    //Cords             //Colors               //TexCoords
    -0.5f, 0.0f, 0.5f,   0.83f, 0.70f, 0.44f,  0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,  5.0f, 0.0f,
     0.5f, 0.0f, -0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,
     0.5f, 0.0f, 0.5f,   0.83f, 0.70f, 0.44f,  5.0f, 0.0f,
     0.0f, 0.8f, 0.0f,   0.92f, 0.86f, 0.76f,  2.5f, 5.0f
};

//adds hole in triangle aka zelda symbol
GLuint indices[] =
{
      0, 1, 2,
      0, 2, 3,
      0, 1, 4,
      1, 2, 4,
      2, 3, 4,
      3, 0, 4
};

int main(void)
{   
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //Let GLFW aware of our OpenGL Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    //Tell GLFW we're using CORE profile, aka only the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "OpenGL Test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    GLenum err = gladLoadGL();
    glViewport(0, 0, width, height);
    Shader shaderProgram("default.vert", "default.frag");
    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    //Texture jam("jFlag.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    //jam.texUnit(shaderProgram, "tex0", 0);

    Texture jam("bBrick.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    jam.texUnit(shaderProgram, "tex0", 0);
   
    float rotation = 0.0f;
    double prevTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //Specify color of background
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        //clean the back buffer and assign new color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.Activate();

        double crntTime = glfwGetTime();
        if (crntTime - prevTime >= 1 / 60)
        {
            rotation += 0.5f;
            prevTime = crntTime;
        }

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
        proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        glUniform1f(uniID, 0.5f);
        jam.Bind();
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    //Delete all
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    jam.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}