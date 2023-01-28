# webserv

## TODO

- [ ] Define project structure
- [X] Define config file format
- [X] Define config file syntax
- [X] Implement config file parser
- [X] Parse configuration file
- [ ] Create functional `Config` class.
- [ ] Use `Config` class to create `Server` class instances.
- [ ] Create `Server` class.
- [ ] Finish TODO list.

## Configuration



The configuration file is written in JSON for the time being. This is how it is structured:

* `servers`*: an array of servers
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
        * `methods`: an array of allowed methods
          * `0`: `"GET"`
          * `1`: `"POST"`
        * `error_pages`: a map of error pages. Paths are relative to the root directory
          * `404`: `"404.html"`
          * `500`: `"error/500.html"`
        * `cgi`: CGI to use
          * `path`: the path to the CGI executable
          * `ext`: an array of extensions to use the CGI for
            * `0`: `"php"`
            * `1`: `"py"`
        * `index`: index file to use (`"index.html"` by default)
        * `upload`: if present, enables upload
          * `max_size`: maximum size of the upload in bytes
          * `save_path`: path to save the uploaded files to
        * `redirect`: if present, the location will redirect to the given URL

Marked with `*` are required fields.