#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>
#include "PasswordManager.h"
#include "UIConnector.h"

std::string readMessage() {
    int length = 0;
    for (int i = 0; i < 4; i++)
    {
        unsigned int read_char = getchar();
        length = length | (read_char << i*8);
    }

    //read the json-message
    std::string msg = "";
    for (int i = 0; i < length; i++)
    {
        msg += getchar();
    }
    return msg;
}

void sendMessage(std::string message) {
    // Collect the length of the message
    unsigned int len = message.length();
    std::cout   << char(len>>0)
                << char(len>>8)
                << char(len>>16)
                << char(len>>24);

    std::cout << message << std::flush;
}

void connectToExtension() {
    auto message = readMessage();

    sendMessage(message);
}

void runAsConsole(int argc, char *argv[]) {
    if(argc < 2) {
        //show help
    }
    else {
        if(std::string(argv[1]) == "chrome-extension://nnhfgfppmfpoecmfondglkcodbagmnoc/") {
            //call from extension
            connectToExtension();
        }
        else if (std::string(argv[1]) == "-p" && argc > 2){
            //call from cmd
            std::string* key = new std::string(argv[2]);
            argv[2] = (char*)"000000000000000000000000000000000000000";
            PasswordManager passwordManager(key);
            if(argc > 3) {
                if(argv[3] == std::string("-a") && argc > 6) {
                    std::string domain = argv[4];
                    std::string login = argv[5];
                    std::string password = argv[6];
                    passwordManager.decryptFile();
                    passwordManager.insertPassword(domain, login, password);
                    passwordManager.finish();
                }
                else if(argv[3] == std::string("-v")) {
                    passwordManager.decryptFile();
                    passwordManager.viewMyPasswords();
                    passwordManager.finish();
                }
                else if(argv[3] == std::string("--blank")) {
                    passwordManager.create_blank();
                }
                else if(argv[3] == std::string("-r") && argc > 4) {
                    int length = atoi(argv[4]);
                    std::cerr << passwordManager.getRandomString(length) << "\n";
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc > 1) {
        runAsConsole(argc, argv);
        return 0;
    }
    else {
        qmlRegisterSingletonType<UIConnector>("app", 1, 0, "UIConnector", UIConnector::UIConnector_singletonTypeProvider);

        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

        QGuiApplication app(argc, argv);

        QQmlApplicationEngine engine;
        const QUrl url(QStringLiteral("qrc:/main.qml"));
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         &app, [url](QObject *obj, const QUrl &objUrl) {
                    if (!obj && url == objUrl)
                        QCoreApplication::exit(-1);
                }, Qt::QueuedConnection);
        engine.load(url);

        return app.exec();
    }
}
