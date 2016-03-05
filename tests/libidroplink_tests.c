#include <stdlib.h>
#include <string.h>

#include "minunit.h"

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
    mu_assert(usr->_id == user_id, "Should have the same id as requested");
    mu_assert(usr->email == TEST_USER, "Should have the same mail as it was created with");
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
    succ = delete_user(base, user_id, token, &err);

    mu_assert(token != NULL, "Should have returned another token");
    mu_assert(succ, "Should have removed the user");
}


MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

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
