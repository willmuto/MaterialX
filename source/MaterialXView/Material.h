#ifndef MATERIALXVIEW_MATERIAL_H
#define MATERIALXVIEW_MATERIAL_H

#include <MaterialXView/Mesh.h>

#include <MaterialXCore/Document.h>
#include <MaterialXFormat/XmlIo.h>
#include <MaterialXFormat/File.h>

#include <MaterialXRender/Handlers/TinyExrImageHandler.h>

#include <nanogui/common.h>
#include <nanogui/glutil.h>

namespace mx = MaterialX;
namespace ng = nanogui;

using StringPair = std::pair<std::string, std::string>;
using ShaderPtr = std::unique_ptr<ng::GLShader>;

void loadLibraries(const mx::StringVec& libraryNames, const mx::FilePath& searchPath, mx::DocumentPtr doc);

StringPair generateSource(const mx::FilePath& filePath, const mx::FilePath& searchPath, mx::DocumentPtr stdLib);
ShaderPtr generateShader(const mx::FilePath& filePath, const mx::FilePath& searchPath, mx::DocumentPtr stdLib);

void bindUniforms(ShaderPtr& shader, mx::ImageHandlerPtr imageHandler, mx::FilePath imagePath, int envSamples,
                  mx::Matrix44& world, mx::Matrix44& view, mx::Matrix44& proj);

#endif // MATERIALXVIEW_MATERIAL_H
