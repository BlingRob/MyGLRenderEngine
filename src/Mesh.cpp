#include "Mesh.h"


/*Mesh :: Mesh(Vertexes&& vertices, std::vector<GLuint>&& indices, std::vector < Texture>&& textures, Material&& material)
{
    this->vertices = std::move(vertices);
    this->indices = std::move(indices);
    this->textures = std::move(textures);
    this->material = std::move(material);

    setupMesh();
}*/
Mesh::~Mesh()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    for (auto& tex : textures) 
    {
        if (auto it = GlobalTextures.find(std::hash<std::string>{}(tex->path.C_Str()));tex.use_count() == 1 && it != GlobalTextures.end())
            GlobalTextures.erase(it);
        tex.reset();
    }
}

void Mesh::setupMesh()
{

    // create buffers/arrays

    // load data into vertex buffers

    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glCreateBuffers(1,&EBO);
    glNamedBufferStorage(EBO, sizeof(GLuint) * indices.size(), indices.data(), 0);

    //pos - 3,nor - 3, tex - 2, tan - 3, btan - 3 = 14 

    glCreateBuffers(1, &VBO);
    //glNamedBufferData(VBO[0], sizeof(GLfloat) * (vertices.Positions.size() + vertices.Normals.size() + vertices.TexCoords.size() + vertices.Tangents.size() + vertices.Bitangents.size()), NULL, GL_STATIC_DRAW);
    glNamedBufferStorage(VBO, sizeof(GLfloat) * (vertices.Positions.size() + vertices.Normals.size() + vertices.TexCoords.size() + vertices.Tangents.size() + vertices.Bitangents.size()), NULL, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(VBO, 0, vertices.Positions.size() * sizeof(GLfloat), vertices.Positions.data());
    glNamedBufferSubData(VBO, sizeof(GLfloat) * vertices.Positions.size(), sizeof(GLfloat) * vertices.Normals.size(), vertices.Normals.data());
    glNamedBufferSubData(VBO, sizeof(GLfloat) * (vertices.Positions.size() + vertices.Normals.size()), sizeof(GLfloat) * vertices.TexCoords.size(), vertices.TexCoords.data());
    glNamedBufferSubData(VBO, sizeof(GLfloat) * (vertices.Positions.size() + vertices.Normals.size() + vertices.TexCoords.size()), sizeof(GLfloat) * vertices.Tangents.size(), vertices.Tangents.data());
    glNamedBufferSubData(VBO, sizeof(GLfloat) * (vertices.Positions.size() + vertices.Normals.size() + vertices.TexCoords.size() + vertices.Tangents.size()), sizeof(GLfloat) * vertices.Bitangents.size(), vertices.Bitangents.data());
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
    glVertexArrayVertexBuffer(VAO, 1, VBO, sizeof(GLfloat) * vertices.Positions.size(), sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 1, Mesh::CardCoordsPerPoint, GL_FLOAT, GL_FALSE, 0);//vertices.Positions.size() * sizeof(GLfloat)
    
    // vertex texture coords
    glVertexArrayAttribBinding(VAO, 2, 2);
    glVertexArrayVertexBuffer(VAO, 2, VBO, sizeof(GLfloat) * (vertices.Normals.size() + vertices.Positions.size()), sizeof(GLfloat) * Mesh::CardCoordsPerTextPoint);
    glVertexArrayAttribFormat(VAO, 2, Mesh::CardCoordsPerTextPoint, GL_FLOAT, GL_FALSE, 0);
    
    // vertex tangent
    glVertexArrayAttribBinding(VAO, 3, 3);
    glVertexArrayVertexBuffer(VAO, 3, VBO, sizeof(GLfloat) * (vertices.Normals.size() + vertices.Positions.size() + vertices.TexCoords.size()), sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 3, Mesh::CardCoordsPerPoint, GL_FLOAT, GL_FALSE, 0);
    
    // vertex bitangent
    glVertexArrayAttribBinding(VAO, 4, 4);
    glVertexArrayVertexBuffer(VAO, 4, VBO, sizeof(GLfloat) * (vertices.Normals.size() + vertices.Positions.size() + vertices.TexCoords.size() + vertices.Tangents.size()), sizeof(GLfloat) * Mesh::CardCoordsPerPoint);
    glVertexArrayAttribFormat(VAO, 4, Mesh::CardCoordsPerPoint, GL_FLOAT, GL_FALSE, 0);
}

void Mesh::Draw(Shader* shader)
{
    const size_t BufferSize = 32;
    // bind appropriate textures
    GLuint diffuseNr = 0;
    GLuint specularNr = 0;
    GLuint normalNr = 0;
    GLuint heightNr = 0;
    GLuint emissiveNr = 0;
    GLuint matallic = 0;
    GLuint roughness = 0;
    GLuint ao = 0;
    std::string number;
    std::string name;
    char Address[BufferSize];

    for (GLuint i = 0; i < textures.size(); ++i)
    {
        glBindTextureUnit(i, textures[i]->id);
        // retrieve texture number (the N in diffuse_textureN)

        name = textures[i]->type;
        if (name == "texture_diffuse")
            snprintf(Address, BufferSize,"tex[%d].diffuse", diffuseNr++);// transfer unsigned int to stream
        else if (name == "texture_specular")
            snprintf(Address, BufferSize, "tex[%d].specular", specularNr++);
        else if (name == "texture_normal")
            snprintf(Address, BufferSize, "tex[%d].normal", normalNr++);
        else if (name == "texture_height")
            snprintf(Address, BufferSize, "tex[%d].height", heightNr++);
        else if (name == "texture_emissive")
            snprintf(Address, BufferSize, "tex[%d].emissive", emissiveNr++);
        else if (name == "texture_metallic_rougness")
            snprintf(Address, BufferSize, "tex[%d].metallic_roughness", matallic++);
        /*else if (name == "texture_metallic")
            sprintf_s(Address, BufferSize, "tex[%d].metallic", matallic++);
        else if (name == "texture_roughness")
            sprintf_s(Address, BufferSize, "tex[%d].roughness", roughness++);*/
        else if (name == "texture_ambient_occlusion")
            snprintf(Address, BufferSize, "tex[%d].ao", ao++);
        else if(name == "SkyBox")
            snprintf(Address, BufferSize, "skybox");

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader->Program, Address), i);
  
    }
    name = std::string("mat[");
    for (size_t i = 0; i < material.size(); ++i) 
    {
        number = std::to_string(i);
        glUniform3f(glGetUniformLocation(shader->Program, (name + number + "].ambient").c_str()), material[i].ambient.r, material[i].ambient.g, material[i].ambient.b);
        glUniform3f(glGetUniformLocation(shader->Program, (name + number + "].diffuse").c_str()), material[i].diffuse.r, material[i].diffuse.g, material[i].diffuse.b);
        glUniform3f(glGetUniformLocation(shader->Program, (name + number + "].specular").c_str()), material[i].specular.r, material[i].specular.g, material[i].specular.b);
        glUniform1f(glGetUniformLocation(shader->Program, (name + number + "].shininess").c_str()), material[i].shininess);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
    glBindVertexArray(0);
}