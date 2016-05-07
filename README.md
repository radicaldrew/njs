
Note: This repo is a fork of the official nginx/njs (nginScript) repo with support for:

   - **standalone module building** (build without having to build the whole nginx)
   - **enhanced javascript**  (new variables, eg to read the geoip status)

It has been tested on Centos, Ubuntu, Alpine Linux and OSX.   
Pull requests are accepted.

# Build Status
--------------

[![Build Status](https://travis-ci.org/ronanj/njs.svg?branch=master)](https://travis-ci.org/ronanj/njs)
<a href="https://scan.coverity.com/projects/ronanj-njs">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/8788/badge.svg"/>
</a>

# Standalone Module Build
-------------------------

You can directly build the dynamic module (`.so`) from this folder:

First configure the module:

    ./configure

This will install the SDK in the ./sdk/ folder.

Then make:

    ./make

It will build the `.so` file in the `build` folder.

Last copy the `.so` into the nginx module, and reload nginx 

    cp build/ngx_http_js_module.so /path/to/nginx/modules/
    nginx -s reload

Make sure to load the module in your nginx config:

    load_module modules/ngx_http_js_module.so;



# JavaScript objects
------------------

    $r
    |- uri
    |- method
    |- httpVersion
    |- remoteAddress
    |- headers{}
    |- args{}
    |- response
      |- status
      |- headers{}
      |- contentType
      |- contentLength
      |- sendHeader()
      |- send(data)
      |- finish()

    $v
    | - * maps to any nginx variable (eg `$v.geoip_city_country_code`)


# Nginx directives
------------------

A new directive names `js_run_block` has been added. It is similar to `js_run` except that it takes
as an argument a block instead of a string.

Example:


    location /getip
    {
        js_run_block {
            function hello(req, res) {
                res.status = 200;
                var ip = $v.remote_addr;
                if (req.args.callback) {
                    res.contentType = 'text/javascript';
                    ip = req.args.callback+"("+ip+")";
                } else {
                    res.contentType = 'application/json';
                }
                res.sendHeader();
                res.send(ip);
                res.finish();
            }
        };
    }




# Example
-------

Create nginx.conf:

    worker_processes 1;
    pid logs/nginx.pid;

    events {
        worker_connections  256;
    }

    http {
        js_set $summary "
            var a, s, h;

            s = 'JS summary\n\n';

            s += 'Method: ' + $r.method + '\n';
            s += 'HTTP version: ' + $r.httpVersion + '\n';
            s += 'Host: ' + $r.headers.host + '\n';
            s += 'Remote Address: ' + $r.remoteAddress + '\n';
            s += 'URI: ' + $r.uri + '\n';

            s += 'Headers:\n';
            for (h in $r.headers) {
                s += '  header \"' + h + '\" is \"' + $r.headers[h] + '\"\n';
            }

            s += 'Args:\n';
            for (a in $r.args) {
                s += '  arg \"' + a + '\" is \"' + $r.args[a] + '\"\n';
            }

            s;
            ";

        server {
            listen 8000;

            location / {
                js_run_block {
                    var res;
                    res = $r.response;
                    res.headers.foo = 1234;
                    res.status = 302;
                    res.contentType = 'text/plain; charset=utf-8';
                    res.contentLength = 15;
                    res.sendHeader();
                    res.send('nginx');
                    res.send('java');
                    res.send('script');
                    res.finish();
                };
            }

            location /summary {
                return 200 $summary;
            }
        }
    }

Run nginx & test the output:

$ curl 127.0.0.1:8000

nginxjavascript

    $ curl -H "Foo: 1099" '127.0.0.1:8000/summary?a=1&fooo=bar&zyx=xyz'

JS summary

    Method: GET
    HTTP version: 1.1
    Host: 127.0.0.1:8000
    Remote Address: 127.0.0.1
    URI: /summary
    Headers:
      header "Host" is "127.0.0.1:8000"
      header "User-Agent" is "curl/7.43.0"
      header "Accept" is "*/*"
      header "Foo" is "1099"
    Args:
      arg "a" is "1"
      arg "fooo" is "bar"
      arg "zyx" is "xyz"


--
NGINX, Inc., http://nginx.com
