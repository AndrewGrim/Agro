#ifndef SHADER_HPP
    #define SHADER_HPP

    #include <string>
    #include <fstream>
    #include <sstream>
    #include <iostream>

    #include "glad.h"
    #include "../util.hpp"

    class Shader {
        public:
            unsigned int ID;

            Shader() {

            }

            Shader(const char *vertext_shader, const char *fragment_shader) {
                compile(vertext_shader, fragment_shader);
            }

            void compile(const char *vertext_shader, const char *fragment_shader) {
                unsigned int vertex, fragment;

                vertex = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertex, 1, &vertext_shader, NULL);
                glCompileShader(vertex);
                checkCompileErrors(vertex, "VERTEX");

                fragment = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragment, 1, &fragment_shader, NULL);
                glCompileShader(fragment);
                checkCompileErrors(fragment, "FRAGMENT");

                ID = glCreateProgram();
                glAttachShader(ID, vertex);
                glAttachShader(ID, fragment);
                glLinkProgram(ID);
                checkCompileErrors(ID, "PROGRAM");

                glDeleteShader(vertex);
                glDeleteShader(fragment);
            }

            void use() {
                glUseProgram(ID);
            };

            void setBool(const std::string &name, bool value) const {
                glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
            };

            void setInt(const std::string &name, int value) const {
                glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
            };

            void setFloat(const std::string &name, float value) const {
                glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
            };

            void setVector2f(const char *name, float x, float y) {
                glUniform2f(glGetUniformLocation(this->ID, name), x, y);
            }

            void setVector3f(const char *name, float x, float y, float z) {
                glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
            }

            void setVector4f(const char *name, Color &color) {
                glUniform4f(glGetUniformLocation(this->ID, name), color.r, color.g, color.b, color.a);
            }

            void setVector4f(const char *name, float x, float y, float z, float w) {
                glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
            }

            void setMatrix4(const char *name, const float *matrix) {
                glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, matrix);
            }

            void checkCompileErrors(unsigned int shader, std::string type) {
                int success;
                char infoLog[1024];
                if (type != "PROGRAM")
                {
                    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                    if (!success)
                    {
                        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                    }
                }
                else
                {
                    glGetProgramiv(shader, GL_LINK_STATUS, &success);
                    if (!success)
                    {
                        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                    }
                }
            }
    };
#endif
