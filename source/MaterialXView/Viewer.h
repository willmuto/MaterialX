#ifndef MATERIALXVIEW_VIEWER_H
#define MATERIALXVIEW_VIEWER_H

#include <MaterialXView/Editor.h>
#include <MaterialXView/Material.h>
#include <MaterialXView/Mesh.h>

namespace mx = MaterialX;
namespace ng = nanogui;

class Viewer : public ng::Screen
{
  public:
    Viewer(const mx::StringVec& libraryFolders,
           const mx::FileSearchPath& searchPath,
           const mx::StringMap& nodeRemap);
    ~Viewer() { }

    void drawContents() override;
    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;

    bool scrollEvent(const ng::Vector2i& p, const ng::Vector2f& rel) override;
    bool mouseMotionEvent(const ng::Vector2i& p, const ng::Vector2i& rel, int button, int modifiers) override;
    bool mouseButtonEvent(const ng::Vector2i& p, int button, bool down, int modifiers) override;

    mx::ElementPtr getSelectedElement() const
    {
        mx::ElementPtr element = nullptr;
        if (_elementSelectionIndex >= 0 && _elementSelectionIndex < _elementSelections.size())
        {
            element = _elementSelections[_elementSelectionIndex];
        }
        return element;
    }

    MaterialPtr getMaterial() const
    {
        return _material;
    }

    mx::DocumentPtr getContentDocument() const
    {
        return _contentDocument;
    }

    ng::Window* getWindow() const
    {
        return _window;
    }

    const mx::FileSearchPath& getSearchPath() const
    {
        return _searchPath;
    }

    const mx::GLTextureHandlerPtr getImageHandler() const
    {
        return _imageHandler;
    }

  private:
    void initCamera();
    void computeCameraMatrices(mx::Matrix44& world,
                               mx::Matrix44& view,
                               mx::Matrix44& proj);

    bool setElementSelection(int index);
    void updateElementSelections();
    void updatePropertyEditor();

  private:
    ng::Window* _window;
    ng::Arcball _arcball;

    MeshPtr _mesh;
    MaterialPtr _material;

    mx::Vector3 _eye;
    mx::Vector3 _center;
    mx::Vector3 _up;
    float _zoom;
    float _viewAngle;
    float _nearDist;
    float _farDist;

    mx::Vector3 _modelTranslation;
    mx::Vector3 _modelTranslationStart;
    float _modelZoom;

    bool _translationActive;
    ng::Vector2i _translationStart;

    mx::StringVec _libraryFolders;
    mx::FileSearchPath _searchPath;
    mx::StringMap _nodeRemap;

    mx::FilePath _materialFilename;
    int _envSamples;

    mx::DocumentPtr _contentDocument;
    mx::DocumentPtr _stdLib;

    std::vector<mx::TypedElementPtr> _elementSelections;
    int _elementSelectionIndex;
    ng::ComboBox* _elementSelectionBox;

    PropertyEditor _propertyEditor;

    mx::GLTextureHandlerPtr _imageHandler;
};

#endif // MATERIALXVIEW_VIEWER_H
