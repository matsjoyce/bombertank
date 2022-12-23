#include "MapView.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QTransform>
#include <QtMath>
#include <QtQml>
#include <QQuickWindow>
#include <QSGImageNode>

std::map<constants::ObjectType, QUrl> spriteFilesForObjectType = {
    {constants::ObjectType::TANK, QUrl(QStringLiteral("qrc:/qml/Sprite/Tank.qml"))},
    {constants::ObjectType::CRATE, QUrl(QStringLiteral("qrc:/qml/Sprite/Crate.qml"))},
    {constants::ObjectType::WALL, QUrl(QStringLiteral("qrc:/qml/Sprite/Wall.qml"))},
    {constants::ObjectType::INDESTRUCTABLE_WALL, QUrl(QStringLiteral("qrc:/qml/Sprite/IndestructableWall.qml"))},
    {constants::ObjectType::SHELL, QUrl(QStringLiteral("qrc:/qml/Sprite/Shell.qml"))},
    {constants::ObjectType::MG_SHELL, QUrl(QStringLiteral("qrc:/qml/Sprite/MGShell.qml"))},
    {constants::ObjectType::ROCKET, QUrl(QStringLiteral("qrc:/qml/Sprite/Rocket.qml"))},
    {constants::ObjectType::START_ZONE, QUrl(QStringLiteral("qrc:/qml/Sprite/StartZone.qml"))},
    {constants::ObjectType::EXPLOSION, QUrl(QStringLiteral("qrc:/qml/Sprite/Explosion.qml"))},
    {constants::ObjectType::BOMB, QUrl(QStringLiteral("qrc:/qml/Sprite/Bomb.qml"))},
    {constants::ObjectType::LASER_TURRET, QUrl(QStringLiteral("qrc:/qml/Sprite/LaserTurret.qml"))},
    {constants::ObjectType::MG_TURRET, QUrl(QStringLiteral("qrc:/qml/Sprite/MGTurret.qml"))},
    {constants::ObjectType::LASER, QUrl(QStringLiteral("qrc:/qml/Sprite/Laser.qml"))},
    {constants::ObjectType::HOMING_ROCKET, QUrl(QStringLiteral("qrc:/qml/Sprite/Rocket.qml"))},
};

std::map<constants::ObjectType, QUrl> inputFilesForObjectType = {
    {constants::ObjectType::TANK, QUrl(QStringLiteral("qrc:/qml/Sprite/TankInput.qml"))}
};


MapView::MapView() {
    _timer.start(16);
    _viewRotProp.setValue(M_PI_2);

    connect(&_timer, &QTimer::timeout, this, &MapView::_doUpdate);
}

void MapView::_checkComponentsLoaded() {
    if (!_spriteComponents.size()) {
        auto engine = qmlEngine(this);

        for (auto& [type, url] : spriteFilesForObjectType) {
            auto component = new QQmlComponent(engine, this);
            auto urlCopy = url; // For clang
            connect(component, &QQmlComponent::statusChanged, this,
                    [=](QQmlComponent::Status status) { qInfo() << urlCopy << status << component->errorString(); });
            component->loadUrl(url);
            _spriteComponents[type] = component;
        }

        for (auto& [type, url] : inputFilesForObjectType) {
            auto component = new QQmlComponent(engine, this);
            auto urlCopy = url; // For clang
            connect(component, &QQmlComponent::statusChanged, this,
                    [=](QQmlComponent::Status status) { qInfo() << urlCopy << status << component->errorString(); });
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

void MapView::_attachToObject(int id, BaseObjectState* obj) {
    qDebug() << "Preparing controller for" << id << "of type" << static_cast<int>(obj->type());
    auto iter = _inputComponents.find(obj->type());
    if (iter == _inputComponents.end() || !iter->second->isReady()) {
        qWarning() << "Could not load input for type since it is not loaded yet or does not exist";
        return;
    }
    auto engine = qmlEngine(this);
    auto context = new QQmlContext(engine->rootContext(), this);
    context->setContextProperty("object", obj);
    _controls = qobject_cast<QQuickItem*>(iter->second->create(context));
    if (!_controls) {
        qWarning() << "Could not create input";
        return;
    }
    _controls->setParentItem(this);
    auto controlsState = qvariant_cast<TankControlState*>(_controls->property("controls"));
    if (!controlsState) {
        qWarning() << "Input does not have a valid controls property";
        return;
    }
    connect(controlsState, &TankControlState::controlsChanged, this, &MapView::_handleControlsUpdated);
}

void MapView::setState(BaseGameState* state) {
    _checkComponentsLoaded();

    if (auto controllableState = dynamic_cast<GameState*>(_state)) {
        disconnect(this, &MapView::controlStateChanged, controllableState, &GameState::setControlState);
    }
    for (auto& [_, sprite] : _sprites) {
        delete sprite.item;
    }
    _sprites.clear();
    _state = state;
    qInfo() << "State set";
    if (auto controllableState = dynamic_cast<GameState*>(_state)) {
        connect(this, &MapView::controlStateChanged, controllableState, &GameState::setControlState);
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

QTransform MapView::_viewTransform() {
    QTransform viewTransform;
    viewTransform.scale(8, 8).rotateRadians(-_viewRotProp.value() + M_PI_2).translate(-_viewCenterProp.value().x(), -_viewCenterProp.value().y());
    return viewTransform;
}

void MapView::_doUpdate() {
    if (!_state) return;
    QElapsedTimer timer;
    timer.start();
    auto engine = qmlEngine(this);
    auto& snapshot = _state->snapshot();
    auto snapshot_iter = snapshot.begin();
    auto sprites_iter = _sprites.begin();

    if (_controlledObject) {
        _viewCenterProp.setValue({_controlledObject->x(), _controlledObject->y()});
        if (_rotateWithControlledObject) {
            _viewRotProp.setValue(_controlledObject->rotation());
        }
    }

    QTransform viewTransform = _viewTransform();

    while (snapshot_iter != snapshot.end() || sprites_iter != _sprites.end()) {
        if (snapshot_iter != snapshot.end() &&
            (sprites_iter == _sprites.end() || sprites_iter->first > snapshot_iter->first)) {
            // Added
            auto iter = _spriteComponents.find(snapshot_iter->second->type());
            if (iter == _spriteComponents.end() || !iter->second->isReady()) {
                // qWarning() << "Could not create item for" << snapshot_iter->first << "since its type"
                //            << static_cast<int>(snapshot_iter->second->type()) << "is not loaded or does not exist";
            }
            else {
                auto context = new QQmlContext(engine->rootContext(), this);
                context->setContextProperty("object", snapshot_iter->second.get());
                auto a = qobject_cast<QQuickItem*>(iter->second->create(context));
                if (!a) {
                    qWarning() << "Could not create item, null was returned";
                }
                else {
                    _sprites.emplace(snapshot_iter->first, MapView::SpriteDetails{a, snapshot_iter->second});
                    a->setParentItem(this);
                }
            }
            ++snapshot_iter;
        }
        else if (sprites_iter != _sprites.end() &&
                 (snapshot_iter == snapshot.end() || sprites_iter->first < snapshot_iter->first)) {
            // Removed
            if (sprites_iter->second.item->property("removable").toBool()) {
                delete sprites_iter->second.item;
                sprites_iter = _sprites.erase(sprites_iter);
            }
            else {
                ++sprites_iter;
            }
        }
        else {
            // Check if we need a controller
            if (_controllingId == snapshot_iter->first) {
                if (!_controls) {
                    _attachToObject(_controllingId, snapshot_iter->second.get());
                }
                if (!_controlledObject) {
                    _controlledObject = snapshot_iter->second.get();
                    connect(_controlledObject, &QObject::destroyed, this, &MapView::_controlledObjectDeleted);
                    emit controlledObjectChanged(_controlledObject);
                }
            }

            if (snapshot_iter->second != sprites_iter->second.object) {
                qWarning() << "Item object is not the object from the snapshot!";
            }
            ++snapshot_iter;
            ++sprites_iter;
        }
    }
    for (auto& spriteDetails : _sprites) {
        // Update
        auto& state = spriteDetails.second.object;
        auto sprite = spriteDetails.second.item;
        auto point = viewTransform.map(QPointF(state->x(), state->y()));
        sprite->setX(point.x() + width() / 2);
        // Translate to pixel coords and angles
        sprite->setY(-point.y() + height() / 2);
        sprite->setRotation(qRadiansToDegrees(-state->rotation() + _viewRotProp.value()));
    }
    if (_controls && _controlsUpdated) {
        if (_controllingId) {
            emit controlStateChanged(_controllingId, qvariant_cast<TankControlState*>(_controls->property("controls")));
        }
        _controlsUpdated = false;
    }
    // qInfo() << timer.elapsed() << "ms in game loop";
    _state->cleanup();
}

QPointF MapView::pixelsToPosition(QPointF pos) {
    bool invertible;
    auto conv = _viewTransform().inverted(&invertible).map(QPointF(pos.x() - width() / 2, -pos.y() + height() / 2));
    if (!invertible) {
        throw std::runtime_error("Cannot invert view matrix???");
    }
    return {conv.x(), conv.y()};
}

QPointF MapView::positionToPixels(QPointF pos) {
    auto conv = _viewTransform().map(pos);
    return {conv.x() + width() / 2, -conv.y() + height() / 2};
}
