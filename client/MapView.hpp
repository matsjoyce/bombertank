#ifndef MAP_VIEW_HPP
#define MAP_VIEW_HPP

#include <QQmlComponent>
#include <QQuickItem>
#include <QTimer>

#include "GameState.hpp"
#include "common/Constants.hpp"
#include "objects/TankControl.hpp"

class MapView : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(GameState* state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(
        int controlledObjectId READ controlledObjectId WRITE setControlledObjectId NOTIFY controlledObjectIdChanged)
    Q_PROPERTY(BaseObjectState* controlledObject READ controlledObject NOTIFY controlledObjectChanged)
    Q_PROPERTY(bool rotateWithControlledObject READ rotateWithControlledObject WRITE setRotateWithControlledObject
                   NOTIFY rotateWithControlledObjectChanged)

    QTimer _timer;
    GameState* _state = nullptr;
    std::map<ObjectType, QQmlComponent*> _spriteComponents;
    std::map<ObjectType, QQmlComponent*> _inputComponents;
    std::map<int, QQuickItem*> _sprites;
    QQuickItem* _controls = nullptr;
    BaseObjectState* _controlledObject = nullptr;
    bool _controlsUpdated = false;
    int _controllingId = 0;
    float _viewCenterX = 0, _viewCenterY = 0, _viewRotation = 0;
    bool _rotateWithControlledObject = false;

    void _doUpdate();
    void _handleControlsUpdated();
    void _checkComponentsLoaded();
    void _attachToObject(int id, ObjectType type);
    void _controlledObjectDeleted();

   public:
    MapView();
    GameState* state() { return _state; }
    void setState(GameState* state);

    int controlledObjectId() { return _controllingId; }
    void setControlledObjectId(int controlledObjectId);
    BaseObjectState* controlledObject() { return _controlledObject; };
    bool rotateWithControlledObject() { return _rotateWithControlledObject; }
    void setRotateWithControlledObject(bool rotateWithControlledObject);

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

   signals:
    void stateChanged(GameState* state);
    void controlledObjectIdChanged(int controlledObjectId);
    void controlledObjectChanged(BaseObjectState* controlledObject);
    void controlStateChanged(int objectId, TankControlState* controlState);
    void rotateWithControlledObjectChanged(bool rotateWithControlledObject);
};

#endif  // MAP_VIEW_HPP
