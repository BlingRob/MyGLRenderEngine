#include "Mesh.h"


Mesh :: Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, std::vector<Texture>&& textures, Material&& material)
{
    this->vertices = std::move(vertices);
    this->indices = std::move(indices);
    this->textures = std::move(textures);
    this->material = std::move(material);

    setupMesh();
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

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

    for (size_t i = 0; i < textures.size(); ++i)
    {
        glBindTextureUnit(i, textures[i].id);
        //glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)

        name = textures[i].type;
        if (name == "texture_diffuse")
            sprintf_s(Address, BufferSize,"tex[%d].diffuse", diffuseNr++);
            //number = "[" + std::to_string(diffuseNr++) + "]";
        else if (name == "texture_specular")
            sprintf_s(Address, BufferSize, "tex[%d].specular", specularNr++);
            //number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if (name == "texture_normal")
            sprintf_s(Address, BufferSize, "tex[%d].normal", normalNr++);
            //number = std::to_string(normalNr++); // transfer unsigned int to stream
        else if (name == "texture_height")
            sprintf_s(Address, BufferSize, "tex[%d].height", heightNr++);
            //number = std::to_string(heightNr++); // transfer unsigned int to stream
        else if (name == "texture_emissive")
            sprintf_s(Address, BufferSize, "tex[%d].emissive", emissiveNr++);
            //number = std::to_string(emissiveNr++); // transfer unsigned int to stream

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader->Program, Address), i);
        // and finally bind the texture
        //glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    name = std::string("mat[");
    //for (size_t i = 0; i < materials.size(); ++i) 
    {
        //number = std::to_string(i);
        glUniform3f(glGetUniformLocation(shader->Program, (name + number + "].ambient").c_str()), material.ambient.r, material.ambient.g, material.ambient.b);
        glUniform3f(glGetUniformLocation(shader->Program, (name + number + "].diffuse").c_str()), material.diffuse.r, material.diffuse.g, material.diffuse.b);
        glUniform3f(glGetUniformLocation(shader->Program, (name + number + "].specular").c_str()), material.specular.r, material.specular.g, material.specular.b);
        glUniform1f(glGetUniformLocation(shader->Program, (name + number + "].shininess").c_str()), material.shininess);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}