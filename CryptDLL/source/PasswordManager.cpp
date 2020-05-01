#include "PasswordManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <algorithm>
#include "string.h"
#include "windows.h"
#include "openssl/conf.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "openssl/sha.h"
#include "openssl/bio.h"
#include "openssl/buffer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

bool PasswordManager::decryptFile() {
    //init file stream
    auto fileStream = new std::fstream("passwords.psswrds", std::ios::in | std::ios::binary);
    if(!fileStream->good())
        std::cerr << "Error while opening the file, check if it exists" << std::endl;

    //read iv for aes
    fileStream->seekg(0);
    char ivbuf[16];
    fileStream->read(ivbuf, 16);
    std::string iv(ivbuf, 16);

    //read encrypted file
    fileStream->seekg(0, std::ios::end);

    int encryptedFileSize = (int)fileStream->tellg() - 16;

    fileStream->seekg(16);

    char encryptedFile[encryptedFileSize];

    fileStream->read(encryptedFile, encryptedFileSize);

    fileStream->close();

    std::string encryptedFileStr(encryptedFile, encryptedFileSize);

    //decrypt file
    std::string decipheredFile = decrypt(encryptedFileStr, hashedKey, iv);

    //std::cerr << "decipheredFile: " << decipheredFile << "\n";
    _json = new rapidjson::Document;
    _json->Parse(decipheredFile.c_str());
    if(_json->IsObject()) {
        std::cerr << "file decrypted successfully\n";
        safeToSave = true;
        return true;
    }
    else {
        std::cerr << "failed to decrypt the file\n";
        return false;
    }
}

PasswordManager::PasswordManager(std::string *key) {
    //hash the password
    hashedKey = hashKey(key);
}

void PasswordManager::create_blank() {
    std::fstream fileStream("passwords.psswrds", std::ios::out | std::ios::binary | std::ios::trunc);
//    std::fstream fileStream2("debug.bin", std::ios::out | std::ios::binary | std::ios::trunc);

    rapidjson::Document json;
    json.SetObject();
    json.AddMember("password_count", 0, json.GetAllocator());
    rapidjson::Value arr;
    arr.SetArray();
    json.AddMember("passwords", arr, json.GetAllocator());
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    std::string decryptedFile = buffer.GetString();
    std::cerr << "JSON: " << decryptedFile << "\n";

    unsigned char ivbuf[16];
    RAND_bytes(ivbuf, 16);
    std::string iv((char*)ivbuf, 16);
    std::string encryptedFile = encrypt(decryptedFile, hashedKey, iv);
    decryptedFile = decrypt(encryptedFile, hashedKey, iv);
    std::cerr << "Hello: " << decryptedFile << "\n";
    fileStream.seekp(0);
    fileStream.write(iv.c_str(), 16);
    //fileStream << iv;
    fileStream.seekp(16);
    fileStream << encryptedFile;
    //fileStream.write(encryptedFile.c_str(), encryptedFile.size());
    fileStream.flush();
    fileStream.close();

/*    fileStream2 << iv;
    fileStream2 << hashedKey;
    fileStream2.flush();
    fileStream2.close();*/
}

int PasswordManager::openssl_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;
    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int PasswordManager::openssl_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

std::string PasswordManager::decrypt(std::string cipheredFile, unsigned char *key, std::string iv) {
    //std::cerr << "decrypting text: \"" << cipheredFile << "\"\n";
    auto _iv = (unsigned char *)(iv.c_str());
    unsigned char decryptedFile[4096];
    int decryptedFile_len;
    auto cipheredStr = (unsigned char *)cipheredFile.c_str();
    decryptedFile_len = openssl_decrypt(cipheredStr, cipheredFile.size(), key, _iv, decryptedFile);
    //std::cerr << "decryption done, decrypted file size: " << decryptedFile_len << std::endl;
    decryptedFile[decryptedFile_len] = '\0';
    std::string decryptedFileStr((char*)decryptedFile, decryptedFile_len);
    //std::cerr << "decrypted string: \"" << decryptedFileStr << "\"\n";
    return decryptedFileStr;
}

std::string PasswordManager::encrypt(std::string decryptedFile, unsigned char *key, std::string iv) {
    //std::cerr << "encrypting text: \"" << decryptedFile << "\"\n";
    auto _iv = (unsigned char *)(iv.c_str());
    unsigned char encryptedFile[4096];
    int encryptedFile_len;
    auto decryptedStr = (unsigned char *)decryptedFile.c_str();
    encryptedFile_len = openssl_encrypt(decryptedStr, decryptedFile.size(), key, _iv, encryptedFile);
    //std::cerr << "encryption done, encrypted file size: " << encryptedFile_len << std::endl;
    encryptedFile[encryptedFile_len] = '\0';
    //std::cerr << "encrypted file: " << encryptedFile << std::endl;
    std::string encryptedFileStr((char*)encryptedFile, encryptedFile_len);
    //std::cerr << "encrypted string: \"" << encryptedFileStr << "\"\n";
    //std::cerr << "encrypted string length: " << encryptedFileStr.size() << std::endl;
    return encryptedFileStr;
}

unsigned char* PasswordManager::hashKey(std::string *key) {
    auto hash = new unsigned char[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, key->c_str(), key->size());
    SHA256_Final(hash, &ctx);
    *key = "000000000000000000000000000";
    delete key;
    key = nullptr;
    return hash;
}

void PasswordManager::demo2() {
    /*unsigned char *key = (unsigned char *)"01234561230123456789012345678901";
    unsigned char *iv = (unsigned char *)"0123456789012345";
    unsigned char *plaintext =
        (unsigned char *)"The quick brown fox jumps over the lazy dog";
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];
    int decryptedtext_len, ciphertext_len;
    ciphertext_len = encrypt (plaintext, strlen ((char *)plaintext), key, iv,
                              ciphertext);
    printf("Ciphertext is:\n");
    for(int i = 0; i < ciphertext_len; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)ciphertext[i] << " ";
        if((i + 1)%6==0)
            std::cout << std::endl;
    }
    decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
                                decryptedtext);
    decryptedtext[decryptedtext_len] = '\0';*/
}

bool PasswordManager::insertPassword(std::string domain, std::string login, std::string password) {

    rapidjson::Value credentials;
    credentials.SetObject();

    rapidjson::Value _domain(domain.c_str(), domain.size());
    rapidjson::Value _login(login.c_str(), login.size());
    rapidjson::Value _password(password.c_str(), password.size());


    credentials.AddMember("domain", _domain, _json->GetAllocator());
    credentials.AddMember("login", _login, _json->GetAllocator());
    credentials.AddMember("password", _password, _json->GetAllocator());

    (*_json)["passwords"].GetArray().PushBack(credentials, _json->GetAllocator());
    needToSave = true;
    return true;
}

void PasswordManager::finish() {
    if(safeToSave && needToSave) {
        std::fstream fileStream("passwords.psswrds", std::ios::out | std::ios::binary | std::ios::trunc);

        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        _json->Accept(writer);
        std::string decryptedFile = buffer.GetString();
        //std::cerr << "JSON: " << decryptedFile << "\n";

        unsigned char ivbuf[16];
        RAND_bytes(ivbuf, 16);
        std::string iv((char *) ivbuf, 16);

        std::string encryptedFile = encrypt(decryptedFile, hashedKey, iv);

        delete hashedKey;
        hashedKey = nullptr;

        fileStream.seekp(0);
        fileStream << iv;
        fileStream << encryptedFile;
        fileStream.flush();
        fileStream.close();
    }
}

std::string PasswordManager::base64encode(unsigned char *bytes, int length) {
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, bytes, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char *buff = (char *)malloc(bptr->length);
    memcpy(buff, bptr->data, bptr->length-1);
    buff[bptr->length-1] = 0;

    BIO_free_all(b64);

    return buff;
}

void PasswordManager::viewMyPasswords() {
    rapidjson::Value arr = (*_json)["passwords"].GetArray();
    if(arr.Size() > 0) {
        std::cerr << "\n";
        std::cerr << "--------------------------------------------\n";
        std::cerr << "\n";
        for (int i = 0; i < arr.Size(); i++) {

            std::string domain = arr[i]["domain"].GetString();
            std::string login = arr[i]["login"].GetString();
            std::string password = arr[i]["password"].GetString();

            std::cerr << domain << " | " << login << " | " << password << "\n";
        }
        std::cerr << "\n";
        std::cerr << "--------------------------------------------\n";
        std::cerr << "\n";
    }
    else
        std::cerr << "Empty\n";
}

std::string PasswordManager::getRandomString(int length) {
    unsigned char bytes[3*(length/4)];
    RAND_bytes(bytes, 3*(length/4));
    for(int i = 0; i < 3*(length/4); i++) {
        std::cerr << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i] << std::dec << " ";
        //std::cerr << bytes;
    }
    std::cerr << "\n";
    auto password = base64encode(bytes, 3*(length/4));
    return password;
}

std::string PasswordManager::getMyPasswords() {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    _json->Accept(writer);
    std::string passwords = buffer.GetString();
    return passwords;
}
