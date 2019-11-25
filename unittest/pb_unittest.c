#include <stdio.h>
#include <endian.h>
#include "unittest.h"
#include "test_config.h"
#include "../src/debug.h"
#include "../src/signer.h"
#include "../src/pb_pack.h"


void test_pack(const char *test_name, pb_em_wtype test_type,
               void *test_data, const char **test_hex, size_t test_data_size, const char *multi_hex) {

    size_t i;
    int pb_len, hex_len;
    uint8_t pb_buffer[256];
    char serialize_hex[256];

    pb_st_item multi[10];
    pb_st_item single = {test_type, 1};

    for (i = 0; i < test_data_size; i++) {

        multi[i].type = test_type;
        multi[i].field = i + 1;

        if (test_type == PB_WT_32) {

            multi[i].data = (void *)((uint32_t *)test_data + i);
        }
        else {

            multi[i].data = (void *)((uint64_t *)test_data + i);
        }

        single.data = multi[i].data;
        pb_len = pb_pack(&single, 1, pb_buffer, sizeof(pb_buffer));
        hex_len = signer_hex2str(pb_buffer, pb_len, serialize_hex, sizeof(serialize_hex));

        UNITTEST_ASSERT_NE(-1, pb_len);
        UNITTEST_ASSERT_NE(-1, hex_len);
        UNITTEST_ASSERT_STR_EQ(serialize_hex, test_hex[i], hex_len);
    }


    pb_len = pb_pack(multi, test_data_size, pb_buffer, sizeof(pb_buffer));
    hex_len = signer_hex2str(pb_buffer, pb_len, serialize_hex, sizeof(serialize_hex));

    UNITTEST_ASSERT_NE(-1, pb_len);
    UNITTEST_ASSERT_NE(-1, hex_len);
    UNITTEST_ASSERT_STR_EQ(serialize_hex, multi_hex, hex_len);

    UNITTEST_PASS(test_name);
}

void test_pack_varint() {

    uint64_t datas[] = {1, 300, 0x123, 0x1f2f32, 0x13232323, 0x3214325412};
    const char *serialize_str[] = {

        "0801",
        "08ac02",
        "08a302",
        "08b2de7c",
        "08a3c68c9901",
        "0892a8c9a1a106",
    };

    test_pack(__func__, PB_WT_VARINT, datas, serialize_str, UNITTEST_GET_ARRAY_SIZE(datas), "080110ac0218a30220b2de7c28a3c68c99013092a8c9a1a106");
}


void test_pack_fixed32() {

    uint32_t datas[] = {1, 300, 0x123, 0x1f2f32, 0x13232323, 0x12345678};
    const char *serialize_str[] = {

        "0d01000000",
        "0d2c010000",
        "0d23010000",
        "0d322f1f00",
        "0d23232313",
        "0d78563412",
    };

    test_pack(__func__, PB_WT_32, datas, serialize_str, UNITTEST_GET_ARRAY_SIZE(datas), "0d01000000152c0100001d2301000025322f1f002d232323133578563412");
}


void test_pack_fixed64() {

    uint64_t datas[] = {1, 300, 0x123, 0x1f2f32, 0x13232323, 0x123456789, 0x12345678ABCDEF};
    const char *serialize_str[] = {

        "090100000000000000",
        "092c01000000000000",
        "092301000000000000",
        "09322f1f0000000000",
        "092323231300000000",
        "098967452301000000",
        "09efcdab7856341200",
    };

    test_pack(__func__, PB_WT_64, datas, serialize_str, UNITTEST_GET_ARRAY_SIZE(datas), "090100000000000000112c0100000000000019230100000000000021322f1f000000000029232323130000000031896745230100000039efcdab7856341200");
}


void test_pack_bytes() {

    const char *bytes_str[] = {
        "1234567890ABCDEF",
        "00010203040506070809",
        TEST_ACCOUNT_ADDR,
        TEST_TRANSFERS_BLOCK_ACTHASH,
        TEST_TRANSFERS_BLOCK_BLKHASH,
        TEST_TRANSFERS_BLOCK_SIGNATURE,
        TEST_TRANSFERS_BLOCK_SENDER_PUBKEY,
    };

    size_t i;
    int pb_len;
    uint8_t pb_buffer[256];
    pb_st_item single = {PB_WT_LD, 1};

    for (i = 0; i < UNITTEST_GET_ARRAY_SIZE(bytes_str); i++) {

        memset(pb_buffer, 0, sizeof(pb_buffer));
        single.data = (void *)bytes_str[i];
        single.length = strlen(bytes_str[i]);
        pb_len = pb_pack(&single, 1, pb_buffer, sizeof(pb_buffer));

        UNITTEST_ASSERT_EQ(pb_len, single.length + 2);
        UNITTEST_ASSERT_EQ(pb_buffer[0], 0xa);
        UNITTEST_ASSERT_EQ(pb_buffer[1], single.length & 0xff);
        UNITTEST_ASSERT_STR_EQ(pb_buffer + 2, bytes_str[i], single.length);
    }

    UNITTEST_AUTO_PASS();
}

void test_pack_embedded() {

    int pb_len, hex_len;
    uint8_t pb_buffer[256] = {0};
    char serialize_hex[512] = {0};

    uint64_t emb_vint = 300;
    uint32_t emb_f32 = 0x87654321;
    uint64_t emb_f64 = 0x1234567890ABCDEF;
    const char *emb_str = "this is an embedded msg";

    uint64_t vint = 600;
    uint32_t f32 = 0x12345678;
    uint64_t f64 = 0xFEDCBA9087654321;
    const char *str = "this is a msg include an embedded msg";

    pb_st_item emb[] = {

        {PB_WT_VARINT, 11, (void *) &emb_vint},
        {PB_WT_32, 12, (void *) &emb_f32},
        {PB_WT_64, 13, (void *) &emb_f64},
        {PB_WT_LD, 14, (void *)emb_str, strlen(emb_str)},
    };

    pb_st_item messages[] = {

        {PB_WT_VARINT, 1, (void *) &vint},
        {PB_WT_32, 2, (void *) &f32},
        {PB_WT_64, 3, (void *) &f64},
        {PB_WT_LD, 4, (void *)str, strlen(str)},
        {PB_WT_EMB, 5, (void *)emb, sizeof(emb) / sizeof(emb[0])},
    };

    pb_len = pb_pack(messages, UNITTEST_GET_ARRAY_SIZE(messages), pb_buffer, sizeof(pb_buffer));
    hex_len = signer_hex2str(pb_buffer, pb_len, serialize_hex, sizeof(serialize_hex));
    UNITTEST_ASSERT_STR_EQ(serialize_hex, "08d8041578563412192143658790badcfe2225746869732069732061206d736720696e636c75646520616e20656d626564646564206d73672a2a58ac02652143658769efcdab907856341272177468697320697320616e20656d626564646564206d7367", hex_len);

    UNITTEST_AUTO_PASS();
}

int main(int argc, char **argv) {

    test_pack_bytes();
    test_pack_varint();
    test_pack_fixed32();
    test_pack_fixed64();
    test_pack_embedded();
    return 0;
}
