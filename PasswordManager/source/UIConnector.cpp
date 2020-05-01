#include "UIConnector.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include <QDebug>

QObject* UIConnector::UIConnector_singletonTypeProvider(QQmlEngine* engine, QJSEngine* scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto connector = new UIConnector();
    return connector;
}

bool UIConnector::decryptFile(QString password) {
    auto key = new std::string(password.toStdString());
    password = "000000000000000000000000000000000000000";
    _manager = new PasswordManager(key);
    return _manager->decryptFile();
}

QJsonObject UIConnector::getData() {
    std::string passwords = _manager->getMyPasswords();
    QString parseData(passwords.c_str());
    QJsonDocument doc = QJsonDocument::fromJson(parseData.toUtf8());
    QJsonObject data;
    data = doc.object();
    return data;
}
