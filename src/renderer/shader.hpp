#ifndef SHADER_HPP
    #define SHADER_HPP

    #include <fstream>
    #include <sstream>
    #include <iostream>

    #include "glad.h"
    #include "../util.hpp"
    #include "../core/string.hpp"

    class Shader {
        public:
            u32 ID;

            Shader() {

            }

            Shader(const char *vertext_shader, const char *fragment_shader) {
                compile(vertext_shader, fragment_shader);
            }

            void compile(const char *vertext_shader, const char *fragment_shader) {
                u32 vertex, fragment;

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

            void setBool(const String &name, bool value) const {
                glUniform1i(glGetUniformLocation(ID, name.data()), (i32)value);
            };

            void setInt(const String &name, i32 value) const {
                glUniform1i(glGetUniformLocation(ID, name.data()), value);
            };

            void setFloat(const String &name, f32 value) const {
                glUniform1f(glGetUniformLocation(ID, name.data()), value);
            };

            void setVector2f(const char *name, f32 x, f32 y) {
                glUniform2f(glGetUniformLocation(this->ID, name), x, y);
            }

            void setVector3f(const char *name, f32 x, f32 y, f32 z) {
                glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
            }

            void setVector4f(const char *name, Color &color) {
                glUniform4f(glGetUniformLocation(this->ID, name), color.r, color.g, color.b, color.a);
            }

            void setVector4f(const char *name, f32 x, f32 y, f32 z, f32 w) {
                glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
            }

            void setMatrix4(const char *name, const f32 *matrix) {
                glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, matrix);
            }

            void checkCompileErrors(u32 shader, String type) {
                i32 success;
                char infoLog[1024];
                if (!(type == "PROGRAM"))
                {
                    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                    if (!success)
                    {
                        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type.data() << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                    }
                }
                else
                {
                    glGetProgramiv(shader, GL_LINK_STATUS, &success);
                    if (!success)
                    {
                        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type.data() << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                    }
                }
            }
    };
#endif
