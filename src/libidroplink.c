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

char *get_id_for_email(char *api_endpoint, char *email, char *password, struct error *err)
{
    CURL *curl;
    char *url,
         *opts;
    CURLcode res;

    curl = curl_easy_init();

    if (curl != NULL) {
        struct curl_string s;
        init_curl_string(&s);

        opts = malloc((strlen(email) + strlen(password) + 16 + 1) * sizeof(char));
        sprintf(opts, "email=%s&password=%s", email, password);

        url = join_url(api_endpoint, "/users", email, "/idformail", NULL);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, opts);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl_result_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);

        if (s.p != NULL) {
            cJSON *root = cJSON_Parse(s.p);
            return cJSON_GetObjectItem(root, "_id")->valuestring;
        }
    }

    err->description = "Unable to prepare CURL";
    err->version = error_version;

    return NULL;
}

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
    char *url,
         *opts;
    CURLcode res;

    curl = curl_easy_init();

    if (curl != NULL) {
        struct curl_string s;
        init_curl_string(&s);

        opts = malloc((strlen(email) + strlen(passwd) + 16 + 1) * sizeof(char));
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
            return cJSON_GetObjectItem(root, "token")->valuestring;
        }
    }

    err->description = "Unable to prepare CURL";
    err->version = error_version;
    return NULL;
}

