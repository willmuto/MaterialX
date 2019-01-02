#include <MaterialXView/Mesh.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <MaterialXView/TinyObjLoader/tiny_obj_loader.h>

#include <iostream>

const float MAX_FLOAT = std::numeric_limits<float>::max();

Mesh::Mesh() :
    _vertCount(0),
    _faceCount(0),
    _boxMin(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT),
    _boxMax(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT),
    _sphereRadius(0.0f)
{
}

Mesh::~Mesh()
{
}

bool Mesh::loadMesh(const std::string& filename)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool load = tinyobj::LoadObj(&attrib, &shapes, &materials, nullptr, &err,
                                 filename.c_str(), nullptr, true, false);
    if (!load)
    {
        std::cerr << err << std::endl;
        return false;
    }

    size_t vertComponentCount = std::max(attrib.vertices.size(), attrib.normals.size());
    _vertCount = vertComponentCount / 3;
    if (!_vertCount)
    {
        return false;
    }

    for (const tinyobj::shape_t& shape : shapes)
    {
        _faceCount += shape.mesh.indices.size() / 3;
    }

    _positions.resize(_vertCount);
    _normals.resize(_vertCount);
    _texcoords.resize(_vertCount);
    _tangents.resize(_vertCount);
    _indices.resize(_faceCount * 3);

    size_t shapeIndexOffset = 0;
    for (const tinyobj::shape_t& shape : shapes)
    {
        for (size_t faceIndex = 0; faceIndex < shape.mesh.indices.size() / 3; faceIndex++)
        {
            const tinyobj::index_t& indexObj0 = shape.mesh.indices[faceIndex * 3 + 0];
            const tinyobj::index_t& indexObj1 = shape.mesh.indices[faceIndex * 3 + 1];
            const tinyobj::index_t& indexObj2 = shape.mesh.indices[faceIndex * 3 + 2];

            int writeIndex0, writeIndex1, writeIndex2;
            if (vertComponentCount == attrib.vertices.size())
            {
                writeIndex0 = indexObj0.vertex_index;
                writeIndex1 = indexObj1.vertex_index;
                writeIndex2 = indexObj2.vertex_index;
            }
            else
            {
                writeIndex0 = indexObj0.normal_index;
                writeIndex1 = indexObj1.normal_index;
                writeIndex2 = indexObj2.normal_index;
            }
  
            // Copy positions and compute bounding box.
            mx::Vector3 v[3];
            for (int k = 0; k < 3; k++)
            {
                _indices[shapeIndexOffset + faceIndex * 3 + 0] = writeIndex0;
                _indices[shapeIndexOffset + faceIndex * 3 + 1] = writeIndex1;
                _indices[shapeIndexOffset + faceIndex * 3 + 2] = writeIndex2;

                v[0][k] = attrib.vertices[3 * indexObj0.vertex_index + k];
                v[1][k] = attrib.vertices[3 * indexObj1.vertex_index + k];
                v[2][k] = attrib.vertices[3 * indexObj2.vertex_index + k];
        
                _boxMin[k] = std::min(v[0][k], _boxMin[k]);
                _boxMin[k] = std::min(v[1][k], _boxMin[k]);
                _boxMin[k] = std::min(v[2][k], _boxMin[k]);
 
                _boxMax[k] = std::max(v[0][k], _boxMax[k]);
                _boxMax[k] = std::max(v[1][k], _boxMax[k]);
                _boxMax[k] = std::max(v[2][k], _boxMax[k]);
            }

            _sphereCenter = (_boxMax + _boxMin) / 2;
            _sphereRadius = (_sphereCenter - _boxMin).getMagnitude();

            // Copy or compute normals
            mx::Vector3 n[3];
            if (indexObj0.normal_index >= 0 &&
                indexObj1.normal_index >= 0 &&
                indexObj2.normal_index >= 0)
            {
                for (int k = 0; k < 3; k++)
                {
                    n[0][k] = attrib.normals[3 * indexObj0.normal_index + k];
                    n[1][k] = attrib.normals[3 * indexObj1.normal_index + k];
                    n[2][k] = attrib.normals[3 * indexObj2.normal_index + k];
                }
            }
            else
            {
                mx::Vector3 faceNorm = (v[1] - v[0]).cross(v[2] - v[0]).getNormalized();
                n[0] = faceNorm;
                n[1] = faceNorm;
                n[2] = faceNorm;
            }

            // Copy texture coordinates.
            mx::Vector2 t[3];
            if (indexObj0.texcoord_index >= 0 &&
                indexObj1.texcoord_index >= 0 &&
                indexObj2.texcoord_index >= 0)
            {
                for (int k = 0; k < 2; k++)
                {
                    t[0][k] = attrib.texcoords[2 * indexObj0.texcoord_index + k];
                    t[1][k] = attrib.texcoords[2 * indexObj1.texcoord_index + k];
                    t[2][k] = attrib.texcoords[2 * indexObj2.texcoord_index + k];
                }
            }

            _positions[writeIndex0] = v[0];
            _positions[writeIndex1] = v[1];
            _positions[writeIndex2] = v[2];

            _normals[writeIndex0] = n[0];
            _normals[writeIndex1] = n[1];
            _normals[writeIndex2] = n[2];

            _texcoords[writeIndex0] = t[0];
            _texcoords[writeIndex1] = t[1];
            _texcoords[writeIndex2] = t[2];
        }

        shapeIndexOffset += shape.mesh.indices.size();
    }

    generateTangents();

    return true;
}

void Mesh::generateTangents()
{
    // Based on Eric Lengyel at http://www.terathon.com/code/tangent.html

    for (size_t faceIndex = 0; faceIndex < _faceCount; faceIndex++)
    {
        int i1 = _indices[faceIndex * 3 + 0];
        int i2 = _indices[faceIndex * 3 + 1];
        int i3 = _indices[faceIndex * 3 + 2];

        const mx::Vector3& v1 = _positions[i1];
        const mx::Vector3& v2 = _positions[i2];
        const mx::Vector3& v3 = _positions[i3];

        const mx::Vector2& w1 = _texcoords[i1];
        const mx::Vector2& w2 = _texcoords[i2];
        const mx::Vector2& w3 = _texcoords[i3];

        float x1 = v2[0] - v1[0];
        float x2 = v3[0] - v1[0];
        float y1 = v2[1] - v1[1];
        float y2 = v3[1] - v1[1];
        float z1 = v2[2] - v1[2];
        float z2 = v3[2] - v1[2];
        
        float s1 = w2[0] - w1[0];
        float s2 = w3[0] - w1[0];
        float t1 = w2[1] - w1[1];
        float t2 = w3[1] - w1[1];
        
        float denom = s1 * t2 - s2 * t1;
        float r = denom ? 1.0f / denom : 0.0f;
        mx::Vector3 dir((t2 * x1 - t1 * x2) * r,
                        (t2 * y1 - t1 * y2) * r,
                        (t2 * z1 - t1 * z2) * r);
        
        _tangents[i1] += dir;
        _tangents[i2] += dir;
        _tangents[i3] += dir;
    }
    
    for (size_t v = 0; v < _vertCount; v++)
    {
        const mx::Vector3& n = _normals[v];
        mx::Vector3& t = _tangents[v];

        // Gram-Schmidt orthogonalize
        if (t != mx::Vector3(0.0f))
        {
            t = (t - n * n.dot(t)).getNormalized();
        }
    }
}
