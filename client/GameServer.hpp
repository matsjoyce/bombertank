#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <QAbstractListModel>
#include <QObject>
#include <QUrl>
#include <QQmlEngine>

#include "GameState.hpp"
#include "common/TcpMessageSocket.hpp"

constexpr auto IdRole = Qt::UserRole;
constexpr auto TitleRole = Qt::UserRole + 1;

struct ListedGame {
    int id;
    QString title;
};

class ListedGameModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

    std::vector<ListedGame> _listedGames;

   public:
    using QAbstractListModel::QAbstractListModel;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

   private:
    void _removeGame(int id);
    void _updateGame(int id, QString title);
    friend class GameServer;
};

class GameServer : public QObject {
    Q_OBJECT
    Q_PROPERTY(ListedGameModel* listedGamesModel READ listedGamesModel CONSTANT)
    Q_PROPERTY(int connectedCount READ connectedCount BINDABLE connectedCountBindable)
    QML_ELEMENT
    QML_UNCREATABLE("Uncreatable!")

    TcpMessageSocket* _msgconn;
    ListedGameModel* _listedGamesModel;
    GameState* _gameState = nullptr;

    Q_OBJECT_BINDABLE_PROPERTY(GameServer, int, _connectedCountProp)

    void _handleMessage(int id, const Message& msg);

   public:
    GameServer(TcpMessageSocket* msgconn, QObject* parent);
    ListedGameModel* listedGamesModel() const { return _listedGamesModel; }

    int connectedCount() const { return _connectedCountProp.value(); }
    QBindable<int> connectedCountBindable() { return &_connectedCountProp; }

    Q_INVOKABLE GameState* joinGame(int id, std::vector<int> modulesForSlots);
    Q_INVOKABLE void createGame(QUrl mapFilePath);
    Q_INVOKABLE void disconnect();
};

#endif  // GAME_SERVER_HPP
