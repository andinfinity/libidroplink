# libidroplink ![travis status](https://travis-ci.org/idrop-link/libidroplink.svg?branch=master)
C implementation of our API

## Requirements
We rely on `automake` and `libcurl`, which can be installed on Ubuntu with the following
command:
```
sudo apt-get install libcurl4-openssl-dev autoreconf
```

You don't have to do anything for curl if you are on OS X. (You might have to get Xcode and
the developer tools prior to building though.) For automake you could install it via `brew`:
```
brew install autoreconf
```

## Installation
```
./configure
make
make install
```

## Development
To build the build tools invoke the following command:
```
autoreconf -iv
```

To clean all generated files which should not end up in the repo invoke this command:
```
git clean -f -d -x
```

Unit tests are available for development environments:
```
./configure && make test
```

The code style we use is basically the linux kernel indent style as described
[here](https://www.kernel.org/doc/Documentation/CodingStyle). Please use GNU indent prior
to commiting by running:
```
indent src/idroplink/libidroplink.h src/libidroplink.c src/vendor/cJSON.c src/vendor/cJSON.h
```
The settings are given by `.indent.pro`.

Note: for the OS X users you have to install `GNU indent` via brew and use this version
instead (which won't be linked, so you'll probably find it in `/usr/local/bin/gindent`).
`BSD indent` does not suffice.

## API Implementation Progress
### API v1
* `API`
    - [x] Check for version compatibility (`GET /api/v1` for first version)

* `DROP`
    - [ ] Initialize drop transaction (`POST /users/:userId/drops`)
    - [ ] Upload file to initialized drop (`POST /users/:id/drops/:id`)
    - [ ] List drops of a User (`GET /users/:id/drops/`)
    - [ ] Get details for a drop of a User (`GET /users/:id/drops/:id`)
    - [ ] Delete drop of a User (`DELETE /users/:id/drops/:id`)

* `USER`
    - [x] Create User (`POST /users`)
    - [x] Lookup User (`GET /users/:id`)
    - [ ] Update User (`PUT /users/:id`)
    - [x] Delete User (`DELETE /users/:id`)
    - [x] Get User ID by email (`GET /users/:email/idformail`)
    - [x] Create Token for a User (`POST /users/:id/authenticate`)
    - [x] Log out (`POST /users/:id/deauthenticate`)
