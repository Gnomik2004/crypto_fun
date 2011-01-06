/*
 * cryptoinit.c
 * Kyle Isom <coder@kyleisom.net>
 * 2011-01-06
 *
 * cryptographic initialisation functions, including key generation 
 */

#include "cryptoinit.h"

int crypto_init( ) {
    #ifdef DBEUG
    printf("[+] initialising gcrypt...\n");
    #endif

    if (! gcry_check_version(GCRYPT_MIN_VERSION)) {
        #ifdef DEBUG
        fprintf("[!] version mismatch. the minimum version is %s\n", 
                GCRYPT_MIN_VERSION);
        #endif
        return EXIT_FAILURE;
    }

    /* suspend secure memory warnings - if secure memory is to be used,
     * it will be re-enabled later. */
    gcry_control(GCRYCTL_SUSPEND_SECMEM_WARNING);

    #ifdef SECURE_MEM

    /* place the random pool in secure memory */
    gcry_control(GCRYCTL_USE_SECURE_RNDPOOL);

    /* allocate secure memory */
    gcry_control(GCRYCTL_INIT_SECMEM, SECURE_MEM, 0);

    /* resume secure memory warnings */
    gcry_control(GCRYCTL_RESUME_SECMEM_WARNING);
    #endif

    /* signal initialization complete  - library ready for use */
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

    #ifdef DEBUG
    printf("[+] finished initialisation...\n");
    #endif

    return EXIT_SUCCESS;
}

int crypto_shutdown( struct crypto_t **keyring ) {
    #ifdef DEBUG
    printf("[+] shutting down crypto system...\n");
    #endif

    struct crypto_t *key = keyring;

    while (NULL != key) {
        /* free the key */
        gcry_free( key->key );
        key->key = NULL;

        free(key);
        key = NULL;

        key++;
    }

    /* disable secmem - has no effect if it's not being used */
    gcry_control(GCRYCTL_TERM_SECMEM);
    gcry_control(GCRYCTL_DISABLE_SECMEM);

    #ifdef DEBUG
    printf("[+] crypto system shutdown!\n");
    #endif

    return EXIT_SUCCESS;
}

int crypto_genkey( struct crypto_t *keyinfo ) {
    #ifdef DEBUG
    printf("[+] generating key...\n");
    #endif

    if (! NULL == keyinfo->key) {
        gcry_free(keyinfo->key);
        keyinfo->key = NULL;
    }

    #ifdef SECURE_MEM
    keyinfo->key = (char *) gcry_random_bytes_secure(
                                keyfile->keysize * sizeof(char),
                                RANDOM_STRENGTH);
    #else
    keyinfo->key = (char *) gcry_random_bytes(
                                keyfile->keysize * sizeof(char),
                                RANDOM_STRENGTH);
    #endif

    if (NULL == keyinfo->key) {
        #ifdef DEBUG
        fprintf(stderr, "[!] error generating key!\n");
        #endif

        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

