#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../src/utils.h"
#include "../src/idroplink/libidroplink.h"

struct error err;
char *token,
     *user_id,
     *base,
     *TEST_USER,
     *TEST_PASS,
     *NEW_USER_NAME;
struct user *usr;


void test_setup() {
    base = getenv("IDL_API_ENDPOINT");

    if (base == NULL)
        base = "http://localhost:7667/api/v1";

    TEST_USER = "foo@bar.de";
    NEW_USER_NAME = "bar@baz.de";
    TEST_PASS = "test";
}

void test_teardown() {
}

MU_TEST(url_joining) {
    /* test cases */
    char *in1 = join_url(base, "/user/", NULL);
    char *in2 = join_url(base, "user", NULL);
    char *in3 = join_url(base, "user/", NULL);
    char *in4 = join_url(base, "/user", NULL);

    char *in5 = join_url(base, "/user/", "/thisisafakeid/", NULL);
    char *in6 = join_url(base, "user", "/thisisafakeid/", NULL);
    char *in7 = join_url(base, "user/", "/thisisafakeid/", NULL);
    char *in8 = join_url(base, "/user", "/thisisafakeid/", NULL);

    char *in9 = join_url(base, "/user/", "thisisafakeid", NULL);
    char *in10 = join_url(base, "user", "thisisafakeid", NULL);
    char *in11 = join_url(base, "user/", "thisisafakeid", NULL);
    char *in12 = join_url(base, "/user", "thisisafakeid", NULL);

    char *in13 = join_url(base, "/user/", "thisisafakeid/", NULL);
    char *in14 = join_url(base, "user", "thisisafakeid/", NULL);
    char *in15 = join_url(base, "user/", "thisisafakeid/", NULL);
    char *in16 = join_url(base, "/user", "thisisafakeid/", NULL);

    char *in17 = join_url(base, "/user/", "/thisisafakeid", NULL);
    char *in18 = join_url(base, "user", "/thisisafakeid", NULL);
    char *in19 = join_url(base, "user/", "/thisisafakeid", NULL);
    char *in20 = join_url(base, "/user", "/thisisafakeid", NULL);

    /* note that we won't test this more thoroughly like the case with three components
     * above as this would take *a lot* of test cases to do it exhaustive */
    char *in21 = join_url(base, "/user", "thisisafakeid", "someaction", NULL);

    size_t test_1_len = strlen("http://localhost:7667/api/v1/user/");
    size_t test_2_len = strlen("http://localhost:7667/api/v1/user/thisisafakeid/");

    mu_assert(strcmp(in1, "http://localhost:7667/api/v1/user/") == 0,
            "Should have concatenated the user url for in 1 correctly");
    mu_assert(strlen(in1) == test_1_len, "Strings should match in lengh for in1");

    mu_assert(strcmp(in2, "http://localhost:7667/api/v1/user/") == 0,
            "Should have concatenated the user url for in2 correctly");
    mu_assert(strlen(in2) == test_1_len, "Strings should match in lengh for in2");

    mu_assert(strcmp(in3, "http://localhost:7667/api/v1/user/") == 0,
            "Should have concatenated the user url for in3 correctly");
    mu_assert(strlen(in3) == test_1_len, "Strings should match in lengh for in3");

    mu_assert(strcmp(in4, "http://localhost:7667/api/v1/user/") == 0,
            "Should have concatenated the user url for in4 correctly");
    mu_assert(strlen(in4) == test_1_len, "Strings should match in lengh for in4");


    mu_assert(strcmp(in5, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in5 correctly");
    mu_assert(strlen(in5) == test_2_len, "Strings should match in lengh for in5");

    mu_assert(strcmp(in6, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in6 correctly");
    mu_assert(strlen(in6) == test_2_len, "Strings should match in lengh for in6");

    mu_assert(strcmp(in7, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in7 correctly");
    mu_assert(strlen(in7) == test_2_len, "Strings should match in lengh for in7");

    mu_assert(strcmp(in8, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in8 correctly");
    mu_assert(strlen(in8) == test_2_len, "Strings should match in lengh for in8");

    mu_assert(strcmp(in9, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in9 correctly");
    mu_assert(strlen(in9) == test_2_len, "Strings should match in lengh for in9");

    mu_assert(strcmp(in10, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in10 correctly");
    mu_assert(strlen(in10) == test_2_len, "Strings should match in lengh for in10");

    mu_assert(strcmp(in11, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in11 correctly");
    mu_assert(strlen(in11) == test_2_len, "Strings should match in lengh for in11");

    mu_assert(strcmp(in12, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in12 correctly");
    mu_assert(strlen(in12) == test_2_len, "Strings should match in lengh for in12");

    mu_assert(strcmp(in13, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in13 correctly");
    mu_assert(strlen(in13) == test_2_len, "Strings should match in lengh for in13");

    mu_assert(strcmp(in14, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in14 correctly");
    mu_assert(strlen(in14) == test_2_len, "Strings should match in lengh for in14");

    mu_assert(strcmp(in15, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in15 correctly");
    mu_assert(strlen(in15) == test_2_len, "Strings should match in lengh for in15");

    mu_assert(strcmp(in16, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in16 correctly");
    mu_assert(strlen(in16) == test_2_len, "Strings should match in lengh for in16");

    mu_assert(strcmp(in17, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in17 correctly");
    mu_assert(strlen(in17) == test_2_len, "Strings should match in lengh for in17");

    mu_assert(strcmp(in18, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in18 correctly");
    mu_assert(strlen(in18) == test_2_len, "Strings should match in lengh for in18");

    mu_assert(strcmp(in19, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in19 correctly");
    mu_assert(strlen(in19) == test_2_len, "Strings should match in lengh for in19");

    mu_assert(strcmp(in20, "http://localhost:7667/api/v1/user/thisisafakeid/") == 0,
            "Should have concatenated the user url for in20 correctly");
    mu_assert(strlen(in20) == test_2_len, "Strings should match in lengh for in20");

    mu_assert(strcmp(in21, "http://localhost:7667/api/v1/user/thisisafakeid/someaction/") == 0,
            "Should have concatenated the user url for in21 correctly");
}

MU_TEST(check_api) {
    mu_assert(check_api_vs(NULL, &err) == 0, "Should have returned 0");
    mu_assert(check_api_vs(base, &err), "Incompatible remote detected.");
}

MU_TEST(add_user) {
    mu_assert(create_user(NULL, NULL, NULL, &err) == NULL, "Should have returned NULL");
    user_id = create_user(base, TEST_USER, TEST_PASS, &err);

    mu_assert(user_id != NULL, "Should have returned id");
}

MU_TEST(fetch_id_for_mail) {
    char *_id;

    mu_assert(get_id_for_email(NULL, NULL, NULL, &err) == NULL,
            "Should have returned NULL");

    _id = get_id_for_email(base, TEST_USER, TEST_PASS, &err);

    mu_assert(_id != NULL, "Should have returned id");

    /* no need to fail here because if so we already have an
     * error from the respective test
     */
    if (_id != NULL && user_id != NULL)
        mu_assert(strcmp(_id, user_id) == 0,
                "Should have returned same id as returned while creating user");

    if (_id != NULL)
        free(_id);
}

MU_TEST(get_token) {
    mu_assert(get_auth_token_for_id(NULL, NULL, NULL, NULL, &err) == NULL,
            "Should have returned NULL");

    token = get_auth_token_for_id(base, user_id, TEST_USER, TEST_PASS, &err);

    mu_assert(token != NULL, "Should have returned token");

}

MU_TEST(fetch_user) {
    mu_assert(get_user(NULL, NULL, NULL, &err) == NULL, "Should have returned NULL");

    usr = get_user(base, user_id, token, &err);

    mu_assert(usr != NULL, "Should have returned a user struct");

    if (usr != NULL && usr->_id != NULL && user_id != NULL)
        mu_assert(strcmp(usr->_id, user_id) == 0,
                "Should have the same id as requested");

    if (usr != NULL && usr->email != NULL && TEST_USER != NULL)
        mu_assert(strcmp(usr->email, TEST_USER) == 0,
                "Should have the same mail as it was created with");
}

MU_TEST(change_user) {
    // NOTHING
}

MU_TEST(log_out) {
    int succ;

    mu_assert(deauthenticate(NULL, NULL, NULL, &err) == 0, "Should have returned 0");

    succ = deauthenticate(base, user_id, token, &err);

    mu_assert(succ, "Should have logged the user out");
}

MU_TEST(remove_user) {
    int succ;

    mu_assert(delete_user(NULL, NULL, NULL, &err) == 0, "Should have returned 0");

    succ = delete_user(base, user_id, "FOOOOOOBAR", &err);
    mu_assert(succ == 0, "Should have not removed the user with an invalid token");

    token = get_auth_token_for_id(base, user_id, TEST_USER, TEST_PASS, &err);
    mu_assert(token != NULL, "Should have returned another token");

    succ = delete_user(base, user_id, token, &err);

    mu_assert(succ, "Should have removed the user");
}


MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    MU_RUN_TEST(url_joining);
    MU_RUN_TEST(check_api);

    MU_RUN_TEST(add_user);
    MU_RUN_TEST(fetch_id_for_mail);
    MU_RUN_TEST(get_token);
    MU_RUN_TEST(fetch_user);
    MU_RUN_TEST(log_out);
    MU_RUN_TEST(remove_user);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return 0;
}
