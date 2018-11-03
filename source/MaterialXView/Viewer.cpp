#include <MaterialXView/Viewer.h>

#include <nanogui/button.h>
#include <nanogui/combobox.h>
#include <nanogui/layout.h>
#include <nanogui/messagedialog.h>

#include <iostream>
#include <fstream>

using MatrixXfProxy = Eigen::Map<const ng::MatrixXf>;
using MatrixXuProxy = Eigen::Map<const ng::MatrixXu>;

const float PI = std::acos(-1.0f);

const int MIN_ENV_SAMPLES = 16;
const int MAX_ENV_SAMPLES = 256;

void writeTextFile(const std::string& text, const std::string& filePath)
{
    std::ofstream file;
    file.open(filePath);
    file << text;
    file.close();
}
  
Viewer::Viewer() :
    ng::Screen(ng::Vector2i(1280, 960), "MaterialXView"),
    _translationActive(false),
    _translationStart(0, 0),
    _envSamples(MIN_ENV_SAMPLES)
{
    _window = new ng::Window(this, "Viewer Options");
    _window->setPosition(ng::Vector2i(15, 15));
    _window->setLayout(new ng::GroupLayout());

    ng::Button* meshButton = new ng::Button(_window, "Load Mesh");
    meshButton->setCallback([this]()
    {
        std::string filename = ng::file_dialog({{"obj", "Wavefront OBJ"}}, false);
        if (!filename.empty())
        {
            mProcessEvents = false;
            _mesh = MeshPtr(new Mesh());
            _mesh->loadMesh(filename);
            bindMesh();
            recenterCamera();
            mProcessEvents = true;
        }
    });

    ng::Button* materialButton = new ng::Button(_window, "Load Material");
    materialButton->setCallback([this]()
    {
        std::string filename = ng::file_dialog({{"mtlx", "MaterialX"}}, false);
        if (!filename.empty())
        {
            mProcessEvents = false;
            _materialFilename = filename;
            try
            {
                _glShader = generateShader(_materialFilename, _searchPath, _stdLib);
                bindMesh();
            }
            catch (std::exception& e)
            {
                new ng::MessageDialog(this, ng::MessageDialog::Type::Warning, "Shader Generation Error", e.what());
            }
            mProcessEvents = true;
        }
    });

    mx::StringVec sampleOptions;
    for (int i = MIN_ENV_SAMPLES; i <= MAX_ENV_SAMPLES; i *= 2)
    {
        mProcessEvents = false;
        sampleOptions.push_back("Samples " + std::to_string(i));
        mProcessEvents = true;
    }
    ng::ComboBox* sampleBox = new ng::ComboBox(_window, sampleOptions);
    sampleBox->setChevronIcon(-1);
    sampleBox->setCallback([this](int index)
    {
        _envSamples = MIN_ENV_SAMPLES * (int) std::pow(2, index);
    });

    performLayout();

    _stdLib = mx::createDocument();
    _startPath = mx::FilePath::getCurrentPath();
    _searchPath = _startPath / mx::FilePath("documents/Libraries");
    _materialFilename = std::string("documents/TestSuite/sxpbrlib/materials/standard_surface_default.mtlx");

    _imageHandler = mx::TinyEXRImageHandler::create();
    loadLibraries({"stdlib", "sxpbrlib"}, _searchPath, _stdLib);

    _mesh = MeshPtr(new Mesh());
    _mesh->loadMesh("documents/TestSuite/Geometry/teapot.obj");
    recenterCamera();

    try
    {
        _glShader = generateShader(_materialFilename, _searchPath, _stdLib);
        bindMesh();
    }
    catch (std::exception& e)
    {
        new ng::MessageDialog(this, ng::MessageDialog::Type::Warning, "Shader Generation Error", e.what());
    }
}

bool Viewer::keyboardEvent(int key, int scancode, int action, int modifiers)
{
    if (Screen::keyboardEvent(key, scancode, action, modifiers))
    {
        return true;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        if (!_materialFilename.isEmpty())
        {
            try
            {
                _glShader = generateShader(_materialFilename, _searchPath, _stdLib);
                bindMesh();
            }
            catch (std::exception& e)
            {
                new ng::MessageDialog(this, ng::MessageDialog::Type::Warning, "Shader Generation Error", e.what());
            }
        }
        return true;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        if (!_materialFilename.isEmpty())
        {
            try
            {
                StringPair source = generateSource(_materialFilename, _searchPath, _stdLib);
                std::string baseName = mx::splitString(_materialFilename.getBaseName(), ".")[0];
                mx::StringVec splitName = mx::splitString(baseName, ".");
                writeTextFile(source.first, _startPath / (baseName + "_vs.glsl"));
                writeTextFile(source.second, _startPath / (baseName + "_ps.glsl"));
            }
            catch (std::exception& e)
            {
                new ng::MessageDialog(this, ng::MessageDialog::Type::Warning, "Shader Generation Error", e.what());
            }
        }
        return true;
    }
    return false;
}

void Viewer::drawContents()
{
    if (!_mesh || !_glShader)
    {
        return;
    }

    mx::Matrix44 world, view, proj;
    computeCameraMatrices(world, view, proj);

    _glShader->bind();

    bindUniforms(_glShader, _imageHandler, _startPath, _envSamples, world, view, proj);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    _glShader->drawIndexed(GL_TRIANGLES, 0, (uint32_t) _mesh->getFaceCount());

    glDisable(GL_FRAMEBUFFER_SRGB);
}

bool Viewer::scrollEvent(const ng::Vector2i& p, const ng::Vector2f& rel)
{
    if (!Screen::scrollEvent(p, rel))
    {
        _cameraParams.zoom = std::max(0.1f, _cameraParams.zoom * ((rel.y() > 0) ? 1.1f : 0.9f));
    }
    return true;
}

bool Viewer::mouseMotionEvent(const ng::Vector2i& p,
                              const ng::Vector2i& rel,
                              int button,
                              int modifiers)
{
    if (!Screen::mouseMotionEvent(p, rel, button, modifiers))
    {
        if (_cameraParams.arcball.motion(p))
        {
        }
        else if (_translationActive)
        {
            mx::Matrix44 world, view, proj;
            computeCameraMatrices(world, view, proj);
            mx::Matrix44 worldView = view * world;
            float zval = ng::project(ng::Vector3f(&_mesh->getSphereCenter()[0]),
                                     ng::Matrix4f(&worldView.getTranspose()[0][0]),
                                     ng::Matrix4f(&proj.getTranspose()[0][0]),
                                     mSize).z();
            ng::Vector3f pos1 = ng::unproject(ng::Vector3f((float) p.x(),
                                                           (float) (mSize.y() - p.y()),
                                                           (float) zval),
                                              ng::Matrix4f(&worldView.getTranspose()[0][0]),
                                              ng::Matrix4f(&proj.getTranspose()[0][0]),
                                              mSize);
            ng::Vector3f pos0 = ng::unproject(ng::Vector3f((float) _translationStart.x(),
                                                           (float) (mSize.y() - _translationStart.y()),
                                                           (float) zval),
                                              ng::Matrix4f(&worldView.getTranspose()[0][0]),
                                              ng::Matrix4f(&proj.getTranspose()[0][0]),
                                              mSize);
            ng::Vector3f delta = pos1 - pos0;
            _cameraParams.modelTranslation = _cameraParams.modelTranslationStart +
                                             mx::Vector3(delta.data(), delta.data() + delta.size());
        }
    }
    return true;
}

bool Viewer::mouseButtonEvent(const ng::Vector2i& p, int button, bool down, int modifiers)
{
    if (!Screen::mouseButtonEvent(p, button, down, modifiers))
    {
        if (button == GLFW_MOUSE_BUTTON_1 && !modifiers)
        {
            _cameraParams.arcball.button(p, down);
        }
        else if (button == GLFW_MOUSE_BUTTON_2 ||
                (button == GLFW_MOUSE_BUTTON_1 && modifiers == GLFW_MOD_SHIFT))
        {
            _cameraParams.modelTranslationStart = _cameraParams.modelTranslation;
            _translationActive = true;
            _translationStart = p;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_1 && !down)
    {
        _cameraParams.arcball.button(p, false);
    }
    if (!down)
    {
        _translationActive = false;
    }
    return true;
}

void Viewer::bindMesh()
{
    if (!_mesh)
    {
        return;
    }

    MatrixXfProxy positions(&_mesh->getPositions()[0][0], 3, _mesh->getPositions().size());
    MatrixXfProxy normals(&_mesh->getNormals()[0][0], 3, _mesh->getNormals().size());
    MatrixXfProxy tangents(&_mesh->getTangents()[0][0], 3, _mesh->getTangents().size());
    MatrixXfProxy texcoords(&_mesh->getTexcoords()[0][0], 2, _mesh->getTexcoords().size());
    MatrixXuProxy indices(&_mesh->getIndices()[0], 3, _mesh->getIndices().size() / 3);

    _glShader->bind();
    _glShader->uploadAttrib("i_position", positions);
    _glShader->uploadAttrib("i_normal", normals);
    _glShader->uploadAttrib("i_tangent", tangents);
    _glShader->uploadAttrib("i_texcoord_0", texcoords);
    _glShader->uploadIndices(indices);
}

void Viewer::recenterCamera()
{
    _cameraParams.arcball = ng::Arcball();
    _cameraParams.arcball.setSize(mSize);
    _cameraParams.modelZoom = 2.0f / _mesh->getSphereRadius();
    _cameraParams.modelTranslation = _mesh->getSphereCenter() * -1.0f;
}

void Viewer::computeCameraMatrices(mx::Matrix44& world,
                                   mx::Matrix44& view,
                                   mx::Matrix44& proj)
{
    float fH = std::tan(_cameraParams.viewAngle / 360.0f * PI) * _cameraParams.dnear;
    float fW = fH * (float) mSize.x() / (float) mSize.y();

    ng::Matrix4f ngView = ng::lookAt(ng::Vector3f(&_cameraParams.eye[0]),
                                     ng::Vector3f(&_cameraParams.center[0]),
                                     ng::Vector3f(&_cameraParams.up[0]));
    ngView *= _cameraParams.arcball.matrix();
    ng::Matrix4f ngProj = ng::frustum(-fW, fW, -fH, fH, _cameraParams.dnear, _cameraParams.dfar);

    view = mx::Matrix44(ngView.data(), ngView.data() + ngView.size()).getTranspose();
    proj = mx::Matrix44(ngProj.data(), ngProj.data() + ngProj.size()).getTranspose();
    world = mx::Matrix44::createScale(mx::Vector3(_cameraParams.zoom * _cameraParams.modelZoom));
    world *= mx::Matrix44::createTranslation(_cameraParams.modelTranslation).getTranspose();
}
