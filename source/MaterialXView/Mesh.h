#ifndef MATERIALXVIEW_MESH_H
#define MATERIALXVIEW_MESH_H

#include <MaterialXCore/Types.h>

#include <string>
#include <vector>

namespace mx = MaterialX;

class Mesh
{
  public:
    Mesh();
    ~Mesh();
  
    bool loadMesh(const std::string& filename);
    void generateTangents();

    size_t getFaceCount() const
    {
        return _faceCount;
    }

    const std::vector<mx::Vector3>& getPositions() const
    {
        return _positions;
    }

    const std::vector<mx::Vector3>& getNormals() const
    {
        return _normals;
    }

    const std::vector<mx::Vector2>& getTexcoords() const
    {
        return _texcoords;
    }
  
    const std::vector<mx::Vector3>& getTangents() const
    {
        return _tangents;
    }
  
    const std::vector<uint32_t>& getIndices() const
    {
        return _indices;
    }

    const mx::Vector3& getSphereCenter() const
    {
        return _sphereCenter;
    }

    float getSphereRadius() const
    {
        return _sphereRadius;
    }

  private:
    size_t _vertCount;
    size_t _faceCount;
  
    std::vector<mx::Vector3> _positions;
    std::vector<mx::Vector3> _normals;
    std::vector<mx::Vector3> _tangents;
    std::vector<mx::Vector2> _texcoords;
    std::vector<uint32_t> _indices;

    mx::Vector3 _boxMin;
    mx::Vector3 _boxMax;
    mx::Vector3 _sphereCenter;
    float _sphereRadius;
};

using MeshPtr = std::unique_ptr<Mesh>;

#endif // MATERIALXVIEW_MESH_H
