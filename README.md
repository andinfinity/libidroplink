# libidroplink
C implementation of our API

## Installation
```
./configure
make
make install
```

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
