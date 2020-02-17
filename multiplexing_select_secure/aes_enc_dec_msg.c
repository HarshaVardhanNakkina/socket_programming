#include <stdio.h>
#include <string.h>

// #include <cryptlib.h>
// #include <bio_lcl.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>


int encrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
int decrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);

void handleErrors(void) {
  ERR_print_errors_fp(stderr);
  abort();
}

int main(int argc, char const *argv[]) {
  
  unsigned char *key = (unsigned char *) "012345678901234567890123456789";
  unsigned char *iv = (unsigned char *) "0123456789012345";

  unsigned char *plaintext = (unsigned char *) "the quick brown fox jumpes over a lazy dog";

  unsigned char ciphertext[128], decryptedtext[128];

  int decryptedtext_len, ciphertext_len;

  ciphertext_len = encrypt(plaintext, strlen((char*) plaintext), key, iv, ciphertext);

  printf("Ciphertext is: \n");
  BIO_dump_fp(stdout, (const char *) ciphertext, ciphertext_len);

  decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

  decryptedtext[decryptedtext_len] = '\0';

  printf("Decrypted text is: %s\n", decryptedtext);

  return 0;
}

int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext) {


  EVP_CIPHER_CTX *ctx;

  int len, ciphertext_len;

  if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  
  ciphertext_len = len;

  if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    handleErrors();

  ciphertext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {

  EVP_CIPHER_CTX *ctx;

  int len, plaintext_len;

  if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();

  plaintext_len = len;

  if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    handleErrors();

  plaintext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}
