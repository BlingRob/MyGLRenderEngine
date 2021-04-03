#include "Mesh.h"


/*Mesh :: Mesh(Vertexes&& vertices, std::vector<GLuint>&& indices, std::vector < Texture>&& textures, Material&& material)
{
    this->vertices = std::move(vertices);
    this->indices = std::move(indices);
    this->textures = std::move(textures);
    this->material = std::move(material);

    setupMesh();
}*/

void Mesh::setupMesh()
{
    // create buffers/arrays

    // load data into vertex buffers
    
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glGenBuffers(5, &VBO[0]);
    //pos - 3,nor - 3, tex - 2, tan - 3, btan - 3 = 14 
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.Positions.size() * sizeof(GLfloat), vertices.Positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER,  vertices.Normals.size() * sizeof(GLfloat), vertices.Normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, vertices.TexCoords.size() * sizeof(GLfloat), vertices.TexCoords.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, vertices.Tangents.size() * sizeof(GLfloat), vertices.Tangents.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, vertices.Bitangents.size() * sizeof(GLfloat), vertices.Bitangents.data(), GL_STATIC_DRAW);

    // set the vertex attribute pointers

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    for(uint16_t i = 0;i < 5;++i)
        glEnableVertexAttribArray(i);
    // set the vertex attribute pointers
    // vertex Positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // vertex normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // vertex texture coords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    // vertex tangent
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // vertex bitangent
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);
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
    std::string number;
    std::string name;
    char Address[BufferSize];

    for (GLuint i = 0; i < textures.size(); ++i)
    {
        glBindTextureUnit(i, textures[i]->id);
        // retrieve texture number (the N in diffuse_textureN)

        name = textures[i]->type;
        if (name == "texture_diffuse")
            sprintf_s(Address, BufferSize,"tex[%d].diffuse", diffuseNr++);// transfer unsigned int to stream
        else if (name == "texture_specular")
            sprintf_s(Address, BufferSize, "tex[%d].specular", specularNr++);
        else if (name == "texture_normal")
            sprintf_s(Address, BufferSize, "tex[%d].normal", normalNr++);
        else if (name == "texture_height")
            sprintf_s(Address, BufferSize, "tex[%d].height", heightNr++);
        else if (name == "texture_emissive")
            sprintf_s(Address, BufferSize, "tex[%d].emissive", emissiveNr++);
        else if(name == "SkyBox")
            sprintf_s(Address, BufferSize, "skybox");

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
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
    glBindVertexArray(0);

}