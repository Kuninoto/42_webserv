Unit test locations
and requests like POST and uploads to / WITHOUT setting the location /
(probably SIGSEGV due to the assumption of targetlocation reference
on the respective functions)

review !TODO comments

Check if client_max_body_size is being asserted

CGI
- Upload files
- Make the route able to accept uploaded files and configure where they should be saved
  (upload_to directive)
- Content-length
- Does CGI handle GET and DELETE?

can DELETE delete a directory?

review chunked transfer
https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/411
"Note: by specification, when sending data in a series of chunks, the Content-Length header is omitted and at the beginning of each chunk you need to add the length of the current chunk in hexadecimal format. See Transfer-Encoding for more details. "

refer to: https://www.rfc-editor.org/rfc/rfc9112, topic 7.1.3

if Content-length is not setted and neither chunked transfer, send 411 Length Required

Connection Timeout

LEXER -------------------------
Check if at least one server exists ✅
Check for empty arguments ✅
Check for multiple arguments (except allow_methods) ✅
Check for ';' ✅
Validate port number ✅
Validate client_max_body_size ✅
Check for bracket pairness ✅

After math --------------------
Improve the HTTP Response Header ✅
Improve the HTML Response Pages
Refactor
Valgrind (fix errors)

TESTS:
Siege ✅
ubuntu_tester
