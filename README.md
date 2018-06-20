ngx_duktape
===========

[ngx_duktape](https://github.com/rryqszq4/ngx_duktape) - Embedded duktape (an embeddable Javascript engine) for nginx-module.

Requirement
-----------
- nginx-1.6.3+

Installation
------------
```sh
$ git clone https://github.com/rryqszq4/ngx_duktape.git

$ wget 'http://nginx.org/download/nginx-1.10.3.tar.gz'
$ tar -zxvf nginx-1.10.3.tar.gz
$ cd nginx-1.10.3

$ ./configure --user=www --group=www \
              --prefix=/path/to/nginx \
              --add-module=/path/to/ngx_duktape
$ make
$ make install
```

Copyright and License
---------------------
```
MIT License

Copyright (c) 2018 Quan Zhao

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

```