/*
 *  Copyright (c) 2018 Quan Zhao
 */

#ifndef _NGX_HTTP_DUK_MODULE_H_
#define _NGX_HTTP_DUK_MODULE_H_

#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_config.h>
#include <nginx.h>

#include "duktape/duktape.h"

ngx_module_t ngx_http_duk_module;
ngx_http_request_t *ngx_duk_request;

typedef enum code_type_s {
    NGX_HTTP_DUK_CODE_TYPE_FILE,
    NGX_HTTP_DUK_CODE_TYPE_STRING
} code_type_t;

typedef struct ngx_http_duk_code_s {
    union code {
        char *file;
        char *string;
    } code;
    code_type_t code_type;
} ngx_http_duk_code_t;

typedef struct ngx_http_duk_rputs_chain_list_s {
    ngx_chain_t **last;
    ngx_chain_t *out;
} ngx_http_duk_rputs_chain_list_t;

typedef struct ngx_http_duk_ctx_s {
    ngx_http_duk_rputs_chain_list_t *rputs_chain;
    unsigned request_body_more : 1;

    duk_context *duk_ctx;
    
} ngx_http_duk_ctx_t;

typedef struct ngx_http_duk_main_conf_s {

	unsigned enabled_content_handler:1;

} ngx_http_duk_main_conf_t;

typedef struct ngx_http_duk_loc_conf_s {

	ngx_http_duk_code_t *content_inline_code;

	ngx_int_t (*content_handler)(ngx_http_request_t *r);

} ngx_http_duk_loc_conf_t;

char *ngx_http_duk_content_inline_phase(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
ngx_http_duk_code_t *ngx_http_duk_code_from_string(ngx_pool_t *pool, ngx_str_t *code_str);
ngx_int_t ngx_http_duk_content_handler(ngx_http_request_t *r);
ngx_int_t ngx_http_duk_content_inline_handler(ngx_http_request_t *r);

#endif