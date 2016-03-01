#include <stdlib.h>
#include <strings.h>

#include <curl/curl.h>
#include "vendor/cJSON.h"

#include "utils.h"
#include "idroplink/libidroplink.h"

unsigned int error_version = 1;

struct curl_string {
    char *p;
    size_t len;
};

void init_curl_string(struct curl_string *s)
{
    s->len = 0;
    s->p = malloc(s->len + 1);

    if (s->p == NULL) {
        fprintf(stderr, "No memory available.\n");
        exit(EXIT_FAILURE);
    }

    /* the string is empty at the moment */
    s->p[0] = '\0';
}

unsigned int get_library_version(void)
{
    return IDL_VERSION;
}

int idl_is_compatible(void)
{
    unsigned int major = get_library_version() >> 16;
    return major == IDL_VERSION_MAJOR;
}

size_t _write_curl_result_string( void *p, size_t size, size_t nmemb, struct curl_string *s)
{
    size_t new_len = s->len + (size * nmemb);
    s->p = realloc(s->p, new_len+1);

    if (s->p == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }

    memcpy(s->p+s->len, p, size*nmemb);
    s->p[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

/**
 * Interface Implementations
 */
/**
 * Authentication
 */
char *get_auth_token(char *api_endpoint, char *email, char *passwd, struct error *err)
{
    char *id,
         *token;

    id = get_id_for_email(api_endpoint, email, passwd, err);

    if (id == NULL) {
        /* assuming the error was set before. */
        return NULL;
    }

    /* note: no error handling needed. if token is NULL, we return it as that */
    token = get_auth_token_for_id(api_endpoint, id, email, passwd, err);

    return token;
}

char *get_auth_token_for_id(char *api_endpoint, char *id, char *email, char *passwd, struct error *err)
{
    CURL *curl;
    CURLcode res;
    char *url,
         *opts,
         *out;

    out = NULL;
    curl = curl_easy_init();

    if (curl != NULL) {
        struct curl_string s;
        init_curl_string(&s);

        opts = malloc((strlen(email) + strlen(passwd) + 16 + 1) * sizeof(char));
        if (opts == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }

        sprintf(opts, "email=%s&password=%s", email, passwd);

        url = join_url(api_endpoint, "/users", id, "/authenticate", NULL);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, opts);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl_result_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);

        if (s.p != NULL) {
            cJSON *root = cJSON_Parse(s.p);
            out = strdup(cJSON_GetObjectItem(root, "token")->valuestring);
            cJSON_Delete(root);
        }

        free(opts);
        free(s.p);
    } else {
        err->description = "Unable to prepare CURL";
        err->version = error_version;
    }

    curl_easy_cleanup(curl);

    return out;
}

int deauthenticate(char *api_endpoint, char *id, char *token, struct error *err)
{

    return 0;
}

/**
 * User
 */
char *get_id_for_email(char *api_endpoint, char *email, char *password, struct error *err)
{
    CURL *curl;
    CURLcode res;
    char *url,
         *opts,
         *out;

    out = NULL;
    curl = curl_easy_init();

    if (curl != NULL) {
        char *user_agent;
        struct curl_string s;
        struct curl_slist *chunk = NULL;
        init_curl_string(&s);

        user_agent = malloc(sizeof(char *) * (26 + 1));
        if (user_agent == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }

        sprintf(user_agent, "User-Agent: libidroplink/%d", IDL_VERSION);
        chunk = curl_slist_append(chunk, user_agent);

        opts = malloc((strlen(email) + strlen(password) + 16 + 1) * sizeof(char));
        if (opts == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }

        sprintf(opts, "email=%s&password=%s", email, password);

        url = join_url(api_endpoint, "/users", email, "/idformail", NULL);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, opts);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl_result_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);

        if (s.p != NULL) {
            cJSON *root = cJSON_Parse(s.p);

            out = strdup(cJSON_GetObjectItem(root, "_id")->valuestring);
            cJSON_Delete(root);
        }

        free(s.p);
        free(opts);
    } else {
        err->description = "Unable to prepare CURL";
        err->version = error_version;
    }

    curl_easy_cleanup(curl);

    return out;
}

struct user *get_user(char *api_endpoint, char *id, char *token, struct error *err)
{
    CURL *curl;
    CURLcode res;
    char *url,
         *opts;
    struct user *usr = malloc(sizeof(struct user));

    if (usr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }

    curl = curl_easy_init();

    if (curl != NULL) {
        char *user_agent;
        struct curl_string s;
        struct curl_slist *chunk = NULL;

        init_curl_string(&s);

        /* Build custom header list for `Authorization` and `UserAgent`. */
        opts = malloc((strlen(token) + 16 + 1) * sizeof(char));
        if (opts == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }

        sprintf(opts, "Authorization: %s", token);
        chunk = curl_slist_append(chunk, opts);

        user_agent = malloc(sizeof(char *) * (26 + 1));
        if (user_agent == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }

        sprintf(user_agent, "User-Agent: libidroplink/%d", IDL_VERSION);
        chunk = curl_slist_append(chunk, user_agent);

        url = join_url(api_endpoint, "/users", id, NULL);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl_result_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        curl_slist_free_all(chunk);

        if (s.p != NULL) {
            cJSON *root = cJSON_Parse(s.p);

            if (cJSON_GetObjectItem(root, "_id") != NULL) {
                usr->_id = cJSON_GetObjectItem(root, "_id")->valuestring;
            } else {
                usr->_id = NULL;
            }

            if (cJSON_GetObjectItem(root, "email") != NULL) {
                usr->email = strdup(cJSON_GetObjectItem(root, "email")->valuestring);
            } else {
                usr->email = NULL;
            }

            if (cJSON_GetObjectItem(root, "creation_date") != NULL) {
                usr->creation_date = strdup(cJSON_GetObjectItem(root, "creation_date")->valuestring);
            } else {
                usr->creation_date = NULL;
            }

            if (usr->_id == NULL && usr->email == NULL && usr->creation_date == NULL) {
                if (cJSON_GetObjectItem(root, "message") != NULL) {
                    err->description = strdup(cJSON_GetObjectItem(root, "message")->valuestring);
                    err->version = error_version;
                } else {
                    err->description = "Unexpected answer from remote";
                    err->version = error_version;
                }

                usr = NULL;
            }

            cJSON_Delete(root);
        }

        free(s.p);
        free(opts);
    } else {
        err->description = "Unable to prepare CURL";
        err->version = error_version;
    }

    curl_easy_cleanup(curl);

    return usr;
}


char *create_user(char *api_endpoint, char *email, char *passwd, struct error *err)
{
    CURL *curl;
    CURLcode res;
    char *url,
         *opts,
         *out;

    out = NULL;

    curl = curl_easy_init();

    if (curl != NULL) {
        char *user_agent;
        struct curl_string s;
        struct curl_slist *chunk = NULL;

        init_curl_string(&s);

        /* Build custom header list for `Authorization` and `UserAgent`. */
        opts = malloc((strlen(email) + strlen(passwd + 16 + 1)) * sizeof(char));
        if (opts == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }

        sprintf(opts, "email=%s&password=%s", email, passwd);

        user_agent = malloc(sizeof(char *) * (26 + 1));
        if (user_agent == NULL) {
            fprintf(stderr, "malloc() failed\n");
            exit(EXIT_FAILURE);
        }

        sprintf(user_agent, "User-Agent: libidroplink/%d", IDL_VERSION);
        chunk = curl_slist_append(chunk, user_agent);

        url = join_url(api_endpoint, "/users", NULL);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, opts);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl_result_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        curl_slist_free_all(chunk);

        if (s.p != NULL) {
            cJSON *root = cJSON_Parse(s.p);

            if (cJSON_GetObjectItem(root, "_id") != NULL) {
                out = strdup(cJSON_GetObjectItem(root, "_id")->valuestring);
            }

            if (out == NULL ) {
                if (cJSON_GetObjectItem(root, "message") != NULL) {
                    err->description = strdup(cJSON_GetObjectItem(root, "message")->valuestring);
                    err->version = error_version;
                } else {
                    err->description = "Unexpected answer from remote";
                    err->version = error_version;
                }
            }

            cJSON_Delete(root);
        }

        free(s.p);
        free(opts);
    } else {
        err->description = "Unable to prepare CURL";
        err->version = error_version;
    }

    curl_easy_cleanup(curl);

    return out;
}
