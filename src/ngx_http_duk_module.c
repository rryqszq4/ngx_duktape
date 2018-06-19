/*
 *  Copyright (c) 2018 Quan Zhao
 */

#include "ngx_http_duk_module.h"
#include "duktape/module/duk_ngx_module.h"

// http init
static ngx_int_t ngx_http_duk_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_duk_handler_init(ngx_http_core_main_conf_t *cmcf, ngx_http_duk_main_conf_t *dmcf);

static void *ngx_http_duk_create_main_conf(ngx_conf_t *cf);
static char *ngx_http_duk_init_main_conf(ngx_conf_t *cf, void *conf);

static void *ngx_http_duk_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_duk_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child); 

// function init
static ngx_int_t ngx_http_duk_init_worker(ngx_cycle_t *cycle);
static void ngx_http_duk_exit_worker(ngx_cycle_t *cycle);

static ngx_command_t ngx_http_duk_commands[] = {

	{ngx_string("content_by_duk"),
	 NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
	 	|NGX_CONF_TAKE1,
	 ngx_http_duk_content_inline_phase,
	 NGX_HTTP_LOC_CONF_OFFSET,
	 0,
	 ngx_http_duk_content_inline_handler
	},

	ngx_null_command
};

static ngx_http_module_t ngx_http_duk_module_ctx = {
    NULL,                          /* preconfiguration */
    ngx_http_duk_init,             /* postconfiguration */

    ngx_http_duk_create_main_conf, /* create main configuration */
    ngx_http_duk_init_main_conf,   /* init main configuration */

    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */

    ngx_http_duk_create_loc_conf,  /* create location configuration */
    ngx_http_duk_merge_loc_conf    /* merge location configuration */

};

ngx_module_t ngx_http_duk_module = {
    NGX_MODULE_V1,
    &ngx_http_duk_module_ctx,    /* module context */
    ngx_http_duk_commands,       /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    ngx_http_duk_init_worker,      /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    ngx_http_duk_exit_worker,      /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t 
ngx_http_duk_init(ngx_conf_t *cf)
{
	ngx_http_core_main_conf_t *cmcf;
    ngx_http_duk_main_conf_t *dmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    dmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_duk_module);

    ngx_duk_request = NULL;

    if (ngx_http_duk_handler_init(cmcf, dmcf) != NGX_OK){
        return NGX_ERROR;
    }

    return NGX_OK;
}

static ngx_int_t 
ngx_http_duk_handler_init(ngx_http_core_main_conf_t *cmcf, ngx_http_duk_main_conf_t *dmcf)
{
    ngx_int_t i;
    ngx_http_handler_pt *h;
    ngx_http_phases phase;
    ngx_http_phases phases[] = {
        NGX_HTTP_POST_READ_PHASE,
        NGX_HTTP_REWRITE_PHASE,
        NGX_HTTP_ACCESS_PHASE,
        NGX_HTTP_CONTENT_PHASE,
        NGX_HTTP_LOG_PHASE
    };
    ngx_int_t phases_c;

    phases_c = sizeof(phases) / sizeof(ngx_http_phases);
    for (i = 0; i < phases_c; i++){
        phase = phases[i];
        switch (phase){
            case NGX_HTTP_POST_READ_PHASE:
                break;
            case NGX_HTTP_REWRITE_PHASE:
                break;
            case NGX_HTTP_ACCESS_PHASE:
                break;
            case NGX_HTTP_CONTENT_PHASE:
                if (dmcf->enabled_content_handler){
                    h = ngx_array_push(&cmcf->phases[phase].handlers);
                    if (h == NULL){
                        return NGX_ERROR;
                    }
                    *h = ngx_http_duk_content_handler;
                }
                break;
            case NGX_HTTP_LOG_PHASE:
                break;
            default:
                break;
        }
    }

    return NGX_OK;
}

static void * 
ngx_http_duk_create_main_conf(ngx_conf_t *cf)
{
	ngx_http_duk_main_conf_t *dmcf;

	dmcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_duk_main_conf_t));
	if (dmcf == NULL) {
		return NULL;
	}

	return dmcf;
}

static char * 
ngx_http_duk_init_main_conf(ngx_conf_t *cf, void *conf)
{
	return NGX_CONF_OK;
}

static void *
ngx_http_duk_create_loc_conf(ngx_conf_t *cf)
{
	ngx_http_duk_loc_conf_t *dlcf;

	dlcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_duk_loc_conf_t));
	if (dlcf == NULL) {
		return NGX_CONF_ERROR;
	}

	dlcf->content_inline_code = NGX_CONF_UNSET_PTR;

	return dlcf;
}

static char *
ngx_http_duk_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
	//ngx_http_core_loc_conf_t *clcf;
	//clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

	ngx_http_duk_loc_conf_t *prev = parent;
	ngx_http_duk_loc_conf_t *conf = child;

	prev->content_inline_code = conf->content_inline_code;

	return NGX_CONF_OK;
}

static ngx_int_t 
ngx_http_duk_init_worker(ngx_cycle_t *cycle)
{
	return NGX_OK;
}

static void
ngx_http_duk_exit_worker(ngx_cycle_t *cycle)
{

}

char *
ngx_http_duk_content_inline_phase(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_duk_main_conf_t *dmcf;
    ngx_http_duk_loc_conf_t *dlcf;
    ngx_str_t *value;
    ngx_http_duk_code_t *code;

    if (cmd->post == NULL) {
        return NGX_CONF_ERROR;
    }

    dmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_duk_module);
    dlcf = conf;

    if (dlcf->content_handler != NULL){
        return "is duplicated";
    }

    value = cf->args->elts;

    code = ngx_http_duk_code_from_string(cf->pool, &value[1]);
    if (code == NGX_CONF_UNSET_PTR){
        return NGX_CONF_ERROR;
    }

    dlcf->content_inline_code = code;
    dlcf->content_handler = cmd->post;
    dmcf->enabled_content_handler = 1;

    return NGX_CONF_OK;
}

ngx_http_duk_code_t *
ngx_http_duk_code_from_string(ngx_pool_t *pool, ngx_str_t *code_str)
{
    ngx_http_duk_code_t *code;
    size_t len;

    code = ngx_pcalloc(pool, sizeof(*code));
    if (code == NULL){
        return NGX_CONF_UNSET_PTR;
    }

    len = ngx_strlen(code_str->data);
    code->code.string = ngx_pcalloc(pool, len + 1);
    if (code->code.string == NULL){
        return NGX_CONF_UNSET_PTR;
    }
    ngx_cpystrn((u_char *)code->code.string, code_str->data, len + 1);
    code->code_type = NGX_HTTP_DUK_CODE_TYPE_STRING;
    return code;
}

ngx_int_t
ngx_http_duk_content_handler(ngx_http_request_t *r)
{
    ngx_http_duk_loc_conf_t *dlcf;
    dlcf = ngx_http_get_module_loc_conf(r, ngx_http_duk_module);
    if (dlcf->content_handler == NULL){
        return NGX_DECLINED;
    }
    return dlcf->content_handler(r);
}

ngx_int_t 
ngx_http_duk_content_inline_handler(ngx_http_request_t *r)
{

    ngx_http_duk_loc_conf_t *dlcf = ngx_http_get_module_loc_conf(r, ngx_http_duk_module);

    ngx_int_t rc;
    ngx_http_duk_ctx_t *ctx;
    ctx = ngx_http_get_module_ctx(r, ngx_http_duk_module);

    if (ctx == NULL){
        ctx = ngx_pcalloc(r->pool, sizeof(*ctx));
        if (ctx == NULL){
            return NGX_ERROR;
        }
    }

    ngx_http_set_ctx(r, ctx, ngx_http_duk_module);

    ngx_duk_request = r;

    ctx->duk_ctx = duk_create_heap_default();
    
    duk_ngx_module_require(ctx->duk_ctx);

  	duk_eval_string(ctx->duk_ctx, dlcf->content_inline_code->code.string);

  	duk_destroy_heap(ctx->duk_ctx);

    ngx_http_duk_rputs_chain_list_t *chain;
    
    rc = NGX_OK;

    if (rc == NGX_OK || rc == NGX_HTTP_OK) {
        chain = ctx->rputs_chain;

        if (ctx->rputs_chain == NULL){
            ngx_buf_t *b;
            ngx_str_t ns;
            u_char *u_str;
            ns.data = (u_char *)" ";
            ns.len = 1;
            
            chain = ngx_pcalloc(r->pool, sizeof(ngx_http_duk_rputs_chain_list_t));
            chain->out = ngx_alloc_chain_link(r->pool);
            chain->last = &chain->out;
        
            b = ngx_calloc_buf(r->pool);
            (*chain->last)->buf = b;
            (*chain->last)->next = NULL;

            u_str = ngx_pstrdup(r->pool, &ns);
            //u_str[ns.len] = '\0';
            (*chain->last)->buf->pos = u_str;
            (*chain->last)->buf->last = u_str + ns.len;
            (*chain->last)->buf->memory = 1;
            ctx->rputs_chain = chain;

            if (r->headers_out.content_length_n == -1){
                r->headers_out.content_length_n += ns.len + 1;
            }else {
                r->headers_out.content_length_n += ns.len;
            }
        }

        //r->headers_out.content_type.len = sizeof("text/html") - 1;
        //r->headers_out.content_type.data = (u_char *)"text/html";
        if (!r->headers_out.status){
            r->headers_out.status = NGX_HTTP_OK;
        }

        if (r->method == NGX_HTTP_HEAD){
            rc = ngx_http_send_header(r);
            if (rc != NGX_OK){
                return rc;
            }
        }

        if (chain != NULL){
            (*chain->last)->buf->last_buf = 1;
        }

        rc = ngx_http_send_header(r);
        if (rc != NGX_OK){
            return rc;
        }

        ngx_http_output_filter(r, chain->out);

        //ngx_http_set_ctx(r, NULL, ngx_http_duk_module);

        return NGX_OK;
    }

    if (rc == NGX_ERROR || rc > NGX_OK) {
        return rc;
    }

    return NGX_OK;
}
