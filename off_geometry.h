#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

class OFFGeometry : public IGeometry
{
public:
    OFFGeometry(const std::string & filename, glm::vec3 color)
    {
        m_vertices_size = 0;
        m_faces_size = 0;
        bool with_texture = false;
        bool with_color = false;
        bool with_normals = false;

        std::ifstream off_file(filename.c_str());
        if (!off_file)
        {
            std::cerr << "Could not find file: " << filename << std::endl;
            return;
        }

        std::string off;
        off_file >> off;
        if (off == "OFF")
        {
            /* default */
        }
        else if (off == "STOFF")
        {
            with_texture = true;
        }
        else if (off == "COFF")
        {
            with_color = true;
        }
        else if (off == "STNOFF")
        {
            with_texture = true;
            with_normals = true;
        }
        else if (off == "CNOFF")
        {
            with_color = true;
            with_normals = true;
        }
        else
        {
            std::cerr << "File is not an OFF file: " << filename << std::endl;
            return;
        }

        int off_vertices, off_faces, off_edges;
        off_file >> off_vertices >> off_faces >> off_edges;

        std::string line;
        int vertices_counter = 0;
        int faces_counter = 0;
        int triangles_counter = 0;
        bool reading_end = false;
        while (std::getline(off_file, line) && !reading_end)
        {
            if (line.empty())
                continue;
            if (line[0] == '#')
                continue; // comment
            if (line.find_first_not_of("\t \n\r") == std::string::npos)
                continue; // only spaces
            if (vertices_counter < off_vertices)
            {
                GLfloat x, y, z;
                std::istringstream istr(line);
                istr >> x >> y >> z;
                if (!istr)
                {
                    std::cerr << "File: " << filename << ", error at vertex " << vertices_counter << std::endl;
                    return;
                }

                m_vertices.push_back(x);
                m_vertices.push_back(y);
                m_vertices.push_back(z);

                if (with_color)
                {
                    GLfloat r, g, b;
                    istr >> r >> g >> b;
                    m_colors.push_back(r);
                    m_colors.push_back(g);
                    m_colors.push_back(b);
                }
                else
                {
                    m_colors.push_back(color.r);
                    m_colors.push_back(color.g);
                    m_colors.push_back(color.b);
                }

                if (with_texture)
                {
                    GLfloat u, v;
                    istr >> u >> v;
                    m_texcoords.push_back(u);
                    m_texcoords.push_back(v);
                }

                if (with_normals)
                {
                    GLfloat nx, ny, nz;
                    istr >> nx >> ny >> nz;
                    m_normals.push_back(nx);
                    m_normals.push_back(ny);
                    m_normals.push_back(nz);
                }

                vertices_counter++;
            }
            else if (faces_counter < off_faces)
            {
                std::istringstream istr(line);
                int count;
                istr >> count;
                if (count > 2)
                {
                    GLuint v0, v1, v2;
                    istr >> v0 >> v1;
                    if (v0 >= GLuint(vertices_counter) || v1 >= GLuint(vertices_counter))
                    {
                        std::cerr << "File: " << filename << ", vertex out of range at face " << faces_counter << std::endl;
                        return;
                    }
                    for (int i = 2; i < count; i++)
                    {
                        istr >> v2;
                        if (v2 >= GLuint(vertices_counter))
                        {
                            std::cerr << "File: " << filename << ", vertex out of range at face " << faces_counter << std::endl;
                            return;
                        }
                        m_faces.push_back(v0);
                        m_faces.push_back(v1);
                        m_faces.push_back(v2);
                        triangles_counter++;
                        v1 = v2;
                    }
                }
                if (!istr)
                {
                    std::cerr << "File: " << filename << ", error at face " << faces_counter << std::endl;
                    return;
                }
                faces_counter++;
            }
            else
                reading_end = true;
        }

        if (vertices_counter < off_vertices || faces_counter < off_faces)
        {
            std::cerr << "File: " << filename << ", unexpected End Of File" << std::endl;
            return;
        }

        if (m_normals.empty())
        {
            m_normals.resize(m_vertices.size());
            computeNormals(m_faces.data(), m_vertices.data(), 0, triangles_counter, m_normals.data());
        }

        m_faces_size = triangles_counter;
        m_vertices_size = vertices_counter;
    }

    ~OFFGeometry()
    {
    }

    const GLfloat * vertices() { return m_vertices.data(); }
    const GLfloat * colors() { return m_colors.data(); }
    const GLfloat * normals() { return m_normals.data(); }
    const GLfloat * texCoords() { return m_texcoords.empty() ? NULL : m_texcoords.data(); }
    const GLuint * faces() { return m_faces.data(); }
    GLsizei verticesSize() { return m_vertices_size; }
    GLsizei size() { return m_faces_size * 3; }

    GLenum type() { return GL_TRIANGLES; }

private:
    std::vector<GLfloat> m_vertices;
    std::vector<GLfloat> m_colors;
    std::vector<GLfloat> m_normals;
    std::vector<GLfloat> m_texcoords;
    GLsizei m_vertices_size;

    std::vector<GLuint> m_faces;
    GLsizei m_faces_size;
};
