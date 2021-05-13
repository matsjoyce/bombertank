#include "MapView.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QTransform>
#include <QtMath>
#include <QtQml>

std::map<ObjectType, QUrl> spriteFilesForObjectType = {
    {ObjectType::TANK, QUrl(QStringLiteral("qrc:/qml/Sprite/Tank.qml"))},
    {ObjectType::CRATE, QUrl(QStringLiteral("qrc:/qml/Sprite/Crate.qml"))},
    {ObjectType::WALL, QUrl(QStringLiteral("qrc:/qml/Sprite/Wall.qml"))},
    {ObjectType::INDESTRUCTABLE_WALL, QUrl(QStringLiteral("qrc:/qml/Sprite/IndestructableWall.qml"))},
    {ObjectType::SHELL, QUrl(QStringLiteral("qrc:/qml/Sprite/Shell.qml"))}};

std::map<ObjectType, QUrl> inputFilesForObjectType = {
    {ObjectType::TANK, QUrl(QStringLiteral("qrc:/qml/Sprite/TankInput.qml"))}};

MapView::MapView() {
    _timer.start(16);

    connect(&_timer, &QTimer::timeout, this, &MapView::_doUpdate);
}

void MapView::_checkComponentsLoaded() {
    if (!_spriteComponents.size()) {
        auto engine = qmlEngine(this);

        for (auto& [type, url] : spriteFilesForObjectType) {
            auto component = new QQmlComponent(engine, this);
            connect(component, &QQmlComponent::statusChanged, this,
                    [=](QQmlComponent::Status status) { qInfo() << url << status << component->errorString(); });
            component->loadUrl(url);
            _spriteComponents[type] = component;
        }

        for (auto& [type, url] : inputFilesForObjectType) {
            auto component = new QQmlComponent(engine, this);
            connect(component, &QQmlComponent::statusChanged, this,
                    [=](QQmlComponent::Status status) { qInfo() << url << status << component->errorString(); });
            component->loadUrl(url);
            _inputComponents[type] = component;
        }
    }
}

void MapView::setControlledObjectId(int controlledObjectId) {
    if (_controllingId == controlledObjectId) {
        return;
    }
    _controlledObject = nullptr;
    emit controlledObjectChanged(nullptr);
    qDebug() << "Controlling" << controlledObjectId;
    _controllingId = controlledObjectId;
    emit controlledObjectIdChanged(controlledObjectId);
    if (_controls) {
        _controls->deleteLater();
        _controls = nullptr;
    }
}

void MapView::_attachToObject(int id, ObjectType type) {
    qDebug() << "Preparing controller for" << id << "of type" << static_cast<int>(type);
    auto iter = _inputComponents.find(type);
    if (iter == _inputComponents.end() || !iter->second->isReady()) {
        qWarning() << "Could not load input for type since it is not loaded yet or does not exist";
        return;
    }
    _controls = qobject_cast<QQuickItem*>(iter->second->create());
    auto controlsState = qvariant_cast<TankControlState*>(_controls->property("controls"));
    if (!controlsState) {
        qWarning() << "Input does not have a valid controls property";
        return;
    }
    connect(controlsState, &TankControlState::leftTrackChanged, this, &MapView::_handleControlsUpdated);
    connect(controlsState, &TankControlState::rightTrackChanged, this, &MapView::_handleControlsUpdated);
    connect(controlsState, &TankControlState::actionChanged, this, &MapView::_handleControlsUpdated);
}

void MapView::setState(GameState* state) {
    _checkComponentsLoaded();

    if (_state) {
        disconnect(this, &MapView::controlStateChanged, _state, &GameState::setControlState);
    }
    _state = state;
    qInfo() << "State set";
    if (_state) {
        connect(this, &MapView::controlStateChanged, _state, &GameState::setControlState);
    }

    emit stateChanged(_state);
}

void MapView::_handleControlsUpdated() { _controlsUpdated = true; }

void MapView::_controlledObjectDeleted() {
    _controlledObject = nullptr;
    emit controlledObjectChanged(nullptr);
}

void MapView::setRotateWithControlledObject(bool rotateWithControlledObject) {
    if (_rotateWithControlledObject != rotateWithControlledObject) {
        _rotateWithControlledObject = rotateWithControlledObject;
        emit rotateWithControlledObjectChanged(rotateWithControlledObject);
    }
}

void MapView::keyPressEvent(QKeyEvent* event) {
    if (_controls) {
        QCoreApplication::sendEvent(_controls, event);
    }
}
void MapView::keyReleaseEvent(QKeyEvent* event) {
    if (_controls) {
        QCoreApplication::sendEvent(_controls, event);
    }
}

void MapView::_doUpdate() {
    QElapsedTimer timer;
    timer.start();
    auto& snapshot = _state->snapshot();
    auto snapshot_iter = snapshot.begin();
    auto sprites_iter = _sprites.begin();

    if (_controlledObject) {
        _viewCenterX = _controlledObject->x();
        _viewCenterY = _controlledObject->y();
        if (_rotateWithControlledObject) {
            _viewRotation = _controlledObject->rotation();
        }
        else {
            _viewRotation = M_PI_2;
        }
    }

    QTransform viewTransform;
    viewTransform.scale(8, 8).rotateRadians(-_viewRotation + M_PI_2).translate(-_viewCenterX, -_viewCenterY);

    while (snapshot_iter != snapshot.end() || sprites_iter != _sprites.end()) {
        if (snapshot_iter != snapshot.end() &&
            (sprites_iter == _sprites.end() || sprites_iter->first > snapshot_iter->first)) {
            // Added
            auto iter = _spriteComponents.find(snapshot_iter->second->type());
            if (iter == _spriteComponents.end() || !iter->second->isReady()) {
                qWarning() << "Could not create item for" << snapshot_iter->first << "since its type"
                           << static_cast<int>(snapshot_iter->second->type()) << "is not loaded or does not exist";
            }
            else {
                auto a = iter->second->create();
                auto sprite = _sprites[snapshot_iter->first] = qobject_cast<QQuickItem*>(a);
                sprite->setParentItem(this);
            }
            ++snapshot_iter;
        }
        else if (sprites_iter != _sprites.end() &&
                 (snapshot_iter == snapshot.end() || sprites_iter->first < snapshot_iter->first)) {
            // Removed
            delete sprites_iter->second;
            sprites_iter = _sprites.erase(sprites_iter);
        }
        else {
            // Check if we need a controller
            if (_controllingId == snapshot_iter->first) {
                if (!_controls) {
                    _attachToObject(_controllingId, snapshot_iter->second->type());
                }
                if (!_controlledObject) {
                    _controlledObject = snapshot_iter->second.get();
                    connect(_controlledObject, &QObject::destroyed, this, &MapView::_controlledObjectDeleted);
                    emit controlledObjectChanged(_controlledObject);
                }
            }

            // Update
            auto& state = snapshot_iter->second;
            auto sprite = sprites_iter->second;
            auto point = viewTransform.map(QPointF(state->x(), state->y()));
            sprite->setX(point.x() + width() / 2);
            // Translate to pixel coords and angles
            sprite->setY(-point.y() + height() / 2);
            sprite->setRotation(qRadiansToDegrees(-state->rotation() + _viewRotation));
            ++snapshot_iter;
            ++sprites_iter;
        }
    }
    if (_controls && _controlsUpdated) {
        if (_controllingId) {
            emit controlStateChanged(_controllingId, qvariant_cast<TankControlState*>(_controls->property("controls")));
        }
        _controlsUpdated = false;
    }
    // qInfo() << timer.elapsed() << "ms in game loop";
}