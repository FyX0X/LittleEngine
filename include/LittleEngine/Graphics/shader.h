#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <LittleEngine/error_logger.h>
#include <glm/glm.hpp>


namespace LittleEngine::Graphics
{


    class Shader
    {
    public:
        unsigned int id;
        // constructor generates the shader on the fly
        Shader();
        Shader(const std::string& vertex, const std::string& fragment, bool isPath = true);
        ~Shader();

        // creates the default shader
        static void Initialize();

        // sets opengl to use the shader
        void Use();
        
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetIntArray(const std::string& name, int size, const int* array) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2(const std::string& name, const glm::vec2& value) const;
        void SetVec2(const std::string& name, float x, float y) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec3(const std::string& name, float x, float y, float z) const;
        void SetVec4(const std::string& name, const glm::vec4& value) const;
        void SetVec4(const std::string& name, float x, float y, float z, float w) const;
        void SetMat2(const std::string& name, const glm::mat2& mat) const;
        void SetMat3(const std::string& name, const glm::mat3& mat) const;
        void SetMat4(const std::string& name, const glm::mat4& mat) const;

    private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        static void CheckCompileErrors(GLuint shader, const std::string& type);

        static GLuint CreateShaderFromFile(const std::string& vertexPath, const std::string& fragmentPath);
        static GLuint CreateShaderFromCode(const std::string& vertexCode, const std::string& fragmentCode);

        static GLuint s_defaultShader;

    };


}