#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include "stb_easy_font.h"

// Шейдеры для треугольника и текста (можно один и тот же)
const char* vertexShaderSource = "#version 330 core\n"
    "layout(location = 0) in vec2 pos;\n"
    "uniform vec2 offset;\n"
    "uniform vec2 scale;\n"
    "void main() {\n"
    "    vec2 p = pos * scale + offset;\n"
    "    gl_Position = vec4(p, 0.0, 1.0);\n"
    "}";

const char* fragmentShaderSource = "#version 330 core\n"
    "uniform vec3 color;\n"
    "out vec4 FragColor;\n"
    "void main() { FragColor = vec4(color, 1.0); }";

float triangle[] = {
     0.0f,  0.5f,
    -0.5f, -0.5f,
     0.5f, -0.5f
};

// src = исходный буфер stb_easy_font (каждые 16 байт — одна вершина, quad = 4 вершины = 64 байта)
// n = кол-во quads (вернула stb_easy_font_print)
// out = куда положить новые вершины (triangles): 6 вершин на quad, по 16 байт на вершину
void quads_to_tris(const char* src, int n, char* out) {
    for (int i = 0; i < n; ++i) {
        const char* v0 = src + i * 64 + 0 * 16;
        const char* v1 = src + i * 64 + 1 * 16;
        const char* v2 = src + i * 64 + 2 * 16;
        const char* v3 = src + i * 64 + 3 * 16;
        // Первый триугольник: v0, v1, v2
        memcpy(out + (i * 6 + 0) * 16, v0, 16);
        memcpy(out + (i * 6 + 1) * 16, v1, 16);
        memcpy(out + (i * 6 + 2) * 16, v2, 16);
        // Второй триугольник: v0, v2, v3
        memcpy(out + (i * 6 + 3) * 16, v0, 16);
        memcpy(out + (i * 6 + 4) * 16, v2, 16);
        memcpy(out + (i * 6 + 5) * 16, v3, 16);
    }
}


int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Core Test", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // === Компилируем шейдеры ===
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL); glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL); glCompileShader(fs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs); glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);

    // === Треугольник VAO/VBO ===
    GLuint vao_tri, vbo_tri;
    glGenVertexArrays(1, &vao_tri);
    glGenBuffers(1, &vbo_tri);
    glBindVertexArray(vao_tri);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tri);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- Буфер для текста ---
    char text[] = "COURSE WORK";
    char text_quads[9999];
    int num_quads = stb_easy_font_print(0, 0, text, NULL, text_quads, sizeof(text_quads));
    printf("num_quads = %d\n", num_quads); // отладка
    // stb_easy_font вернет вершины в экранных пикселях (левый нижний угол — (0,0))
    char text_tris[6 * 16 * 512]; // запас на 512 quad-символов (6 вершин по 16 байт)
    quads_to_tris(text_quads, num_quads, text_tris);
    int num_tris = num_quads * 2; // 2 треугольника на quad, 3 вершины на треугольник
    printf("num_tris = %d\n", num_tris);

    float text_scale = 4.0f;

    for (int i = 0; i < num_tris * 3; ++i) {
        float* v = (float*)(text_tris + i * 16);
        v[0] *= text_scale;  // x
        v[1] *= text_scale;  // y
    }

    int len = strlen(text);
    float text_w = len * 8 * text_scale;
    float text_h = 13 * text_scale;
    float w = 800.0f, h = 600.0f;
    float px = (w - text_w) / 2.0f;
    float py = (h + text_h) / 2.0f; // по вертикали — чтобы текст был ровно по центру

    // sx, sy:
    float sx = 2.0f / w, sy = 2.0f / h;

    // --- VAO/VBO для текста ---
    GLuint vao_txt, vbo_txt;
    glGenVertexArrays(1, &vao_txt);
    glGenBuffers(1, &vbo_txt);
    glBindVertexArray(vao_txt);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_txt);
    glBufferData(GL_ARRAY_BUFFER, num_tris * 3 * 16, text_tris, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);

        // --- Треугольник ---
        glUniform3f(glGetUniformLocation(prog, "color"), 1.0, 0.6, 0.1);
        glUniform2f(glGetUniformLocation(prog, "offset"), 0.0, 0.0);
        glUniform2f(glGetUniformLocation(prog, "scale"), 1.0, 1.0);
        glBindVertexArray(vao_tri);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // --- Текст ---
        glUniform3f(glGetUniformLocation(prog, "color"), 1.0, 1.0, 1.0); // Белый
         // размер окна
        float w = 800.0f, h = 600.0f;
        // масштаб: 1 пиксель в нормализованные координаты
        float sx = 2.0f / w, sy = 2.0f / h;
        glUniform2f(glGetUniformLocation(prog, "offset"), -1.0f + px * sx, 1.0f - py * sy);
        glUniform2f(glGetUniformLocation(prog, "scale"), sx, -sy);
        glBindVertexArray(vao_txt);
        glDrawArrays(GL_TRIANGLES, 0, num_tris * 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
