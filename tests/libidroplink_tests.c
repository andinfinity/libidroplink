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
    int i;
    char *error_message;

    /* test cases */
    char *actual[] = {
            join_url(base, "/user/", NULL),
            join_url(base, "user", NULL),
            join_url(base, "user/", NULL),
            join_url(base, "/user", NULL),

            join_url(base, "/user/", "/thisisafakeid/", NULL),
            join_url(base, "user", "/thisisafakeid/", NULL),
            join_url(base, "user/", "/thisisafakeid/", NULL),
            join_url(base, "/user", "/thisisafakeid/", NULL),

            join_url(base, "/user/", "thisisafakeid", NULL),
            join_url(base, "user", "thisisafakeid", NULL),
            join_url(base, "user/", "thisisafakeid", NULL),
            join_url(base, "/user", "thisisafakeid", NULL),

            join_url(base, "/user/", "thisisafakeid/", NULL),
            join_url(base, "user", "thisisafakeid/", NULL),
            join_url(base, "user/", "thisisafakeid/", NULL),
            join_url(base, "/user", "thisisafakeid/", NULL),

            join_url(base, "/user/", "/thisisafakeid", NULL),
            join_url(base, "user", "/thisisafakeid", NULL),
            join_url(base, "user/", "/thisisafakeid", NULL),
            join_url(base, "/user", "/thisisafakeid", NULL),

            /* note that we won't test this more thoroughly like the case with three
             * components above as this would take *a lot* of test cases to do it
             * exhaustive */
            join_url(base, "/user", "thisisafakeid", "someaction", NULL)
    };

    const char *expected[] = {
            "http://localhost:7667/api/v1/user/",
            "http://localhost:7667/api/v1/user/",
            "http://localhost:7667/api/v1/user/",
            "http://localhost:7667/api/v1/user/",

            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",

            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",

            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",

            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",
            "http://localhost:7667/api/v1/user/thisisafakeid/",

            "http://localhost:7667/api/v1/user/thisisafakeid/someaction/"
    };

    /* should we replace this with an actual assert? */
    mu_assert(sizeof(actual) == sizeof(expected),
            "Both arrays with actual and expected strings must match in length");

    for (i = 0; i < sizeof(actual) / sizeof(char *); ++i) {
        /* compare actual and expected strings */
        asprintf(&error_message,
                "Should have concatenated the user url for test string %d correctly."
                " Result is %s, but expected %s.", i, actual[i], expected[i]);
        mu_assert(strcmp(actual[i], expected[i]) == 0, error_message);
        free(error_message);

        /* compare length of actual and expected strings */
        asprintf(&error_message, "Strings should match in length for test string %d", i);
        mu_assert(strlen(actual[i]) == strlen(expected[i]), error_message);
        free(error_message);

        free(actual[i]);
    }
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
