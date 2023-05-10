Unit test locations

review !TODO comments

POSTs are not working properly for /

Review CGI
- Content-length
- Review which env variables are in fact needed
- Does CGI handle GET and DELETE?
- process_form is not parsing QUERY_STRING properly

can DELETE delete a directory?

review chunked transfer
https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/411
"Note: by specification, when sending data in a series of chunks, the Content-Length header is omitted and at the beginning of each chunk you need to add the length of the current chunk in hexadecimal format. See Transfer-Encoding for more details. "

refer to: https://www.rfc-editor.org/rfc/rfc9112, topic 7.1.3

Upload files
Make the route able to accept uploaded files and configure where they should be saved
Connection Timeout

LEXER -------------------------
check if at least one server exists ✅
check for empty arguments ✅
check for multiple arguments (except allow_methods) ✅
check for ';' ✅
validate port number ✅
validate client_max_body_size ✅

check for bracket pairness ❌
(no closing bracket for blocks is working and shouldn't)

After math --------------------
Better the HTTP Response Header ✅
Better the HTML Response Pages
Refactor
Valgrind (fix errors)

TESTS:
Siege ✅
ubuntu_tester
