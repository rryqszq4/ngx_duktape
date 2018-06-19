/*
 *  Copyright (c) 2018 Quan Zhao
 */

#include "../duktape.h"
#include "../../ngx_http_duk_module.h"

static duk_ret_t 
duk_ngx_print(duk_context *ctx)
{
	ngx_buf_t *b;
	ngx_http_duk_rputs_chain_list_t *chain;
	ngx_http_duk_ctx_t *rctx;
	ngx_http_request_t *r;
	u_char *u_str;
	ngx_str_t ns;

	r = ngx_duk_request;
	rctx = ngx_http_get_module_ctx(r, ngx_http_duk_module);

	ns.data = (u_char *)duk_safe_to_lstring(ctx, -1, &(ns.len));

	if (rctx->rputs_chain == NULL){
        chain = ngx_pcalloc(r->pool, sizeof(ngx_http_duk_rputs_chain_list_t));
        chain->out = ngx_alloc_chain_link(r->pool);
        chain->last = &chain->out;
    }else {
        chain = rctx->rputs_chain;
        (*chain->last)->next = ngx_alloc_chain_link(r->pool);
        chain->last = &(*chain->last)->next;
    }

    b = ngx_calloc_buf(r->pool);
    (*chain->last)->buf = b;
    (*chain->last)->next = NULL;

    u_str = ngx_pstrdup(r->pool, &ns);
    //u_str[ns.len] = '\0';
    (*chain->last)->buf->pos = u_str;
    (*chain->last)->buf->last = u_str + ns.len;
    (*chain->last)->buf->memory = 1;

    *b->last++ = '\n';

    rctx->rputs_chain = chain;

    if (r->headers_out.content_length_n == -1){
        r->headers_out.content_length_n += ns.len + 1 + 1;
    }else {
        r->headers_out.content_length_n += ns.len + 1;
    }

	return 0;
}

static duk_ret_t 
duk_ngx_echo(duk_context *ctx)
{
	ngx_buf_t *b;
	ngx_http_duk_rputs_chain_list_t *chain;
	ngx_http_duk_ctx_t *rctx;
	ngx_http_request_t *r;
	u_char *u_str;
	ngx_str_t ns;

	r = ngx_duk_request;
	rctx = ngx_http_get_module_ctx(r, ngx_http_duk_module);

	ns.data = (u_char *)duk_safe_to_lstring(ctx, -1, &(ns.len));

	if (rctx->rputs_chain == NULL){
        chain = ngx_pcalloc(r->pool, sizeof(ngx_http_duk_rputs_chain_list_t));
        chain->out = ngx_alloc_chain_link(r->pool);
        chain->last = &chain->out;
    }else {
        chain = rctx->rputs_chain;
        (*chain->last)->next = ngx_alloc_chain_link(r->pool);
        chain->last = &(*chain->last)->next;
    }

    b = ngx_calloc_buf(r->pool);
    (*chain->last)->buf = b;
    (*chain->last)->next = NULL;

    u_str = ngx_pstrdup(r->pool, &ns);
    //u_str[ns.len] = '\0';
    (*chain->last)->buf->pos = u_str;
    (*chain->last)->buf->last = u_str + ns.len;
    (*chain->last)->buf->memory = 1;

    rctx->rputs_chain = chain;

    if (r->headers_out.content_length_n == -1){
        r->headers_out.content_length_n += ns.len + 1;
    }else {
        r->headers_out.content_length_n += ns.len;
    }

	return 0;
}

static const duk_function_list_entry ngx_module_funcs[] = {
	{ "print", duk_ngx_print, 1 },
	{ "echo", duk_ngx_echo, 1 },
	{ NULL, NULL, 0 }
};

static duk_ret_t 
duk_ngx_module_init(duk_context *ctx)
{
	duk_push_object(ctx);
	duk_put_function_list(ctx, -1, ngx_module_funcs);
	return 1;
}

void 
duk_ngx_module_require(duk_context *ctx)
{
	duk_push_c_function(ctx, duk_ngx_module_init, 0);
    duk_call(ctx, 0);
    duk_put_global_string(ctx, "ngx");

    duk_push_c_function(ctx, duk_ngx_print, 1);
    duk_put_global_string(ctx, "ngx_print");

    duk_push_c_function(ctx, duk_ngx_echo, 1);
    duk_put_global_string(ctx, "ngx_echo");
}

