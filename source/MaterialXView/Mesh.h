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

    size_t getFaceCount()
    {
        return _faceCount;
    }

    const mx::Vector3 getMeshCenter()
    {
        return _meshCenter;
    }

    float getMaxDist()
    {
        return _maxDist;
    }

    std::vector<mx::Vector3>& getPositions()
    {
        return _positions;
    }

    std::vector<mx::Vector3>& getNormals()
    {
        return _normals;
    }

    std::vector<mx::Vector2>& getTexcoords()
    {
        return _texcoords;
    }
  
    std::vector<mx::Vector3>& getTangents()
    {
        return _tangents;
    }
  
    std::vector<uint32_t>& getIndices()
    {
        return _indices;
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
    mx::Vector3 _meshCenter;
    float _maxDist = 0.0f;
};

using MeshPtr = std::unique_ptr<Mesh>;

#endif // MATERIALXVIEW_MESH_H
