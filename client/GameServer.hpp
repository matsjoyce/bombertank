#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include <QAbstractListModel>
#include <QObject>

#include "GameState.hpp"
#include "common/TcpMessageSocket.hpp"

class ListedGame : public QObject {
    Q_OBJECT

    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

    int _id;
    QString _title;

   public:
    ListedGame(int id, QObject* parent = nullptr) : QObject(parent), _id(id) {}
    int id() const { return _id; }
    QString title() const { return _title; }
    void setTitle(QString title);

   signals:
    void titleChanged(QString title);
};

class ListedGameModel : public QAbstractListModel {
    Q_OBJECT

    std::vector<ListedGame*> _listedGames;

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

    TcpMessageSocket* _msgconn;
    ListedGameModel* _listedGamesModel;
    GameState* _gameState = nullptr;

    void _handleMessage(int id, const Message& msg);

   public:
    GameServer(TcpMessageSocket* msgconn, QObject* parent);
    ListedGameModel* listedGamesModel() const { return _listedGamesModel; }

    Q_INVOKABLE GameState* joinGame(int id);
    Q_INVOKABLE void createGame();
};

#endif  // GAME_SERVER_HPP