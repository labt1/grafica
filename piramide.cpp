#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <./stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Dimensiones de la ventana
const int WIDTH = 800;
const int HEIGHT = 600;

// Vértices de la pirámide

GLfloat vertices[] = {
    // Coordenadas         // Coordenadas de textura
    // Cara frontal
    -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,  // V0
    0.5f, -0.5f, 0.5f,     1.0f, 0.0f,  // V1
    0.0f, 0.5f, 0.0f,      0.5f, 1.0f,  // V2

    // Cara derecha
    0.5f, -0.5f, 0.5f,     0.0f, 0.0f,  // V3
    0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  // V4
    0.0f, 0.5f, 0.0f,      0.5f, 1.0f,  // V5

    // Cara trasera
    0.5f, -0.5f, -0.5f,    0.0f, 0.0f,  // V6
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  // V7
    0.0f, 0.5f, 0.0f,       0.5f, 1.0f,  // V8

    // Cara izquierda
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // V9
    -0.5f, -0.5f, 0.5f,    1.0f, 0.0f,  // V10
    0.0f, 0.5f, 0.0f,       0.5f, 1.0f,  // V11

    // Cara inferior
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,  // V12
    -0.5f, -0.5f, 0.5f,    1.0f, 0.0f,  // V13
    0.5f, -0.5f, 0.5f,     1.0f, 1.0f,  // V14
    0.5f, -0.5f, -0.5f,    0.0f, 1.0f   // V15
};

GLuint indices[] = {
    0, 1, 2,  // Cara frontal
    3, 4, 5,  // Cara derecha
    6, 7, 8,  // Cara trasera
    9, 10, 11,  // Cara izquierda
    12, 13, 14,  // Cara inferior 1
    14, 15, 12,  // Cara inferior 2
};

// Función para cargar una imagen como textura
GLuint loadTexture(const char* file_path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, numComponents;
    unsigned char* image = stbi_load(file_path, &width, &height, &numComponents, 0);
    if (image == nullptr) {
        std::cerr << "Error al cargar la imagen: " << file_path << std::endl;
        return 0;
    }

    GLenum format;
    if (numComponents == 1)
        format = GL_RED;
    else if (numComponents == 3)
        format = GL_RGB;
    else if (numComponents == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

// Función para procesar las entradas del teclado
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Matrices de transformación
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

// Ángulos de rotación en los ejes
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W && action == GLFW_REPEAT)
        angleX += 5.0f;
    if (key == GLFW_KEY_S && action == GLFW_REPEAT)
        angleX -= 5.0f;
    if (key == GLFW_KEY_A && action == GLFW_REPEAT)
        angleY += 5.0f;
    if (key == GLFW_KEY_D && action == GLFW_REPEAT)
        angleY -= 5.0f;
    if (key == GLFW_KEY_UP && action == GLFW_REPEAT)
        angleZ += 5.0f;
    if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)
        angleZ -= 5.0f;
}

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurar GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear una ventana GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Pirámide con imágenes", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Hacer que la ventana GLFW sea el contexto actual
    glfwMakeContextCurrent(window);

    // Configurar la función de callback para el teclado
    glfwSetKeyCallback(window, keyCallback);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error al inicializar GLAD" << std::endl;
        return -1;
    }

    // Crear el programa de shaders
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "    TexCoord = aTexCoord;\n"
        "}\n";

    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D texture1;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(texture1, TexCoord);\n"
        "}\n";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Crear los objetos de la pirámide
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Posiciones de los vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Coordenadas de textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Cargar las texturas
    GLuint texture1 = loadTexture("C:/Users/labt_/OneDrive/Documentos/Grafica/Texturas/sticker-1.jpg");
    GLuint texture2 = loadTexture("C:/Users/labt_/OneDrive/Documentos/Grafica/Texturas/sticker-2.jpg");
    GLuint texture3 = loadTexture("C:/Users/labt_/OneDrive/Documentos/Grafica/Texturas/sticker-3.jpg");
    GLuint texture4 = loadTexture("C:/Users/labt_/OneDrive/Documentos/Grafica/Texturas/sticker-4.jpg");
    //GLuint texture5 = loadTexture("C:/Users/labt_/OneDrive/Documentos/Grafica/Texturas/sticker-5.jpg");

    // Habilitar la prueba de profundidad
    glEnable(GL_DEPTH_TEST);

    // Ciclo principal de renderizado
    while (!glfwWindowShouldClose(window)) {
        // Procesar entradas del teclado
        processInput(window);


        // Limpiar el buffer de color y el buffer de profundidad
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Calcular las matrices de transformación
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));

        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);


        // Usar el programa de shaders
        glUseProgram(shaderProgram);


        // Pasar las matrices de transformación al shader
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        // Dibujar la pirámide
        glBindVertexArray(VAO);

        // Cara frontal
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Cara derecha
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(GLuint)));

        // Cara trasera
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

        // Cara izquierda
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 3);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(GLuint)));

        // Cara inferior
        glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_2D, texture5);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 4);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));


        glBindVertexArray(0);

        // Intercambiar los buffers y revisar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberar recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
