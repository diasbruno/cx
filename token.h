#ifndef __CX_TOKEN_H__
#define __CX_TOKEN_H__ 1

struct token_t* tokens(void);
struct token_t* create_token(int type, const char* name,
                             int col, int lin);
struct token_t* create_punct_token(const char name);
struct token_t* token_next(struct token_t** t);
void log_token(struct token_t* t);
void log_tokens(void);
void free_token(struct token_t* t);

#endif // __CX_TOKEN_H__
