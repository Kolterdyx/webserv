# webserv

## Configuration

The configuration file is written in JSON. This is how it is structured:

* `servers`: an array of servers
  * `0`
    * `name`: the name of the server
    * `listen`*: an array of addresses to listen on (`host:port` or `port`)
      * `0`: `"localhost:8080"`
      * `1`: `"8081"`
    * `root`*: the root directory of the server
    * `autoindex`: whether to enable autoindexing (`false` by default)
    * `error_pages`: a map of error pages. Paths are relative to the root directory
      * `404`: `"404.html"`
      * `500`: `"error/500.html"`
    * `max_body_size`: the maximum size of a request body in bytes (`1000000` by default)
    * `locations`*: an array of location configurations
      * `0`
        * `path`: the path to the location
        * `root`: the root directory of the location
        * `autoindex`: whether to enable autoindexing for this location (`false` by default)
        * `error_pages`: a map of error pages. Paths are relative to the root directory
          * `404`: `"404.html"`
          * `500`: `"error/500.html"`
        * `cgi`: CGI to use
          * `bin_path`: the path to the CGI executable
          * `cgi_ext`: an array of extensions to use the CGI for
            * `0`: `"php"`
            * `1`: `"py"`
        * `index`: index file to use (`"index.html"` by default)
        * `upload`: if present, enables upload
          * `max_size`: maximum size of the upload in bytes
          * `save_path`: path to save the uploaded files to
        * `redirect`: if present, the location will redirect to the given URL

Marked with `*` are required fields.