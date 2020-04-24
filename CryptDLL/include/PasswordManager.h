#include <string>
#include <vector>

class PasswordManager {
public:
    explicit PasswordManager(std::string *key);
    std::string getRandomString(int length);
    void viewMyPasswords();
    //std::string getPasswordFor(std::string domain);
    //std::string viewRaw();
    void create_blank();
    static void demo2();
    bool insertPassword(std::string domain, std::string login, std::string password);
    //bool deletePassword(int i);
    void start();
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

    std::string testing1;

    std::string testing2;

    struct Password {
        std::string domain;
        std::string login;
        std::string password;
    };
    struct FileInfo {
        int controlNumberSize = 4;
        int fileSize;
        int fileSizeOffset = controlNumberSize;
        int passwordsCount;
        int passwordsCountOffset = fileSizeOffset + 4;
        int firstPasswordCountOffset = passwordsCountOffset + 4;
    };
    unsigned char* hashedKey = nullptr;
    FileInfo fileInfo;
    std::vector<Password*> passwords;
};
