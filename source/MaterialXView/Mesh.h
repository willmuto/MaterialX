#ifndef MATERIALXVIEW_MESH_H
#define MATERIALXVIEW_MESH_H

#include <MaterialXCore/Types.h>
#include <MaterialXFormat/File.h>

#include <string>
#include <vector>

namespace mx = MaterialX;

const float MAX_FLOAT = std::numeric_limits<float>::max();

using MeshPtr = std::unique_ptr<class Mesh>;

class Partition
{
  public:
    Partition() :
        _faceCount(0)
    {
    }
    ~Partition() { }

    const std::string& getName() const
    {
        return _name;
    }

    const std::vector<uint32_t>& getIndices() const
    {
        return _indices;
    }

    size_t getFaceCount() const
    {
        return _faceCount;
    }

  private:
    friend class Mesh;

    std::string _name;
    std::vector<uint32_t> _indices;
    size_t _faceCount;
};

class Mesh
{
  public:
    Mesh() :
        _vertCount(0),
        _boxMin(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT),
        _boxMax(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT),
        _sphereRadius(0.0f)
    {
    }
    ~Mesh() { }

    bool loadMesh(const mx::FilePath& filename);
    void generateTangents();

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
  
    const mx::Vector3& getSphereCenter() const
    {
        return _sphereCenter;
    }

    float getSphereRadius() const
    {
        return _sphereRadius;
    }

    size_t getPartitionCount() const
    {
        return _partitions.size();
    }

    const Partition& getPartition(size_t partIndex) const
    {
        return _partitions[partIndex];
    }

  private:
    std::vector<mx::Vector3> _positions;
    std::vector<mx::Vector3> _normals;
    std::vector<mx::Vector3> _tangents;
    std::vector<mx::Vector2> _texcoords;
    size_t _vertCount;

    mx::Vector3 _boxMin;
    mx::Vector3 _boxMax;

    mx::Vector3 _sphereCenter;
    float _sphereRadius;

    std::vector<Partition> _partitions;
};

#endif // MATERIALXVIEW_MESH_H
