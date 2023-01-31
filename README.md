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

## Usage
```
Usage: webserv <config_file>
```

## Configuration

The configuration file is written in XML.
Here is a sample configuration file:

```xml
<webserv>
    <server name="test">
        <client_max_body_size>100000000</client_max_body_size>
        <root>/var/www</root>
        <index>index.html</index>
        <error_page status="404" path="path/to/404.html"/>
        <error_page status="500" path="path/to/50x.html"/>
        <error_page status="501" path="path/to/50x.html"/>
        <error_page status="502" path="path/to/50x.html"/>
        <directory_listing enabled="false"/>
        <route path="/somepath">
            <root>/var/www/another/path</root>
            <directory_listing enabled="false"/>
            <index>not_the_default.html</index>
            <error_page status="404" path="path/to/different/404.html"/>
        </route>
        <route path="/someotherpath">
            <redirect code="301" path="path/to/redirect.html">
                http://localhost:8080/
            </redirect>
        </route>
        <route path="/dirlist">
            <directory_listing enabled="true"/>
            <root>/var/www/mydir</root>
            <directory_index>directories.html</directory_index>
        </route>
        <route path="/usecgi">
            <cgi enabled="true" path="/usr/bin/php" ext=".php"/>
            <root>/var/www/mydir</root>
            <index>index.php</index>
        </route>
        <listen host="localhost" port="8080"/>
        <listen host="0.0.0.0"/>
    </server>
</webserv>
```

### `<webserv>`
The `<webserv>` element is the root element of the configuration file.

### `<server>`
The `<server>` element is used to define a server. It has an optional `name`
attribute.

### `<client_max_body_size>`
The `<client_max_body_size>` element is used to define the maximum size of the
body of a request in bytes. It has no attributes.

### `<root>`
The `<root>` element is used to define the root directory of the server.
It has no attributes. It can be defined both in the `<server>` element and in
the `<route>` element. When defined in the `<route>` element, it overrides the
one defined in the `<server>` element.

### `<index>`
The `<index>` element is used to define the default index file of the server.
It has no attributes. It can be defined both in the `<server>` element and in
the `<route>` element. When defined in the `<route>` element, it overrides the
one defined in the `<server>` element.

### `<error_page>`
The `<error_page>` element is used to define an error page.
It has a `status` attribute that defines the status code of the error page, and
a `path` attribute that defines the path to the error page. The path is relative
to the root directory of the server or the route.
Both attributes are required.
This element can be defined multiple times, inside a `<server>` element or a
`<route>` element. The last defined error page for a given status code will be
used. If no error page is defined for a given status code, the default error
page will be used. When defined (for the same status code) in the `<route>`
element, it overrides the one defined in the `<server>` element.

### `<directory_listing>`
The `<directory_listing>` element is used to enable or disable directory
listing. It has an `enabled` attribute that can be set to `true` or `false`.
The default value is `false`, meaning that directory listing is disabled. When 
defined in the `<route>` element, it overrides the one defined in the 
`<server>` element.

### `<route>`
The `<route>` element is used to define a route. It has a `path` attribute that
defines the path of the route. The path is relative to the root path of the
server. The path must start with a `/`.
This element can be defined multiple times, inside a `<server>` element. The
last defined route for a given path will be used.

### `<redirect>`
The `<redirect>` element is used to define a redirect. It has a `code` attribute
that defines the status code of the redirect, and a `path` attribute that
defines the path to the redirect page. It contains a URL to redirect the client
to. The path is relative to the root directory of the server or the route. Only 
the `code` attribute is required. The `path` attribute is optional. This element
can only be defined inside a `<route>` element, and only once. If the `path`
attribute is not defined, the default redirect page will be used.

### `<cgi>`
The `<cgi>` element is used to enable or disable CGI. It has an `enabled`
attribute that can be set to `true` or `false`. It also has a `path` attribute
that defines the path to the CGI executable. It also has an `ext` attribute that
defines the extension of the CGI files. The default value is `.cgi`. The default
value for the `enabled` attribute is `true`, meaning that CGI is enabled if the
`<cgi>` element is defined. It can be defined both in the `<server>` and the 
`<route>` element. When defined in the `<route>` element, it overrides the one 
defined in the `<server>` element.

### `<listen>`
The `<listen>` element is used to define a listening address. It has a `host`
attribute that defines the host to listen to, and a `port` attribute that
defines the port to listen to. At least one of these attributes needs to be set.
This element can be defined multiple times, inside a `<server>` element.
If no port is defined, the default port is 80. If no host is defined, the
default host is `127.0.0.1` (localhost). At least one `<listen>` element needs
to be defined inside a `<server>` element.

When multiple servers are defined, the first one with a defined `<listen>`
element for a given host and port will respond to all requests for that host and
port.
