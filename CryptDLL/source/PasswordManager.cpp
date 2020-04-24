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

void PasswordManager::start() {
    //init file stream
    auto fileStream = new std::fstream("passwords.psswrds", std::ios::in | std::ios::binary);
    if(!fileStream->good())
        std::cerr << "Error while opening the file, check if it exists" << std::endl;

    //read iv for aes
    fileStream->seekg(0);
    char ivbuf[16 + 1];
    fileStream->read(ivbuf, 16);
    ivbuf[16] = '\0';
    std::string iv(ivbuf);

    //read encrypted file
    fileStream->seekg(0, std::ios::end);

    int encryptedFileSize = (int)fileStream->tellg() - 16;

    fileStream->seekg(16);

    char encryptedFile[encryptedFileSize + 1];

    fileStream->read(encryptedFile, encryptedFileSize);

    fileStream->close();

    encryptedFile[encryptedFileSize] = '\0';

    std::string encryptedFileStr(encryptedFile, encryptedFileSize);

    //decrypt file
    Sleep(100);
    std::string decipheredFile = decrypt(encryptedFileStr, hashedKey, iv);
    Sleep(100);

    //put file into string stream
    auto stringStream = new std::stringstream(std::ios::out | std::ios::in |std::ios::binary);
    stringStream->write(decipheredFile.c_str(), decipheredFile.size());
    stringStream->seekg(0);

    //read & check control number
    int controlNumber;
    stringStream->seekg(0, std::ios::end);
    int size = stringStream->tellg();
    char* raw = new char[size + 1];
    stringStream->seekg(0);
    stringStream->read(raw, size);
    raw[size] = '\0';
    testing1 = std::string(raw, size);
    stringStream->seekg(0);
    stringStream->read((char*)(&controlNumber), 4);

    if(controlNumber == 13371488) {
        std::cerr << "file has been deciphered successfully\n";
        safeToSave = true;

        //read fileinfo
        stringStream->seekg(4);
        stringStream->read((char*)(&fileInfo.passwordsCount), 4);
        stringStream->seekg(8);

        if(fileInfo.passwordsCount == 0)
            std::cerr << "file is empty\n";
        int currentOffset = 8;
        for(int i = 0; i < fileInfo.passwordsCount; i++) {
            //domain
            int domain_size;
            stringStream->read((char*)(&domain_size), 4);
            currentOffset+=4;
            stringStream->seekg(currentOffset);
            char* domain_c_string = new char(domain_size + 1);
            stringStream->read(domain_c_string, domain_size);
            currentOffset+=domain_size;
            stringStream->seekg(currentOffset);
            domain_c_string[domain_size] = '\0';
            std::string domain_string(domain_c_string);

            //login
            int login_size;
            stringStream->read((char*)(&login_size), 4);
            currentOffset+=4;
            stringStream->seekg(currentOffset);
            char* login_c_string = new char(login_size + 1);
            stringStream->read(login_c_string, login_size);
            currentOffset+=login_size;
            stringStream->seekg(currentOffset);
            login_c_string[login_size] = '\0';
            std::string login_string(login_c_string);

            //password
            int password_size;
            stringStream->read((char*)(&password_size), 4);
            currentOffset+=4;
            stringStream->seekg(currentOffset);
            char* password_c_string = new char(password_size + 1);
            stringStream->read(password_c_string, password_size);
            currentOffset+=password_size;
            stringStream->seekg(currentOffset);
            password_c_string[password_size] = '\0';
            std::string password_string(password_c_string);

            auto psswrd = new Password();
            psswrd->domain = domain_string;
            psswrd->login = login_string;
            psswrd->password = password_string;
            passwords.push_back(psswrd);
        }
    }
    else {
        std::cerr << "failed to decypher the file\n";
    }
}

PasswordManager::PasswordManager(std::string *key) {
    //hash the password
    hashedKey = hashKey(key);
}

void PasswordManager::create_blank() {
    std::fstream fileStream("passwords.psswrds", std::ios::out | std::ios::binary | std::ios::trunc);

    std::stringstream stringStream(std::ios::out | std::ios::binary);

    stringStream.seekp(0);
    int controlNumber = 13371488;
    stringStream.write((char*)&controlNumber, 4);
    stringStream.seekp(4);
    int a = 0;
    stringStream.write((char*)&a, 4);
    std::string decryptedFile(stringStream.rdbuf()->str());
    unsigned char ivbuf[16];
    RAND_bytes(ivbuf, 16);
    std::string iv((char*)ivbuf, 16);
    std::string encryptedFile = encrypt(decryptedFile, hashedKey, iv);
    fileStream.seekp(0);
    fileStream.write(iv.c_str(), 16);
    fileStream.seekp(16);
    fileStream.write(encryptedFile.c_str(), encryptedFile.size());
    fileStream.flush();
    fileStream.close();
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
    unsigned char *_iv = (unsigned char *)(iv.c_str());
    unsigned char decryptedFile[256];
    int decryptedFile_len;
    unsigned char * cipheredStr = (unsigned char *)cipheredFile.c_str();
    decryptedFile_len = openssl_decrypt(cipheredStr, cipheredFile.size(), key, _iv, decryptedFile);
    //std::cerr << "decryption done, decrypted file size: " << decryptedFile_len << std::endl;
    decryptedFile[decryptedFile_len] = '\0';
    std::string decryptedFileStr((char*)decryptedFile, decryptedFile_len);
    //std::cerr << "decrypted string: \"" << decryptedFileStr << "\"\n";
    return decryptedFileStr;
}

std::string PasswordManager::encrypt(std::string decryptedFile, unsigned char *key, std::string iv) {
    //std::cerr << "encrypting text: \"" << decryptedFile << "\"\n";
    unsigned char *_iv = (unsigned char *)(iv.c_str());
    unsigned char encryptedFile[256];
    int encryptedFile_len;
    unsigned char * decryptedStr = (unsigned char *)decryptedFile.c_str();
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
    Sleep(100);
    unsigned char* hash = new unsigned char[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, key->c_str(), key->size());
    SHA256_Final(hash, &ctx);
    *key = "000000000000000000000000000";
    delete key;
    key = nullptr;
    return hash;
    Sleep(100);
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
    for(int i = 0; i < passwords.size(); i++) {
        if(passwords[i]->domain == domain) {
            std::cerr << "Password for this domain already exists\n";
            return false;
        }
    }
    auto psswrd = new Password();
    psswrd->domain = domain;
    psswrd->login = login;
    psswrd->password = password;
    passwords.push_back(psswrd);
    needToSave = true;
    return true;
};

void PasswordManager::finish() {
    if(safeToSave && needToSave) {
        Sleep(100);
        std::fstream fileStream("passwords.psswrds", std::ios::out | std::ios::binary | std::ios::trunc);
        std::stringstream stringStream(std::ios::out | std::ios::binary);
        stringStream.seekp(0);
        int controlNumber = 13371488;
        stringStream.write((char *) &controlNumber, 4);
        stringStream.seekp(4);
        int a = passwords.size();
        stringStream.write((char *) &a, 4);
        stringStream.seekp(8);

        int currentOffset = 8;
        for (int i = 0; i < passwords.size(); i++) {
            auto psswrd = passwords[i];

            int domain_size = psswrd->domain.size();
            stringStream.write((char *) &domain_size, 4);
            currentOffset += 4;
            stringStream.seekp(currentOffset);

            stringStream.write(psswrd->domain.c_str(), domain_size);
            currentOffset += domain_size;
            stringStream.seekp(currentOffset);

            int login_size = psswrd->login.size();
            stringStream.write((char *) &login_size, 4);
            currentOffset += 4;
            stringStream.seekp(currentOffset);

            stringStream.write(psswrd->login.c_str(), login_size);
            currentOffset += login_size;
            stringStream.seekp(currentOffset);

            int password_size = psswrd->password.size();
            stringStream.write((char *) &password_size, 4);
            currentOffset += 4;
            stringStream.seekp(currentOffset);

            stringStream.write(psswrd->password.c_str(), password_size);
            currentOffset += password_size;
            stringStream.seekp(currentOffset);

            fileStream.flush();
        }
        testing2 = stringStream.rdbuf()->str();
        if (testing1 != testing2)
            std::cerr << "ALARMALARMALARM";

        std::string decryptedFile(stringStream.rdbuf()->str());

        unsigned char ivbuf[16];
        RAND_bytes(ivbuf, 16);
        std::string iv((char *) ivbuf, 16);

        std::string encryptedFile = encrypt(decryptedFile, hashedKey, iv);

        delete hashedKey;
        hashedKey = nullptr;

        fileStream.seekp(0);
        fileStream.write(iv.c_str(), 16);
        fileStream.seekp(16);
        fileStream.write(encryptedFile.c_str(), encryptedFile.size());
        fileStream.flush();
        fileStream.close();
        Sleep(100);
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
    std::cerr << "\n";
    std::cerr << "--------------------------------------------\n";
    std::cerr << "\n";
    for (int i = 0; i < passwords.size(); i++) {
        auto psswrd = passwords[i];
        std::cerr << psswrd->domain << " | " << psswrd->login << " | " << psswrd->password << "\n";
    }
    std::cerr << "\n";
    std::cerr << "--------------------------------------------\n";
    std::cerr << "\n";
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
