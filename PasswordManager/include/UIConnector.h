#include <QObject>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QJsonObject>
#include "PasswordManager.h"

#ifndef PASSWORDMANAGER_UICONNECTOR_H
#define PASSWORDMANAGER_UICONNECTOR_H

class UIConnector : public QObject {
    Q_OBJECT
public:
    explicit UIConnector(QObject* parent = nullptr){};
    static QObject* UIConnector_singletonTypeProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    Q_INVOKABLE bool decryptFile(QString password);
    Q_INVOKABLE QJsonObject getData();
    //Q_INVOKABLE bool serialize(QJsonObject);
protected:
    PasswordManager* _manager = nullptr;

};

#endif //PASSWORDMANAGER_UICONNECTOR_H
