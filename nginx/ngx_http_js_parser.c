

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "ngx_http_js_module.h"

static u_char *
ngx_http_js_run_read_block(ngx_conf_t *cf);

static char *
ngx_http_js_parse_block(ngx_conf_t *cf, ngx_command_t *cmd);


char * 
ngx_http_js_run_block(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf)
{
    char        *rv;
    ngx_conf_t   confsave;

    confsave = *cf;
    cf->handler = ngx_http_js_run;
    cf->handler_conf = conf;

    rv = ngx_http_js_parse_block(cf, cmd);

    *cf = confsave;
    return rv;
}


static char *
ngx_http_js_parse_block(ngx_conf_t *cf, ngx_command_t *cmd)
{
    u_char           *p;
    char             *rv;
    ngx_str_t        *dst;
    ngx_array_t      *saved;

    saved = cf->args;

    cf->args = ngx_array_create(cf->temp_pool, 4, sizeof(ngx_str_t));
    if (cf->args == NULL) {
        return NGX_CONF_ERROR;
    }

    p = ngx_http_js_run_read_block(cf);
    if (p==0) return NGX_CONF_ERROR;


    dst = ngx_array_push(saved);
    if (dst == NULL) {
        return NGX_CONF_ERROR;
    }
    dst->len = ngx_strlen(p);
    dst->data = p;

    cf->args = saved;

    rv = (*cf->handler)(cf, cmd, cf->handler_conf);

    if (rv == NGX_CONF_OK) {
        return NGX_CONF_OK;
    }

    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, rv);
    return NGX_CONF_ERROR;

}


static u_char *
ngx_http_js_run_read_block(ngx_conf_t *cf)
{
    u_char       ch=0, pch=0;
    size_t       len = 0;
    u_char       *p, *start;
    ngx_buf_t    *b;

    cf->args->nelts = 0;
    b = cf->conf_file->buffer;
    start = b->pos;
 
    for ( ;; ) {

        if (b->pos >= b->last) {
            ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                          "unexpected end of file, "
                          "js_run_block is expecting to be closed with \"};\"");
            return 0;
        }

        ch = *b->pos++;
        if (ch == LF) {
            cf->conf_file->line++;
        }

        if (pch=='}' && ch==';') {
            break;
        }
        pch = ch;

        len++;
    }

    p = ngx_palloc(cf->pool, len);
    if (p == NULL) {
        return NULL;
    }

    (void) ngx_copy(p, start, len);
    p[len-1]=0;
    return p;

}