#include "LittleEngine/Graphics/shader.h"



#include <fstream>
#include <sstream>
#include <iostream>
#include <LittleEngine/Utils/logger.h>

namespace LittleEngine::Graphics
{

#pragma region default shaders

    GLuint Shader::s_defaultShader = 0;

    const std::string defaultVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec4 aColor;
        layout (location = 3) in float aTexIndex;

        out vec2 vTexCoord;
        out vec4 vColor;
        flat out int vTexIndex;

        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * vec4(aPos, 0.0, 1.0);
            vTexCoord = aTexCoord;
            vColor = aColor;
            vTexIndex = int(aTexIndex);
        } 
    )";

    const std::string old_defaultFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 vTexCoord;
        in vec4 vColor;
        flat in int vTexIndex;

        uniform sampler2D uTextures[16];

        void main()
        {
            int index = int(vTexIndex);
            FragColor = vColor * texture(uTextures[index], vTexCoord);
        }
    )";

    const std::string defaultFragmentShader = R"(
    #version 330 core
    out vec4 FragColor;

    in vec2 vTexCoord;
    in vec4 vColor;
    flat in int vTexIndex;

    uniform sampler2D uTex0;
    uniform sampler2D uTex1;
    uniform sampler2D uTex2;
    uniform sampler2D uTex3;
    uniform sampler2D uTex4;
    uniform sampler2D uTex5;
    uniform sampler2D uTex6;
    uniform sampler2D uTex7;
    uniform sampler2D uTex8;
    uniform sampler2D uTex9;
    uniform sampler2D uTex10;
    uniform sampler2D uTex11;
    uniform sampler2D uTex12;
    uniform sampler2D uTex13;
    uniform sampler2D uTex14;
    uniform sampler2D uTex15;

    void main()
    {
        vec4 texColor;
        if (vTexIndex == 0) texColor = texture(uTex0, vTexCoord);
        else if (vTexIndex == 1) texColor = texture(uTex1, vTexCoord);
        else if (vTexIndex == 2) texColor = texture(uTex2, vTexCoord);
        else if (vTexIndex == 3) texColor = texture(uTex3, vTexCoord);
        else if (vTexIndex == 4) texColor = texture(uTex4, vTexCoord);
        else if (vTexIndex == 5) texColor = texture(uTex5, vTexCoord);
        else if (vTexIndex == 6) texColor = texture(uTex6, vTexCoord);
        else if (vTexIndex == 7) texColor = texture(uTex7, vTexCoord);
        else if (vTexIndex == 8) texColor = texture(uTex8, vTexCoord);
        else if (vTexIndex == 9) texColor = texture(uTex9, vTexCoord);
        else if (vTexIndex == 10) texColor = texture(uTex10, vTexCoord);
        else if (vTexIndex == 11) texColor = texture(uTex11, vTexCoord);
        else if (vTexIndex == 12) texColor = texture(uTex12, vTexCoord);
        else if (vTexIndex == 13) texColor = texture(uTex13, vTexCoord);
        else if (vTexIndex == 14) texColor = texture(uTex14, vTexCoord);
        else if (vTexIndex == 15) texColor = texture(uTex15, vTexCoord);
        else texColor = vec4(1.0, 0.0, 1.0, 1.0); // fallback magenta

        FragColor = vColor * texColor;
    }
)";


#pragma endregion

#pragma region Initialization / lifetime management
    

    Shader::~Shader()
    {
		Cleanup();
    }

    void Shader::Create(const std::string& vertex, const std::string& fragment, bool isPath)
    {
        if (id != 0)
        {
            Utils::Logger::Warning("Shader::Create : Shader was already created.");
            return;
        }
        if (isPath)
        {
            id = CreateShaderFromFile(vertex, fragment);
        }
        else
        {
            id = CreateShaderFromCode(vertex, fragment);
        }
	}

    void Shader::CreateDefault()
    {
        if (s_defaultShader == 0)
        {
            Utils::Logger::Warning("Shader constructor : Shader was not initialized.");
            Initialize();
        }
        id = s_defaultShader;
	}

    void Shader::Cleanup()
    {
        if (id != 0)
        {
            glDeleteProgram(id);
            id = 0;
        }
        else
        {
            Utils::Logger::Warning("Shader::Cleanup : Shader was not created.");
        }
	}

    void Shader::Initialize()
    {
        if (s_defaultShader != 0)
        {
            Utils::Logger::Warning("Shader::Initialize : Shader was already initialized.");
            return;
        }

#if DEVELOPMENT_BUILD == 1
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
#endif
        s_defaultShader = CreateShaderFromCode(defaultVertexShader, defaultFragmentShader);

    }

#pragma endregion

#pragma region Uniform Binding

    // activate the shader
    // ------------------------------------------------------------------------
    void Shader::Use()
    {
        if (id == 0)
        {
            Utils::Logger::Critical("Shader::Use : Shader was not created. Call Create() first.");
        }
        glUseProgram(id);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void Shader::SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
    }
    // ------------------------------------------------------------------------
    void Shader::SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }    
    // ------------------------------------------------------------------------
    void Shader::SetIntArray(const std::string& name, int size, const int* array) const
    {
        glUniform1iv(glGetUniformLocation(id, name.c_str()), size, array);
    }
    // ------------------------------------------------------------------------
    void Shader::SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

#pragma endregion

#pragma region Helper

    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void Shader::CheckCompileErrors(GLuint shader, const std::string& type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);

                std::stringstream ss;
                ss << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog;
                Utils::Logger::Critical(ss.str());
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);

                std::stringstream ss;
                ss << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog;
                Utils::Logger::Critical(ss.str());
            }
        }
    }

#pragma endregion

#pragma region Shader Loading / Compiling

    GLuint Shader::CreateShaderFromFile(const std::string& vertexPath, const std::string& fragmentPath)
    {

        std::string vertexCode;
        std::string fragmentCode;

        // 1. retrieve the vertex/fragment source code from filePath

        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            Utils::Logger::Critical("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " + std::string(e.what()));
        }

        return CreateShaderFromCode(vertexCode, fragmentCode);

    }

    GLuint Shader::CreateShaderFromCode(const std::string& vertexCode, const std::string& fragmentCode)
    {
        GLuint id = 0;

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // 2. compile shaders
        GLuint vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");
        // shader Program
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        CheckCompileErrors(id, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);


        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            Utils::Logger::Error("OpenGL Error in Shader::CreateShaderFromCode - Code: " + std::to_string(err));
        }

        return id;
    }

#pragma endregion

}