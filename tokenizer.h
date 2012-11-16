struct TokenizerT_ {
	char *token;
	struct TokenizerT_ *next;
};
typedef struct TokenizerT_* TokenizerT;



/*
 * TKCreate creates a new TokenizerT object.
 *     tc: a string containing the characters that can be in a token.
 *     fn: name of the file that should be read and tokenized.
 *
 * TKCreate will open the file and cache the FILE object.  If it cannot
 * open the given file, it will return an error.
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 */
TokenizerT TKCreate(char *tc, char *fn);

/*
 * TKDestroy destroys a TokenizerT object.  It closes the FILE that
 * TKCreate opened when creating the tokenizer.  It also deallocate
 * all memory allocated dynamically in TKCreate.
 */
void TKDestroy(TokenizerT tk);

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token is
 * dynamically allocated.  Caller is responsible for freeing the space
 * once it is no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns NULL.
 */

char *TKGetNextToken(TokenizerT tk);



