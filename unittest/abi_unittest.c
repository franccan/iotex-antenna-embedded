#include <stdlib.h>
#include <string.h>
#include "unittest.h"
#include "../src/abi_pack.h"
#include "../src/endian_conv.h"
#include "../src/signer.h"

char input[160];
char expected[160];
const char *in[] = {
    "01020304050601020304050601020304050601020304050601020304050607",
    "0102030405060102030405060102030405060102030405060102030405060708",
    "010203040506010203040506010203040506010203040506010203040506070801",
    NULL
};

const char *out[] = {
    "7fd283460000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000001f0102030405060102030405060102030405060102030405060102030405060700",
    "7fd28346000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000200102030405060102030405060102030405060102030405060102030405060708",
    "7fd283460000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000002101020304050601020304050601020304050601020304050601020304050607080100000000000000000000000000000000000000000000000000000000000000",
};

void test_abi_pack() {
    int i;
    for (i = 0; in[i]; i++) {
        int data_len = signer_str2hex(in[i], (uint8_t *)input, strlen(in[i])/2);
        UNITTEST_ASSERT_EQ(data_len, strlen(in[i])/2);

        uint64_t out_size;
        uint8_t *data = abi_pack_publish((const uint8_t *)input, data_len, &out_size);
        UNITTEST_ASSERT_NE(data, NULL);

        data_len = signer_str2hex(out[i], (uint8_t *)expected, strlen(out[i])/2);
        UNITTEST_ASSERT_EQ(out_size, data_len);
        
        UNITTEST_ASSERT_STR_EQ(data, expected, data_len);

        free(data);
        data = NULL;
    }
    UNITTEST_AUTO_PASS();
}

int main(int argc, char **argv) {
    test_abi_pack();
    return 0;
}
