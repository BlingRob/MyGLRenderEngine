#include "Mesh.h"


Mesh::~Mesh()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    /*for (auto& tex : textures)
    {
        if (auto it = GlobalTextures.find(std::hash<std::string>{}(tex->path.C_Str()));tex.use_count() == 1 && it != GlobalTextures.end())
            GlobalTextures.erase(it);
        tex.reset();
    }*/
}

void Mesh::setupMesh()
{
    std::array<std::size_t, 5> Biases;
    // create buffers/arrays

    // load data into vertex buffers

    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glCreateBuffers(1,&EBO);
    glNamedBufferStorage(EBO, sizeof(GLuint) * indices.size(), indices.data(), 0);

    std::size_t i = 0, sum = 0;
    std::size_t Bytes = sizeof(GLfloat) * std::accumulate(vertices._msizes.begin(), vertices._msizes.end(), 0, [&Biases,&i,&sum](auto& accum, std::size_t& x)
        {
            sum = (accum + x);
            Biases[i++] = sizeof(GLfloat) * sum;
            return sum;
        });

    //pos - 3,nor - 3, tex - 2, tan - 3, btan - 3 = 14 

    glCreateBuffers(1, &VBO);
    //glNamedBufferData(VBO[0], sizeof(GLfloat) * (vertices.Positions.size() + vertices.Normals.size() + vertices.TexCoords.size() + vertices.Tangents.size() + vertices.Bitangents.size()), NULL, GL_STATIC_DRAW);
    glNamedBufferStorage(VBO, Bytes, NULL, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(VBO, 0, Biases[0], vertices.Positions);
    glNamedBufferSubData(VBO, Biases[0], sizeof(GLfloat) * vertices._msizes[Vertexes::PointTypes::normals], vertices.Normals);
    glNamedBufferSubData(VBO, Biases[1], sizeof(GLfloat) * vertices._msizes[Vertexes::PointTypes::texture], vertices.TexCoords);
    glNamedBufferSubData(VBO, Biases[2], sizeof(GLfloat) * vertices._msizes[Vertexes::PointTypes::tangent], vertices.Tangents);
    glNamedBufferSubData(VBO, Biases[3], sizeof(GLfloat) * vertices._msizes[Vertexes::PointTypes::bitangent], vertices.Bitangents);

    // set the vertex attribute pointers

    glCreateVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);
    glVertexArrayElementBuffer(VAO, EBO);
    for (uint16_t i = 0; i < 5; ++i)
        glEnableVertexArrayAttrib(VAO, i);


    // vertex Positions
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 0, Mesh::CardCoordsPerPoint, GL_FLOAT, GL_FALSE, 0);
    
    // vertex normals
    glVertexArrayAttribBinding(VAO, 1, 1);
    glVertexArrayVertexBuffer(VAO, 1, VBO, Biases[0], sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 1, Mesh::CardCoordsPerPoint, GL_FLOAT, GL_FALSE, 0);//vertices.Positions.size() * sizeof(GLfloat)
    
    // vertex texture coords
    glVertexArrayAttribBinding(VAO, 2, 2);
    glVertexArrayVertexBuffer(VAO, 2, VBO, Biases[1], sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 2, Mesh::CardCoordsPerTextPoint, GL_FLOAT, GL_FALSE, 0);
    
    // vertex tangent
    glVertexArrayAttribBinding(VAO, 3, 3);
    glVertexArrayVertexBuffer(VAO, 3, VBO, Biases[2], sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 3, Mesh::CardCoordsPerPoint, GL_FLOAT, GL_FALSE, 0);
    
    // vertex bitangent
    glVertexArrayAttribBinding(VAO, 4, 4);
    glVertexArrayVertexBuffer(VAO, 4, VBO, Biases[3], sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 4, Mesh::CardCoordsPerPoint, GL_FLOAT, GL_FALSE, 0);
}

void Mesh::Draw(const Shader* shader)
{
    for (GLuint i = 0; i < textures.size(); ++i) 
    {
        switch (textures[i]->type) 
        {
            case Texture_Types::Diffuse:
                shader->setTexture("tex.diffuse", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Diffuse));
            break;
            case Texture_Types::Normal:
                shader->setTexture("tex.normal", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Normal));
            break;
            case Texture_Types::Specular:
                shader->setTexture("tex.specular", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Specular));
            break;
            case Texture_Types::Emissive:
                shader->setTexture("tex.emissive", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Emissive));
            break;
            case Texture_Types::Height:
                shader->setTexture("tex.height", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Height));
            break;
            case Texture_Types::Metallic_roughness:
                shader->setTexture("tex.metallic_roughness", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Metallic_roughness));
            break;
            case Texture_Types::Ambient_occlusion:
                shader->setTexture("tex.ao", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Ambient_occlusion));
            break;
            case Texture_Types::Skybox:
                shader->setTexture("skybox", std::get<GLuint>(textures[i]->id), static_cast<GLuint>(Texture_Types::Skybox));
            break;
        }
    }

    glUniform3f(glGetUniformLocation(shader->Program, "mat.ambient"), material.ambient.r, material.ambient.g, material.ambient.b);
    glUniform3f(glGetUniformLocation(shader->Program, "mat.diffuse"), material.diffuse.r, material.diffuse.g, material.diffuse.b);
    glUniform3f(glGetUniformLocation(shader->Program, "mat.specular"), material.specular.r, material.specular.g, material.specular.b);
    glUniform1f(glGetUniformLocation(shader->Program, "mat.shininess"), material.shininess);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
    glBindVertexArray(0);
}