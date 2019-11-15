#include <stdio.h>
#include "debug.h"
#include "config.h"
#include "unittest.h"
#include "test_config.h"
#include "../src/u128.h"
#include "../src/debug.h"
#include "../src/rule.h"
#include "../src/parse.h"
#include "../src/request.h"
#include "../src/iotex_emb.h"


void test_parse_str() {

    char str_array[5][128];
    const char *raw_string = "{"
                             "\"key1\": \"121\","
                             "\"key2\": \"0123456789\","
                             "\"key3\": \"abcdefghijk\","
                             "\"key4\": \"io1e2nqsyt7fkpzs5x7zf2uk0jj72teu5n6aku3tr\","
                             "\"key5\": \"53e729d28b0c69fc66c4317fdc6ee7af292980ce781b56b502e2ee2e0b9ca48a\""
                             "}";

    json_parse_rule rules[] = {

        {"key1", JSON_TYPE_STR, NULL, &str_array[0], 128},
        {"key2", JSON_TYPE_STR, NULL, &str_array[1], 128},
        {"key3", JSON_TYPE_STR, NULL, &str_array[2], 128},
        {"key4", JSON_TYPE_STR, NULL, &str_array[3], 128},
        {"key5", JSON_TYPE_STR, NULL, &str_array[4], 128},
        {NULL},
    };

    UNITTEST_ASSERT_EQ(0, json_parse_response(raw_string, rules));
    UNITTEST_ASSERT_STR_EQ(str_array[0], "121", strlen(str_array[0]));
    UNITTEST_ASSERT_STR_EQ(str_array[1], "0123456789", strlen(str_array[0]));
    UNITTEST_ASSERT_STR_EQ(str_array[2], "abcdefghijk", strlen(str_array[0]));
    UNITTEST_ASSERT_STR_EQ(str_array[3], "io1e2nqsyt7fkpzs5x7zf2uk0jj72teu5n6aku3tr", strlen(str_array[0]));
    UNITTEST_ASSERT_STR_EQ(str_array[4], "53e729d28b0c69fc66c4317fdc6ee7af292980ce781b56b502e2ee2e0b9ca48a", strlen(str_array[0]));
    UNITTEST_AUTO_PASS();
}


void test_parse_double() {

    double d[5];
    const char *raw_string = "{"
                             "\"key1\": 0.1,"
                             "\"key2\": 0.00001,"
                             "\"key3\": 0.12343,"
                             "\"key4\": 12343434.11312,"
                             "\"key5\": 3.141592653589793"
                             "}";

    json_parse_rule rules[] = {

        {"key1", JSON_TYPE_DOUBLE, NULL, &d[0]},
        {"key2", JSON_TYPE_DOUBLE, NULL, &d[1]},
        {"key3", JSON_TYPE_DOUBLE, NULL, &d[2]},
        {"key4", JSON_TYPE_DOUBLE, NULL, &d[3]},
        {"key5", JSON_TYPE_DOUBLE, NULL, &d[4]},
        {NULL},
    };

    UNITTEST_ASSERT_EQ(0, json_parse_response(raw_string, rules));
    UNITTEST_ASSERT_EQ(0.1, d[0]);
    UNITTEST_ASSERT_EQ(0.00001, d[1]);
    UNITTEST_ASSERT_EQ(0.12343, d[2]);
    UNITTEST_ASSERT_EQ(12343434.11312, d[3]);
    UNITTEST_ASSERT_EQ(3.141592653589793, d[4]);
    UNITTEST_AUTO_PASS();
}


void test_parse_object() {

    iotex_st_account_meta meta;
    const char *raw_string = "{"
                             "\"address\": \"io1e2nqsyt7fkpzs5x7zf2uk0jj72teu5n6aku3tr\","
                             "\"balance\": \"8703340000000000000000\","
                             "\"nonce\": \"30\","
                             "\"pendingNonce\": \"31\","
                             "\"numActions\": \"43\","
                             "}";

    json_parse_rule rules[] = {

        {"address", JSON_TYPE_STR, NULL, (void *) &meta.address, sizeof(meta.address)},
        {"balance", JSON_TYPE_NUMBER, NULL, (void *) &meta.balance},
        {"nonce", JSON_TYPE_NUMBER, NULL, (void *) &meta.nonce},
        {"pendingNonce", JSON_TYPE_NUMBER, NULL, (void *) &meta.pendingNonce},
        {"numActions", JSON_TYPE_NUMBER, NULL, (void *) &meta.numActions},
        {NULL}
    };

    UNITTEST_ASSERT_EQ(0, json_parse_response(raw_string, rules));
    UNITTEST_ASSERT_STR_EQ(meta.address, "io1e2nqsyt7fkpzs5x7zf2uk0jj72teu5n6aku3tr", strlen(meta.address));
    UNITTEST_ASSERT_EQ(1, u128_equal(meta.balance, construct_u128("8703340000000000000000")));
    UNITTEST_ASSERT_EQ(1, u128_equal(meta.nonce, construct_u128("30")));
    UNITTEST_ASSERT_EQ(1, u128_equal(meta.pendingNonce, construct_u128("31")));
    UNITTEST_ASSERT_EQ(1, u128_equal(meta.numActions, construct_u128("43")));

    UNITTEST_AUTO_PASS();
}


void test_parse_number() {

    uint128_t n[5];
    const char *raw_string = "{"
                             "\"key1\": 0,"
                             "\"key2\": 1,"
                             "\"key3\": 2000,"
                             "\"key4\": 3141592653589793,"
                             "\"key5\": 8703340000000000000000"
                             "}";

    json_parse_rule rules[] = {

        {"key1", JSON_TYPE_NUMBER, NULL, &n[0]},
        {"key2", JSON_TYPE_NUMBER, NULL, &n[1]},
        {"key3", JSON_TYPE_NUMBER, NULL, &n[2]},
        {"key4", JSON_TYPE_NUMBER, NULL, &n[3]},
        {"key5", JSON_TYPE_NUMBER, NULL, &n[4]},
        {NULL},
    };

    UNITTEST_ASSERT_EQ(0, json_parse_response(raw_string, rules));

    UNITTEST_ASSERT_EQ(1, u128_equal(0, n[0]));
    UNITTEST_ASSERT_EQ(1, u128_equal(1, n[1]));
    UNITTEST_ASSERT_EQ(1, u128_equal(2000, n[2]));
    UNITTEST_ASSERT_EQ(1, u128_equal(3141592653589793, n[3]));
    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("8703340000000000000000"), n[4]));

    UNITTEST_AUTO_PASS();
}


struct person {

    char name[32];
    uint128_t age;
};


int person_data_and_rule_bind(json_parse_rule *rule, void *data) {

    if (!rule || !data) {

        return -1;
    }

    json_parse_rule *f;
    struct person *p = (struct person *)data;

    while (rule && rule->key) {

        if ((f = find_rule_by_key(rule, "age"))) {

            f->value = (void *)&p->age;
        }
        else if ((f = find_rule_by_key(rule, "name"))) {

            f->value = (void *)&p->name;
            f->value_len = sizeof(p->name);
        }

        rule++;
    }

    return 0;

}

void test_parse_object_array() {


    uint128_t total;
    size_t actual_size;
    struct person persons[7];
    const char *raw_string = "{"
                             "\"total\": 7,"
                             "\"persons\": ["
                             "{\"name\": \"Marie\", \"age\": 18},"
                             "{\"name\": \"Johara\", \"age\": 32},"
                             "{\"name\": \"Wilmar\", \"age\": 39},"
                             "{\"name\": \"Arnold\", \"age\": 45},"
                             "{\"name\": \"Morgan\", \"age\": 21},"
                             "{\"name\": \"Rezene\", \"age\": 78},"
                             "{\"name\": \"Souza\", \"age\": 32},"
                             "],"
                             "}";


    json_parse_rule p_rules[]  = {

        {"age", JSON_TYPE_NUMBER},
        {"name", JSON_TYPE_STR},
        {NULL},
    };

    json_parse_rule rules[] = {

        {"total", JSON_TYPE_NUMBER, NULL, &total},
        {"persons", JSON_TYPE_ARRAY, p_rules, (void *)persons, sizeof(persons) / sizeof(persons[0]), JSON_TYPE_OBJECT, sizeof(struct person), person_data_and_rule_bind, &actual_size},
        {NULL},
    };

    UNITTEST_ASSERT_EQ(0, json_parse_response(raw_string, rules));

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("7"), total));
    UNITTEST_ASSERT_EQ(actual_size, 7);

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("18"), persons[0].age));
    UNITTEST_ASSERT_STR_EQ("Marie", persons[0].name, strlen(persons[0].name));

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("32"), persons[1].age));
    UNITTEST_ASSERT_STR_EQ("Johara", persons[1].name, strlen(persons[1].name));

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("39"), persons[2].age));
    UNITTEST_ASSERT_STR_EQ("Wilmar", persons[2].name, strlen(persons[2].name));

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("45"), persons[3].age));
    UNITTEST_ASSERT_STR_EQ("Arnold", persons[3].name, strlen(persons[3].name));

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("21"), persons[4].age));
    UNITTEST_ASSERT_STR_EQ("Morgan", persons[4].name, strlen(persons[4].name));

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("78"), persons[5].age));
    UNITTEST_ASSERT_STR_EQ("Rezene", persons[5].name, strlen(persons[5].name));

    UNITTEST_ASSERT_EQ(1, u128_equal(construct_u128("32"), persons[6].age));
    UNITTEST_ASSERT_STR_EQ("Souza", persons[6].name, strlen(persons[6].name));
}

int main(int argc, char **argv) {

    test_parse_str();
    test_parse_number();
    test_parse_double();
    test_parse_object();
    test_parse_object_array();
    return 0;
}
