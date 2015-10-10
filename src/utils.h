/**
*
* Copyright (c) 2015 Christian Schulze
* Distributed under the GNU GPL v2. For full terms see the file LICENSE.
*
*/

#ifndef _UTILS_H_
#define _UTILS_H_

/**
 * Joins a variable number of path components that are joined by *one* slash, independent
 * of prepended or appended slashes.
 *
 * IMPORTANT: The last argument has to be NULL, otherwise we can't know what the last
 * argument is.
 */
char *join_url(char *comp, ...);

#endif
