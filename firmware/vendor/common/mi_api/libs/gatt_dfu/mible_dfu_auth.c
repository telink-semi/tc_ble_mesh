/* Includes ------------------------------------------------------------------*/
//#include <stdlib.h>
//#include <stddef.h>
//#include <string.h>
#include "mible_log.h"
#include "mible_dfu_auth.h"
#include "mible_dfu_flash.h"
#include "third_party/mbedtls/sha256_hkdf.h"
#include "third_party/mbedtls/sha256.h"
#include "third_party/pt/pt.h"
#include "cryptography/mi_crypto.h"
#include "cryptography/mi_crypto_backend_mbedtls.h"

#if MI_API_ENABLE
/* Private define ------------------------------------------------------------*/
#define MIBLE_DFU_TLV_NEW_VERSION                          0x01
#define MIBLE_DFU_TLV_OLD_VERSION                          0x02
#define MIBLE_DFU_TLV_SIGN_TYPE                            0x03
#define MIBLE_DFU_TLV_MODEL                                0x04
#define MIBLE_DFU_TLV_PID                                  0x05

#define MIBLE_DFU_CERT_MAX_SIZE                            512

/* Private typedef -----------------------------------------------------------*/
#if defined ( __CC_ARM )

__packed struct mible_dfu_ver {
    uint8_t major;
    uint8_t minor;
    uint8_t revision;
    uint16_t build;
};
typedef struct mible_dfu_ver mible_dfu_ver_t;

__packed struct mible_dfu_tag {
    uint32_t magic[4];
    uint16_t tag_size;
    uint16_t product_id;
    uint32_t firmware_size;
    uint16_t certificates_size;
    uint8_t flag;
    uint8_t reserved;
    uint8_t payload[1] ;
};
typedef struct mible_dfu_tag mible_dfu_tag_t;

__packed struct mible_dfu_tlv {
    uint8_t type;
    uint8_t length;
    uint8_t value[1];
};
typedef struct mible_dfu_tlv mible_dfu_tlv_t;

#elif defined   ( __GNUC__ )

struct __PACKED mible_dfu_ver {
    uint8_t major;
    uint8_t minor;
    uint8_t revision;
    uint16_t build;
};
typedef struct mible_dfu_ver mible_dfu_ver_t;

struct __PACKED mible_dfu_tag {
    uint8_t magic[16];
    uint16_t tag_size;
    uint16_t product_id;
    uint32_t firmware_size;
    uint16_t certificates_size;
    uint8_t flag;
    uint8_t reserved;
    uint8_t payload[1];
};
typedef struct mible_dfu_tag mible_dfu_tag_t;

struct __PACKED mible_dfu_tlv {
    uint8_t type;
    uint8_t length;
    uint8_t value[1];
};
typedef struct mible_dfu_tlv mible_dfu_tlv_t;

#endif

static void get_next(const uint8_t ps[], int16_t length, int8_t next[])
{
    int16_t j = 0;
    int8_t k = -1;

    next[0] = -1;
    while (j < length - 1) {
       if (k == -1 || ps[j] == ps[k]) {
           if (ps[++j] == ps[++k]) {
              next[j] = next[k];
           } else {
              next[j] = k;
           }
       } else {
           k = next[k];
       }
    }
}

static uint32_t get_tag_offset(uint32_t uncheck_size)
{
    int8_t next[16], j = 0;
    const uint8_t magic[] = {0x47, 0x26, 0x56, 0x82, 0x41, 0x54, 0x4F, 0x46,
                       0x54, 0xEF, 0x49, 0x4D, 0x00, 0x00, 0x00, 0x00};
    uint8_t buf[128], buf_left = 0;
    get_next(magic, 16, next);
    while (uncheck_size >= sizeof(buf) && j < 16) {
        if (buf_left == 0) {
            uncheck_size -= sizeof(buf);
            mible_dfu_flash_read(buf, sizeof(buf), uncheck_size);
            buf_left = sizeof(buf);
        }

        if (-1 == j || buf[buf_left - 1] == magic[j]) {
            buf_left--;
            j++;
        } else {
            j = next[j];
        }
    }

    return (16 == j) ? uncheck_size + buf_left : 0xFFFFFFFF;
}

static mible_status_t parse_tlv(uint8_t type, uint32_t offset, uint16_t max_len,
                                    void * data, uint16_t data_len)
{
    uint16_t index = 0;
    uint8_t dfu_tlv[offsetof(mible_dfu_tlv_t, value)];
    mible_dfu_tlv_t *p_tlv;
    mible_status_t status;

    if (NULL == data || 0 == data_len) {
        return MI_ERR_INVALID_PARAM;
    }

    MI_LOG_DEBUG("TLV total len is %d\n", max_len);
    p_tlv = (mible_dfu_tlv_t *)&dfu_tlv[0];
    while (index < max_len) {
        if (max_len < (index + sizeof(dfu_tlv))) {
            MI_LOG_WARNING("invalid length to get header\n");
            return MI_ERR_DATA_SIZE;
        }

        status = mible_dfu_flash_read(&dfu_tlv[0], sizeof(dfu_tlv), offset + index);
        if (MI_SUCCESS != status) {
            MI_LOG_WARNING("get TLV header fail (err %d)\n", status);
            return status;
        }

        MI_LOG_DEBUG("type is %d and len is %d\n", p_tlv->type, p_tlv->length);
        if (max_len < (index + sizeof(dfu_tlv) + p_tlv->length)) {
            MI_LOG_WARNING("invalid length to get payload\n");
            return MI_ERR_DATA_SIZE;
        }

        if (p_tlv->type == type && p_tlv->length == data_len) {
            status = mible_dfu_flash_read(data, data_len,
                                            offset + index + sizeof(dfu_tlv));
            if (MI_SUCCESS != status) {
                MI_LOG_WARNING("get TLV payload fail (err %d)\n", status);
                return status;
            }

            return MI_SUCCESS;
        }

        index += p_tlv->length + sizeof(dfu_tlv);
    }

    if (index > max_len) {
        MI_LOG_WARNING("the format of tag might be wrong\n");
    }

    return MI_ERR_NOT_FOUND;
}

void calc_hash(uint8_t sha[32], uint32_t firmware_size)
{
    mbedtls_sha256_context ctx;
    uint32_t reminding_size = firmware_size, copy_size;
    uint8_t buffer[256];

    mbedtls_sha256_init( &ctx );
    mbedtls_sha256_starts( &ctx, 0 );

    while (reminding_size > 0) {
        copy_size = MIN(reminding_size, sizeof(buffer));
        mible_dfu_flash_read(buffer, copy_size, firmware_size - reminding_size);
        mbedtls_sha256_update( &ctx, buffer, copy_size );
        reminding_size -= copy_size;
    }

    mbedtls_sha256_finish( &ctx, sha );
    mbedtls_sha256_free( &ctx );
}

int mible_dfu_auth(dfu_ctx_t * p_ctx, uint32_t product_id, uint32_t dfu_pack_size)
{
    mible_dfu_tag_t * p_dfu_tag = (mible_dfu_tag_t *)p_ctx->tag;
    int ret = 0;

    /* fetch tag */
    {
        /* find tag HEAD*/
        uint32_t tag_offset = get_tag_offset(dfu_pack_size);
        if (0xFFFFFFFF == tag_offset) {
            MI_LOG_WARNING("Not found TAG HEAD(MAGIC NUM)\n");
            return 1;
        }
        MI_LOG_DEBUG("TAG offset : 0x%X\n", tag_offset);

        /* Get tag size */
        uint16_t tag_size;
        ret = mible_dfu_flash_read(&tag_size, sizeof(tag_size), tag_offset + 16); /* 16 = magic num */
        if (MI_SUCCESS != ret) {
            MI_LOG_WARNING("fail to read tag size (err %d)\n", ret);
            return 2;
        }

        /* Check tag size (4 = CRC32) */
        if (tag_size < 32) {
            MI_LOG_WARNING("invalid tag size (mini size 32)\n");
            return 3;
        }

        ret = mible_dfu_flash_read(p_ctx->tag, MIN(tag_size, 512), tag_offset);
        if (MI_SUCCESS != ret) {
            MI_LOG_WARNING("fail to read tag (err %d)\n", ret);
            return 4;
        }

        if (product_id != p_dfu_tag->product_id) {
            MI_LOG_WARNING("DFU package isn't suit this model.\n", p_dfu_tag->product_id);
            return 5;
        }
        MI_LOG_DEBUG("Flag: %d\n", p_dfu_tag->flag);

        /* fetch TLV */
        mible_dfu_ver_t version;
        ret = parse_tlv(MIBLE_DFU_TLV_NEW_VERSION,
                        tag_offset + offsetof(mible_dfu_tag_t, payload),
                        tag_size - offsetof(mible_dfu_tag_t, payload) - 4,    /* CRC and fixed elements */
                        &version,
                        sizeof(version));
        if (MI_SUCCESS == ret) {
            MI_LOG_DEBUG("new firmware %u.%u.%u_%04u\n", version.major, version.minor,
                              version.revision, version.build);
        } else {
            MI_LOG_WARNING("Get new firmware version fail (err %d)\n", ret);
        }
    }

    /* fetch certificates */
    {
        msc_crt_t crt;
        uint8_t certs[p_dfu_tag->certificates_size];
        ret = mible_dfu_flash_read(certs, p_dfu_tag->certificates_size,
                                   p_dfu_tag->firmware_size + p_dfu_tag->tag_size);
        if (MI_SUCCESS != ret) {
            MI_LOG_WARNING("Can't read certificates (error %d)\n", ret);
            return 10;
        }

        /* get server certificate */
        char * pem_crt = strstr( (const char *) certs, PEM_HEADER );
        uint16_t cert_len = mbedtls_crt_pem2der((const unsigned char *)pem_crt, p_dfu_tag->certificates_size,
                                                p_ctx->server_der_crt, MIBLE_DFU_CERT_MAX_SIZE);

        ret = mi_crypto_crt_parse_der(p_ctx->server_der_crt, cert_len, NULL, &crt);
        if (ret < 0) {
            MI_LOG_WARNING("Can't find server certificate: %d\n", ret);
            return 11;
        } else {
            memcpy(&p_ctx->server_crt, &crt, sizeof crt);
        }

        /* get developer certificate */
        pem_crt = strstr( (const char *) pem_crt+1, PEM_HEADER );
        cert_len = mbedtls_crt_pem2der((const unsigned char *)pem_crt, p_dfu_tag->certificates_size,
                                       p_ctx->developer_der_crt, MIBLE_DFU_CERT_MAX_SIZE);
        ret = mi_crypto_crt_parse_der(p_ctx->developer_der_crt, cert_len, NULL, &crt);
        if (ret < 0) {
            MI_LOG_WARNING("Can't parse developer certificate: %d\n", ret);
            return 12;
        } else {
            memcpy(&p_ctx->developer_crt, &crt, sizeof crt);
        }
    }

    /* fetch signature */
    {
        uint16_t signature_size = dfu_pack_size - p_dfu_tag->firmware_size - p_dfu_tag->tag_size - p_dfu_tag->certificates_size;
        uint8_t signature[signature_size];
        mible_dfu_flash_read(signature, signature_size,
                             p_dfu_tag->firmware_size + p_dfu_tag->tag_size + p_dfu_tag->certificates_size);
        ret = mbedtls_read_signature(signature, signature_size, p_ctx->pack_sig, 64);
        if (ret < 0) {
            MI_LOG_WARNING("fail to get signature from DFU package.\n");
            return 13;
        }
    }

    calc_hash(p_ctx->pack_sha, p_dfu_tag->firmware_size + p_dfu_tag->tag_size);

    return 0;
}
#endif

