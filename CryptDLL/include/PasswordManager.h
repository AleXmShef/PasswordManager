#include <string>
#include <vector>
#include "rapidjson/document.h"

#ifndef PASSWORDMANAGER_PASSWORDMANAGER_H
#define PASSWORDMANAGER_PASSWORDMANAGER_H

class PasswordManager {
public:
    explicit PasswordManager(std::string *key);
    std::string getRandomString(int length);
    void viewMyPasswords();
    std::string getMyPasswords();
    //std::string getPasswordFor(std::string domain);
    //std::string viewRaw();
    void create_blank();
    static void demo2();
    bool insertPassword(std::string domain, std::string login, std::string password);
    //bool deletePassword(int i);
    bool decryptFile();
    void finish();
    //void test();
private:
    bool safeToSave = false;
    bool needToSave = false;
    int openssl_decrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key,
                unsigned char* iv, unsigned char* ciphertext);
    int openssl_encrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                unsigned char *iv, unsigned char *plaintext);
    std::string decrypt(std::string cipheredText, unsigned char *key, std::string iv);
    std::string encrypt(std::string decipheredText, unsigned char *key, std::string iv);

    std::string base64encode(unsigned char* bytes, int length);

    unsigned char* hashKey(std::string *key);

    rapidjson::Document* _json;
    unsigned char* hashedKey = nullptr;
};

#endif //PASSWORDMANAGER_PASSWORDMANAGER_H
