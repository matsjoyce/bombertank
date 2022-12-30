#ifndef MAP_VIEW_HPP
#define MAP_VIEW_HPP

#include <QQmlComponent>
#include <QQuickItem>
#include <QTimer>
#include <QImage>
#include <QQmlEngine>

#include "GameState.hpp"
#include "common/Constants.hpp"
#include "objects/TankControl.hpp"

class AppContext;

class MapView : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(BaseGameState* state READ state WRITE setState NOTIFY stateChanged)
    Q_MOC_INCLUDE("AppContext.hpp")
    Q_PROPERTY(AppContext* context READ context WRITE setContext NOTIFY contextChanged)
    Q_PROPERTY(
        int controlledObjectId READ controlledObjectId WRITE setControlledObjectId NOTIFY controlledObjectIdChanged)
    Q_PROPERTY(BaseObjectState* controlledObject READ controlledObject NOTIFY controlledObjectChanged)
    Q_PROPERTY(bool rotateWithControlledObject READ rotateWithControlledObject WRITE setRotateWithControlledObject
                   NOTIFY rotateWithControlledObjectChanged)
    Q_PROPERTY(QPointF viewCenter READ viewCenter WRITE setViewCenter BINDABLE bindableViewCenter NOTIFY viewCenterChanged)
    Q_PROPERTY(float viewRotation READ viewRotation WRITE setViewRotation BINDABLE bindableViewRotation NOTIFY viewRotationChanged)
    QML_ELEMENT

    QTimer _timer;
    BaseGameState* _state = nullptr;
    AppContext* _context = nullptr;
    std::map<int, QQmlComponent*> _spriteComponents;
    std::map<int, QQmlComponent*> _inputComponents;

    struct SpriteDetails {
        QQuickItem* item;
        std::shared_ptr<BaseObjectState> object;
    };

    std::map<int, SpriteDetails> _sprites;
    QQuickItem* _controls = nullptr;
    BaseObjectState* _controlledObject = nullptr;
    bool _controlsUpdated = false;
    int _controllingId = 0;
    bool _rotateWithControlledObject = false;

    void _doUpdate();
    void _handleControlsUpdated();
    void _attachToObject(int id, BaseObjectState* obj);
    void _controlledObjectDeleted();
    QTransform _viewTransform();

   public:
    MapView();
    BaseGameState* state() { return _state; }
    void setState(BaseGameState* state);

    AppContext* context() { return _context; }
    void setContext(AppContext* context);

    int controlledObjectId() { return _controllingId; }
    void setControlledObjectId(int controlledObjectId);
    BaseObjectState* controlledObject() { return _controlledObject; };
    bool rotateWithControlledObject() { return _rotateWithControlledObject; }
    void setRotateWithControlledObject(bool rotateWithControlledObject);

    QPointF viewCenter() const { return _viewCenterProp.value(); }
    void setViewCenter(QPointF pos) { return _viewCenterProp.setValue(pos); }
    QBindable<QPointF> bindableViewCenter() const { return &_viewCenterProp; }

    float viewRotation() const { return _viewRotProp.value(); }
    void setViewRotation(float rot) { return _viewRotProp.setValue(rot); }
    QBindable<float> bindableViewRotation() const { return &_viewRotProp; }

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    Q_INVOKABLE QPointF pixelsToPosition(QPointF pos);
    Q_INVOKABLE QPointF positionToPixels(QPointF pos);

signals:
    void stateChanged(BaseGameState* state);
    void contextChanged(AppContext* state);
    void controlledObjectIdChanged(int controlledObjectId);
    void controlledObjectChanged(BaseObjectState* controlledObject);
    void controlStateChanged(int objectId, TankControlState* controlState);
    void rotateWithControlledObjectChanged(bool rotateWithControlledObject);
    void viewCenterChanged(QPointF pos);
    void viewRotationChanged(float rot);

private:
    Q_OBJECT_BINDABLE_PROPERTY(MapView, QPointF, _viewCenterProp, &MapView::viewCenterChanged)
    Q_OBJECT_BINDABLE_PROPERTY(MapView, float, _viewRotProp, &MapView::viewRotationChanged)
};

#endif  // MAP_VIEW_HPP
