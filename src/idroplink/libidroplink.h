/**
*
* Copyright (c) 2015 Christian Schulze
* Distributed under the GNU GPL v2. For full terms see the file LICENSE.
*
*/

#ifndef _LIBIDROPLINK_H_
#define _LIBIDROPLINK_H_

#define API_COMPAT_VERSION "v1"

#define IDL_VERSION_MAJOR 0
#define IDL_VERSION_MINOR 1
#define IDL_VERSION ((IDL_VERSION_MAJOR << 16) | IDL_VERSION_MINOR)

/**
 * Simple error that gets returned when something unexpected happens.
 *
 * `version` holds static information about the version of this very structure because
 * it may be modified in the future.
 * `description` further describes the issue if possible.
 */
struct error {
        /* internal error versioning to avoid errors on changing attributes over time */
        int version;
        /* description as provided by the remote API or alternatively a rough problem description */
        char *description;
        /* http status code if available, otherwise defaults to 0 if failed */
        long http_code;
};

struct user {
        char *_id;
        char *email;
        char *creation_date;
};

/**
 * --- META
 */

/**
 * Returns version of library.
 *
 * @returns version
 */
unsigned int get_library_version(void);

/**
 * Tests whether or not binary and header file are compatible
 *
 * @returns if test was successful or not
 */
int idl_is_compatible(void);

/**
 * -- API
 */
int check_api_vs(char *base, struct error *err);

/**
 * --- Auth
 */

/**
 * Retrieves an authorization token for the given credentials.
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param email the users email
 * @param passwd the users password
 * @param err an allocated error struct that is being filled up if the function returns
 * NULL due to a failure
 *
 * @return auth token
 */
char *get_auth_token(char *api_endpoint, char *email, char *passwd,
                     struct error *err);

/**
 * Retrieves an authorization token for the given credentials.
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param id the users internal id that is mandatory for the API url
 * @param email the users email
 * @param passwd the users password
 * @param err an allocated error struct that is being filled up if the function returns
 * NULL due to a failure
 *
 * @return auth token
 */
char *get_auth_token_for_id(char *api_endpoint, char *user_id, char *email,
                            char *passwd, struct error *err);

/**
 * Revokes the previously issued auth token
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param email the users email
 * @param passwd the users password
 * @param err an allocated error struct that is being filled up if the function returns
 * NULL due to a failure
 *
 * @return If successfull, 1 gets returned. If not, something unexpected happened. See
 * err for details.
 */
int deauthenticate(char *api_endpoint, char *user_id, char *token,
                   struct error *err);

/**
 * User
 */

/**
 * Fetches the id for the given email and then retrieves an access token.
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param email the users email
 * @param passwd the users password
 * @param err an allocated error struct that is being filled up if the function returns
 * NULL due to a failure
 *
 * @return user id
 */
char *get_id_for_email(char *api_endpoint, char *email, char *password,
                       struct error *err);

/**
 * Creates a user
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param email the users email
 * @param passwd the users password
 * @param err an allocated error struct that is being filled up if the function returns
 * NULL due to a failure
 *
 * @returns the users id (NULL if unsuccessful)
 */
char *create_user(char *api_endpoint, char *email, char *passwd,
                  struct error *err);

/**
 * Get information about the user
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param id the users id as returned by `get_id_for_email`.
 * @param token access token that allows the client to request the resource
 * @param err an allocated error struct that is being filled up if the function returns
 * NULL due to a failure
 *
 * @returns user information
 */
struct user *get_user(char *api_endpoint, char *user_id, char *token,
                      struct error *err);

/**
 * Deletes a user entry.
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param id the users id as returned by `get_id_for_email`.
 * @param token access token that allows the client to request the resource
 * @param err an allocated error struct that is being filled up if the function returns
 * NULL due to a failure
 *
 * @return If successfull, 1 gets returned. If not, something unexpected happened. See
 * err for details.
 */
int delete_user(char *api_endpoint, char *user_id, char *token,
                struct error *err);

#endif
