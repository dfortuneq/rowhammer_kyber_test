/* Deterministic randombytes by Daniel J. Bernstein */
/* taken from SUPERCOP (https://bench.cr.yp.to)     */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "kem.h"
#include "randombytes.h"

#define NTESTS 1


static uint32_t seed[32] = {
  3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,2,3,8,4,6,2,6,4,3,3,8,3,2,7,9,5
};
static uint32_t in[12];
static uint32_t out[8];
static int outleft = 0;

#define ROTATE(x,b) (((x) << (b)) | ((x) >> (32 - (b))))
#define MUSH(i,b) x = t[i] += (((x ^ seed[i]) + sum) ^ ROTATE(x,b));

static void surf(void)
{
  uint32_t t[12]; uint32_t x; uint32_t sum = 0;
  int r; int i; int loop;

  for (i = 0;i < 12;++i) t[i] = in[i] ^ seed[12 + i];
  for (i = 0;i < 8;++i) out[i] = seed[24 + i];
  x = t[11];
  for (loop = 0;loop < 2;++loop) {
    for (r = 0;r < 16;++r) {
      sum += 0x9e3779b9;
      MUSH(0,5) MUSH(1,7) MUSH(2,9) MUSH(3,13)
      MUSH(4,5) MUSH(5,7) MUSH(6,9) MUSH(7,13)
      MUSH(8,5) MUSH(9,7) MUSH(10,9) MUSH(11,13)
    }
    for (i = 0;i < 8;++i) out[i] ^= t[i + 4];
  }
}

void randombytes(uint8_t *x,size_t xlen)
{
  while (xlen > 0) {
    if (!outleft) {
      if (!++in[0]) if (!++in[1]) if (!++in[2]) ++in[3];
      surf();
      outleft = 8;
    }
    *x = out[--outleft];
    printf("%02x", *x);
    ++x;
    --xlen;
  }
  printf("\n");
}

// 新增文件操作函数
static int write_to_file(const char *filename, const uint8_t *data, size_t len) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to open file for writing");
        return -1;
    }
    size_t written = fwrite(data, 1, len, fp);
    fclose(fp);
    return (written == len) ? 0 : -1;
}

static int read_from_file(const char *filename, uint8_t *buf, size_t len) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open file for reading");
        return -1;
    }
    size_t read = fread(buf, 1, len, fp);
    fclose(fp);
    return (read == len) ? 0 : -1;
}

int main(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
  uint8_t key_a[CRYPTO_BYTES];
  uint8_t key_b[CRYPTO_BYTES];

    // // Key-pair generation
    // crypto_kem_keypair(pk, sk);
    // // 存储公私钥
    // if(write_to_file("pk_1.bin", pk, CRYPTO_PUBLICKEYBYTES) ||
    //    write_to_file("sk_1.bin", sk, CRYPTO_SECRETKEYBYTES)) {
    //     return -1;
    // }

    // // Encapsulation
    // crypto_kem_enc(ct, key_b, pk);
    // // 存储密文和共享密钥
    // if(write_to_file("ct_1.bin", ct, CRYPTO_CIPHERTEXTBYTES) ||
    //    write_to_file("secret_b.bin", key_b, CRYPTO_BYTES)) {
    //     return -1;
    // }

    // // Decapsulation
    // crypto_kem_dec(key_a, ct, sk);

    // // 验证文件读取功能
    // uint8_t pk_read[CRYPTO_PUBLICKEYBYTES];
    // uint8_t sk_read[CRYPTO_SECRETKEYBYTES];
    // uint8_t ct_read[CRYPTO_CIPHERTEXTBYTES];
    
    // if(read_from_file("pk_1.bin", pk_read, CRYPTO_PUBLICKEYBYTES) ||
    //    read_from_file("sk_1.bin", sk_read, CRYPTO_SECRETKEYBYTES) ||
    //    read_from_file("ct_1.bin", ct_read, CRYPTO_CIPHERTEXTBYTES)) {
    //     return -1;
    // }

    // // 验证读取数据一致性
    // if(memcmp(pk, pk_read, CRYPTO_PUBLICKEYBYTES) ||
    //    memcmp(sk, sk_read, CRYPTO_SECRETKEYBYTES) ||
    //    memcmp(ct, ct_read, CRYPTO_CIPHERTEXTBYTES)) {
    //     fprintf(stderr, "File data mismatch!\n");
    //     return -1;
    // }

    // 读取文件内容
    if(read_from_file("pk_1.bin", pk, CRYPTO_PUBLICKEYBYTES) ||
    read_from_file("sk_1.bin", sk, CRYPTO_SECRETKEYBYTES) ||
    read_from_file("ct_1.bin", ct, CRYPTO_CIPHERTEXTBYTES) ||
    read_from_file("secret_b.bin", key_b, CRYPTO_BYTES))
  {
     return -1;
 }
    // printf("Public Key: ");
    // for(j=0;j<CRYPTO_PUBLICKEYBYTES;j++)
    //   printf("%02x",pk[j]);
    // printf("\n");
    // printf("Secret Key: ");
    // for(j=0;j<CRYPTO_SECRETKEYBYTES;j++)
    //   printf("%02x",sk[j]);
    // printf("\n");

    // Encapsulation
    // crypto_kem_enc(ct, key_b, pk);
    // printf("Ciphertext: ");
    // for(j=0;j<CRYPTO_CIPHERTEXTBYTES;j++)
    //   printf("%02x",ct[j]);
    // printf("\n");
    // printf("Shared Secret B: ");
    // for(j=0;j<CRYPTO_BYTES;j++)
    //   printf("%02x",key_b[j]);
    // printf("\n");

    // Decapsulation
    crypto_kem_dec(key_a, ct, sk);
    // printf("Shared Secret A: ");
    // for(j=0;j<CRYPTO_BYTES;j++)
    //   printf("%02x",key_a[j]);
    // printf("\n");
    int flag = 0;
    for(int j=0;j<CRYPTO_BYTES;j++) {
      if(key_a[j] != key_b[j]) {
        fprintf(stderr, "ERROR\n");
        flag = -1;
        return -1;
      }
    }
    if(flag == 0) {
      fprintf(stderr, "OK\n");
    }

  return 0;
}
