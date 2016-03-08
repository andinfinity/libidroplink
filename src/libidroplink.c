#include <stdlib.h>
#include <string.h>

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

size_t _write_curl_result_string(void *p, size_t size, size_t nmemb, void *s_)
{
        struct curl_string *s = s_;

        size_t new_len = s->len + (size * nmemb);

        s->p = realloc(s->p, new_len + 1);

        if (s->p == NULL) {
                fprintf(stderr, "realloc() failed\n");
                exit(EXIT_FAILURE);
        }

        memcpy(s->p + s->len, p, size * nmemb);
        s->p[new_len] = '\0';
        s->len = new_len;

        return size * nmemb;
}

/**
 * Interface Implementations
 */
int check_api_vs(char *base, struct error *err)
{
        CURL *curl;

        int out;

        long http_code = 0;

        out = 0;

        if (base == NULL)
                return out;

        curl = curl_easy_init();

        if (curl != NULL) {
                char *user_agent_header;

                CURLcode res;

                struct curl_slist *header_chunk = NULL;

                struct curl_string s;

                init_curl_string(&s);

                /* User agent header */
                user_agent_header = malloc(sizeof(char *) * (26 + 1));
                if (user_agent_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(user_agent_header, "User-Agent: libidroplink/%d",
                        IDL_VERSION);

                header_chunk =
                    curl_slist_append(header_chunk, user_agent_header);

                curl_easy_setopt(curl, CURLOPT_URL, base);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 _write_curl_result_string);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

                res = curl_easy_perform(curl);
                curl_slist_free_all(header_chunk);
                free(user_agent_header);

                if (s.p != NULL && strlen(s.p) > 0) {
                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                          &http_code);

                        if (http_code == 200) {
                                out = 1;
                        } else {
                                out = 0;

                                err->description =
                                    "Unexpected answer from remote.";
                                err->version = error_version;
                                err->http_code = http_code;
                        }

                        free(s.p);
                } else {
                        err->description = "Unexpected answer from remote.";
                        err->version = error_version;
                        err->http_code = http_code;
                }

                curl_easy_cleanup(curl);
        } else {
                err->description = "Unable to prepare CURL";
                err->version = error_version;
                err->http_code = http_code;
        }

        return out;
}

/**
 * Authentication
 */
char *get_auth_token(char *api_endpoint, char *email, char *passwd,
                     struct error *err)
{
        char *id, *token;

        if (api_endpoint == NULL || email == NULL || passwd == NULL)
                return NULL;

        id = get_id_for_email(api_endpoint, email, passwd, err);

        if (id == NULL) {
                /* assuming the error was set before. */
                return NULL;
        }

        /* note: no error handling needed. if token is NULL, we return it as that */
        token = get_auth_token_for_id(api_endpoint, id, email, passwd, err);

        return token;
}

char *get_auth_token_for_id(char *api_endpoint, char *user_id, char *email,
                            char *passwd, struct error *err)
{
        CURL *curl;

        char *out;

        long http_code = 0;

        out = NULL;

        if (api_endpoint == NULL || user_id == NULL || email == NULL
            || passwd == NULL)
                return out;

        curl = curl_easy_init();

        if (curl != NULL) {
                char *post_fields, *user_agent_header, *url;

                CURLcode res;

                struct curl_slist *header_chunk = NULL;

                struct curl_string s;

                init_curl_string(&s);

                post_fields =
                    malloc((strlen(email) + strlen(passwd) + 16 +
                            1) * sizeof(char));
                if (post_fields == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(post_fields, "email=%s&password=%s", email, passwd);

                /* User agent header. */
                user_agent_header = malloc(sizeof(char *) * (26 + 1));
                if (user_agent_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(user_agent_header, "User-Agent: libidroplink/%d",
                        IDL_VERSION);

                header_chunk =
                    curl_slist_append(header_chunk, user_agent_header);

                url =
                    join_url(api_endpoint, "/users", user_id, "/authenticate",
                             NULL);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_POST, url);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 _write_curl_result_string);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

                res = curl_easy_perform(curl);
                curl_slist_free_all(header_chunk);
                free(post_fields);
                free(url);
                free(user_agent_header);

                if (s.p != NULL && strlen(s.p) > 0) {
                        cJSON *root = cJSON_Parse(s.p);

                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                          &http_code);

                        if (http_code == 201
                            && (cJSON_GetObjectItem(root, "token") != NULL)) {
                                out =
                                    strdup(cJSON_GetObjectItem
                                           (root, "token")->valuestring);
                        } else {
                                if (cJSON_GetObjectItem(root, "message") !=
                                    NULL) {
                                        err->description =
                                            strdup(cJSON_GetObjectItem
                                                   (root,
                                                    "message")->valuestring);
                                        err->version = error_version;
                                        err->http_code = http_code;
                                } else {
                                        err->description =
                                            "Unexpected response from remote.";
                                        err->version = error_version;
                                        err->http_code = http_code;
                                }
                        }

                        cJSON_Delete(root);
                        free(s.p);
                }

                curl_easy_cleanup(curl);
        } else {
                err->description = "Unable to prepare CURL";
                err->version = error_version;
                err->http_code = http_code;
        }

        return out;
}

int deauthenticate(char *api_endpoint, char *user_id, char *token,
                   struct error *err)
{
        CURL *curl;

        long http_code = 0;

        if (api_endpoint == NULL || user_id == NULL || token == NULL)
                return 0;

        curl = curl_easy_init();

        if (curl != NULL) {
                char *auth_header, *url, *user_agent_header;

                CURLcode res;

                struct curl_string s;

                struct curl_slist *header_chunk = NULL;

                init_curl_string(&s);

                /* Authorization header */
                auth_header = malloc((strlen(token) + 16 + 1) * sizeof(char));
                if (auth_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(auth_header, "Authorization: %s", token);

                header_chunk = curl_slist_append(header_chunk, auth_header);

                /* User agent header */
                user_agent_header = malloc(sizeof(char *) * (26 + 1));
                if (user_agent_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(user_agent_header, "User-Agent: libidroplink/%d",
                        IDL_VERSION);

                header_chunk =
                    curl_slist_append(header_chunk, user_agent_header);

                url =
                    join_url(api_endpoint, "/users", user_id, "/deauthenticate",
                             NULL);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_POST, url);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 _write_curl_result_string);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

                res = curl_easy_perform(curl);
                curl_slist_free_all(header_chunk);
                free(auth_header);
                free(url);
                free(user_agent_header);

                if (s.p != NULL && strlen(s.p) > 0) {
                        cJSON *root = cJSON_Parse(s.p);

                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                          &http_code);

                        if (http_code != 200) {
                                if (cJSON_GetObjectItem(root, "message") !=
                                    NULL) {
                                        err->description =
                                            strdup(cJSON_GetObjectItem
                                                   (root,
                                                    "message")->valuestring);
                                        err->version = error_version;
                                        err->http_code = http_code;
                                } else {
                                        err->description =
                                            "Unexpected answer from remote";
                                        err->version = error_version;
                                        err->http_code = http_code;
                                }
                        }

                        cJSON_Delete(root);
                        free(s.p);
                }

                curl_easy_cleanup(curl);
        } else {
                err->description = "Unable to prepare CURL";
                err->version = error_version;
                err->http_code = 0;
        }

        return http_code == 200;
}

/**
 * User
 */
char *get_id_for_email(char *api_endpoint, char *email, char *password,
                       struct error *err)
{
        CURL *curl;

        char *out;

        long http_code = 0;

        out = NULL;

        if (api_endpoint == NULL || email == NULL || password == NULL)
                return out;

        curl = curl_easy_init();

        if (curl != NULL) {
                char *post_fields, *url, *user_agent_header;

                CURLcode res;

                struct curl_string s;

                struct curl_slist *header_chunk = NULL;

                init_curl_string(&s);

                /* User agent header */
                user_agent_header = malloc(sizeof(char *) * (26 + 1));
                if (user_agent_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(user_agent_header, "User-Agent: libidroplink/%d",
                        IDL_VERSION);

                header_chunk =
                    curl_slist_append(header_chunk, user_agent_header);

                /* POST fields */
                post_fields =
                    malloc((strlen(email) + strlen(password) + 16 +
                            1) * sizeof(char));
                if (post_fields == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(post_fields, "email=%s&password=%s", email, password);

                url =
                    join_url(api_endpoint, "/users", email, "/idformail", NULL);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_POST, url);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 _write_curl_result_string);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

                res = curl_easy_perform(curl);
                curl_slist_free_all(header_chunk);
                free(post_fields);
                free(url);
                free(user_agent_header);

                if (s.p != NULL && strlen(s.p) > 0) {
                        cJSON *root = cJSON_Parse(s.p);

                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                          &http_code);

                        if (http_code == 200
                            && cJSON_GetObjectItem(root, "_id") != NULL) {
                                out =
                                    strdup(cJSON_GetObjectItem
                                           (root, "_id")->valuestring);
                        } else {
                                if (cJSON_GetObjectItem(root, "message") !=
                                    NULL) {
                                        err->description =
                                            strdup(cJSON_GetObjectItem
                                                   (root,
                                                    "message")->valuestring);
                                        err->version = error_version;
                                        err->http_code = http_code;
                                }
                        }

                        free(s.p);
                        cJSON_Delete(root);
                }

                curl_easy_cleanup(curl);
        } else {
                err->description = "Unable to prepare CURL";
                err->version = error_version;
                err->http_code = http_code;
        }

        return out;
}

char *create_user(char *api_endpoint, char *email, char *passwd,
                  struct error *err)
{
        CURL *curl;

        char *out;

        long http_code = 0;

        out = NULL;

        if (api_endpoint == NULL || email == NULL || passwd == NULL)
                return NULL;

        curl = curl_easy_init();

        if (curl != NULL) {
                char *post_fields, *url, *user_agent_header;

                CURLcode res;

                struct curl_string s;

                struct curl_slist *header_chunk = NULL;

                init_curl_string(&s);

                /* User agent header */
                user_agent_header = malloc(sizeof(char *) * (26 + 1));
                if (user_agent_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(user_agent_header, "User-Agent: libidroplink/%d",
                        IDL_VERSION);

                header_chunk =
                    curl_slist_append(header_chunk, user_agent_header);

                /* POST fields */
                post_fields =
                    malloc((strlen(email) +
                            strlen(passwd + 16 + 1)) * sizeof(char));
                if (post_fields == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(post_fields, "email=%s&password=%s", email, passwd);

                url = join_url(api_endpoint, "/users", NULL);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 _write_curl_result_string);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

                res = curl_easy_perform(curl);
                curl_slist_free_all(header_chunk);
                free(post_fields);
                free(url);
                free(user_agent_header);

                if (s.p != NULL && strlen(s.p) > 0) {
                        cJSON *root = cJSON_Parse(s.p);

                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                          &http_code);

                        if (http_code == 201
                            && cJSON_GetObjectItem(root, "_id") != NULL) {
                                out =
                                    strdup(cJSON_GetObjectItem
                                           (root, "_id")->valuestring);
                        } else {
                                if (cJSON_GetObjectItem(root, "message") !=
                                    NULL) {
                                        err->description =
                                            strdup(cJSON_GetObjectItem
                                                   (root,
                                                    "message")->valuestring);
                                        err->version = error_version;
                                        err->http_code = http_code;
                                } else {
                                        err->description =
                                            "Unexpected answer from remote";
                                        err->version = error_version;
                                        err->http_code = http_code;
                                }
                        }

                        free(s.p);
                        cJSON_Delete(root);
                }

                curl_easy_cleanup(curl);
        } else {
                err->description = "Unable to prepare CURL";
                err->version = error_version;
                err->http_code = http_code;
        }

        return out;
}

struct user *get_user(char *api_endpoint, char *user_id, char *token,
                      struct error *err)
{
        CURL *curl;

        long http_code = 0;

        struct user *usr;

        if (api_endpoint == NULL || user_id == NULL || token == NULL)
                return NULL;

        usr = malloc(sizeof(struct user));

        if (usr == NULL) {
                fprintf(stderr, "malloc() failed\n");
                exit(EXIT_FAILURE);
        }

        curl = curl_easy_init();

        if (curl != NULL) {
                char *auth_header, *url, *user_agent_header;

                CURLcode res;

                struct curl_string s;

                struct curl_slist *header_chunk = NULL;

                init_curl_string(&s);

                /* Authorization header */
                auth_header = malloc((strlen(token) + 16 + 1) * sizeof(char));
                if (auth_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(auth_header, "Authorization: %s", token);

                header_chunk = curl_slist_append(header_chunk, auth_header);

                /* User agent header */
                user_agent_header = malloc(sizeof(char *) * (26 + 1));
                if (user_agent_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(user_agent_header, "User-Agent: libidroplink/%d",
                        IDL_VERSION);

                header_chunk =
                    curl_slist_append(header_chunk, user_agent_header);

                url = join_url(api_endpoint, "/users", user_id, NULL);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 _write_curl_result_string);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

                res = curl_easy_perform(curl);
                curl_slist_free_all(header_chunk);
                free(auth_header);
                free(url);
                free(user_agent_header);

                if (s.p != NULL && strlen(s.p) > 0) {
                        cJSON *root = cJSON_Parse(s.p);

                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                          &http_code);

                        if (http_code == 200
                            && cJSON_GetObjectItem(root, "_id") != NULL) {
                                usr->_id =
                                    strdup(cJSON_GetObjectItem
                                           (root, "_id")->valuestring);
                        } else {
                                usr->_id = NULL;
                        }

                        if (http_code == 200
                            && cJSON_GetObjectItem(root, "email") != NULL) {
                                usr->email =
                                    strdup(cJSON_GetObjectItem
                                           (root, "email")->valuestring);
                        } else {
                                usr->email = NULL;
                        }

                        if (http_code == 200
                            && cJSON_GetObjectItem(root,
                                                   "creation_date") != NULL) {
                                usr->creation_date =
                                    strdup(cJSON_GetObjectItem
                                           (root,
                                            "creation_date")->valuestring);
                        } else {
                                usr->creation_date = NULL;
                        }

                        if (http_code == 200 && usr->_id == NULL
                            && usr->email == NULL
                            && usr->creation_date == NULL) {
                                if (cJSON_GetObjectItem(root, "message") !=
                                    NULL) {
                                        err->description =
                                            strdup(cJSON_GetObjectItem
                                                   (root,
                                                    "message")->valuestring);
                                        err->version = error_version;
                                        err->http_code = http_code;
                                } else {
                                        err->description =
                                            "Unexpected answer from remote";
                                        err->version = error_version;
                                        err->http_code = http_code;
                                }

                                usr = NULL;
                        }

                        cJSON_Delete(root);
                        free(s.p);
                }

                curl_easy_cleanup(curl);
        } else {
                err->description = "Unable to prepare CURL";
                err->version = error_version;
                err->http_code = http_code;
        }

        return usr;
}

int delete_user(char *api_endpoint, char *user_id, char *token,
                struct error *err)
{
        CURL *curl;

        long http_code = 0;

        if (api_endpoint == NULL || user_id == NULL || token == NULL)
                return 0;

        curl = curl_easy_init();

        if (curl != NULL) {
                char *auth_header, *url, *user_agent_header;

                CURLcode res;

                struct curl_string s;

                struct curl_slist *header_chunk = NULL;

                init_curl_string(&s);

                /* Authorization header */
                auth_header = malloc((strlen(token) + 16 + 1) * sizeof(char));
                if (auth_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(auth_header, "Authorization: %s", token);

                header_chunk = curl_slist_append(header_chunk, auth_header);
                /* User agent header */
                user_agent_header = malloc(sizeof(char *) * (26 + 1));
                if (user_agent_header == NULL) {
                        fprintf(stderr, "malloc() failed\n");
                        exit(EXIT_FAILURE);
                }
                sprintf(user_agent_header, "User-Agent: libidroplink/%d",
                        IDL_VERSION);

                header_chunk =
                    curl_slist_append(header_chunk, user_agent_header);

                url = join_url(api_endpoint, "/users", user_id, NULL);
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                                 _write_curl_result_string);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

                res = curl_easy_perform(curl);
                curl_slist_free_all(header_chunk);
                free(auth_header);
                free(url);
                free(user_agent_header);

                if (s.p != NULL && strlen(s.p) > 0) {
                        cJSON *root = cJSON_Parse(s.p);

                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                          &http_code);

                        if (http_code != 200) {
                                if (cJSON_GetObjectItem(root, "message") !=
                                    NULL) {
                                        err->description =
                                            strdup(cJSON_GetObjectItem
                                                   (root,
                                                    "message")->valuestring);
                                        err->version = error_version;
                                        err->http_code = http_code;
                                } else {
                                        err->description =
                                            "Unexpected answer from remote";
                                        err->version = error_version;
                                        err->http_code = http_code;
                                }
                        }

                        cJSON_Delete(root);
                        free(s.p);
                }

                curl_easy_cleanup(curl);
        } else {
                err->description = "Unable to prepare CURL";
                err->version = error_version;
                err->http_code = 0;
        }

        return http_code == 200;
}
