#include "LittleEngine/Graphics/shader.h"


namespace LittleEngine::Graphics
{

#pragma region default shaders

    GLuint Shader::s_defaultShader = 0;

    const std::string defaultVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec4 aColor;
        layout (location = 3) in float aTexIndex;       // float because needs to be interpolated

        out vec2 vTexCoord;
        out vec4 vColor;
        out float vTexIndex;

        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * view * vec4(aPos, 0.0, 1.0);
            vTexCoord = aTexCoord;
            vColor = aColor;
            vTexIndex = aTexIndex;
        } 
    )";

    const std::string defaultFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 vTexCoord;
        in vec4 vColor;
        in float vTexIndex;

        uniform sampler2D uTextures[16];

        void main()
        {
            int index = int(vTexIndex);
            FragColor = vColor * texture(uTextures[index], vTexCoord);
        }
    )";

#pragma endregion

#pragma region Initialization / lifetime management

    Shader::Shader()
    {
        if (s_defaultShader == 0)
        {
            LogError("Shader constructor : Shader was not initialied.");
            Initialize();
        }
        id = s_defaultShader;
    }
    
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader::Shader(const std::string& vertex, const std::string& fragment, bool isPath)
    {
        if (isPath)
        {
            id = CreateShaderFromFile(vertex, fragment);
        }
        else 
        {
            id = CreateShaderFromCode(vertex, fragment);
        }
    }

    Shader::~Shader()
    {
        if (id != 0)
            glDeleteProgram(id);
    }

    void Shader::Initialize()
    {
        if (s_defaultShader != 0)
        {
            LogError("Shader::Initialize : Shader was already initialized.");
            return;
        }

        s_defaultShader = CreateShaderFromCode(defaultVertexShader, defaultFragmentShader);

    }

#pragma endregion

#pragma region Uniform Binding

    // activate the shader
    // ------------------------------------------------------------------------
    void Shader::Use()
    {
        glUseProgram(id);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void Shader::SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
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
                ThrowError(ss.str());
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
                ThrowError(ss.str());
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
            ThrowError("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " + std::string(e.what()));
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
            LogError("OpenGL Error in Shader::CreateShaderFromCode - Code: " + std::to_string(err));
        }

        return id;
    }

#pragma endregion

}