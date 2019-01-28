#ifndef MATERIALXVIEW_MATERIAL_H
#define MATERIALXVIEW_MATERIAL_H

#include <MaterialXRender/Handlers/GeometryHandler.h>

#include <MaterialXCore/Document.h>
#include <MaterialXFormat/XmlIo.h>
#include <MaterialXFormat/File.h>

#include <MaterialXRender/OpenGL/GLTextureHandler.h>
#include <MaterialXGenShader/HwShader.h>

#include <nanogui/common.h>
#include <nanogui/glutil.h>

namespace mx = MaterialX;
namespace ng = nanogui;

using MaterialPtr = std::shared_ptr<class Material>;
using GLShaderPtr = std::shared_ptr<ng::GLShader>;

using StringPair = std::pair<std::string, std::string>;

class MaterialSubset
{
  public:
    mx::TypedElementPtr elem;
    std::string udim;
};

class Material
{
  public:
    Material() { }
    ~Material() { }

    /// Generate a shader from the given inputs.
    bool generateShader(const mx::FileSearchPath& searchPath, mx::ElementPtr elem);

    /// Return the underlying OpenGL shader.
    GLShaderPtr getShader() const { return glShader; }
    
    /// Bind the underlying OpenGL shader, returning true upon success.
    bool bindShader();

    /// Bind viewing information for this material.
    void bindViewInformation(const mx::Matrix44& world, const mx::Matrix44& view, const mx::Matrix44& proj);

    /// Bind all images for this material.
    void bindImages(mx::GLTextureHandlerPtr imageHandler, const mx::FileSearchPath& imagePath, const std::string& udim);

    /// Bind a single image.
    bool bindImage(std::string filename, const std::string& uniformName, mx::GLTextureHandlerPtr imageHandler,
        mx::ImageDesc& desc, const std::string& udim = mx::EMPTY_STRING);

    /// Bind lights to shader.
    void bindLights(mx::GLTextureHandlerPtr imageHandler, const mx::FileSearchPath& imagePath, int envSamples);

    /// Bind the given mesh to this material.
    void bindMesh(mx::MeshPtr mesh) const;

    /// Bind a mesh partition to this material.
    void bindPartition(mx::MeshPartitionPtr part) const;

    /// Draw the given mesh partition.
    void drawPartition(mx::MeshPartitionPtr part) const;

    // Return the block of public uniforms for this material.
    const mx::Shader::VariableBlock* getPublicUniforms() const;

    /// Find a public uniform from its MaterialX path.
    mx::Shader::Variable* findUniform(const std::string& path) const;

  public:
    mx::DocumentPtr doc;
    GLShaderPtr glShader;
    mx::HwShaderPtr hwShader;
    std::vector<MaterialSubset> subsets;
    bool hasTransparency;
};

mx::DocumentPtr loadDocument(const mx::FilePath& filePath, mx::DocumentPtr stdLib);
mx::DocumentPtr loadLibraries(const mx::StringVec& libraryNames, const mx::FileSearchPath& searchPath);
void remapNodes(mx::DocumentPtr& doc, const mx::StringMap& nodeRemap);

mx::HwShaderPtr generateSource(const mx::FileSearchPath& searchPath, mx::ElementPtr elem);

#endif // MATERIALXVIEW_MATERIAL_H
