/**
*
* Copyright (c) 2015 Christian Schulze
* Distributed under the GNU GPL v2. For full terms see the file LICENSE.
*
*/

#ifndef _LIBIDROPLINK_H_
#define _LIBIDROPLINK_H_

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
    int version;
    char* description;
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
 * --- Auth
 */

/**
 * Fetches the id for the given email and then retrieves an access token.
 *
 * @param email the users email
 * @param passwd the users password
 *
 * @return access token
 */
char *get_id_for_email(char *api_endpoint, char *email, char *password, struct error *err);

/**
 * Retrieves an authorization token for the given credentials.
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param email the users email
 * @param passwd the users password
 * @param err pointer to an error structure that gets written if something unexpected
 * happens.
 */
char *get_auth_token(char *api_endpoint, char *email, char *passwd, struct error *err);

/**
 * Retrieves an authorization token for the given credentials.
 *
 * @param api_endpoint the URL to the API endpoint (full, for example `http://a.b/api/v1`).
 * @param id the users internal id that is mandatory for the API url
 * @param email the users email
 * @param passwd the users password
 * @param err pointer to an error structure that gets written if something unexpected
 * happens.
 */
char *get_auth_token_for_id(char *api_endpoint, char *id, char *email, char *passwd, struct error *err);

#endif
