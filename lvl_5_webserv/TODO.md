FIX LOCATIONS

Change the way responses are made, each method must take its own path on response() after location resolving
Currently, for a DELETE, it is in fact deleting but taking the same path as a GET ending up on a
404 Response status


Upload files
DELETE method: Make the response status accurate -> https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/DELETE
Make the route able to accept uploaded files and configure where they should
be saved
Connection Timeout

CGI (POST and GET)

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
Better the HTTP Response Header
Better the HTML Response Pages
Refactor

TESTS:
Siege ✅
ubuntu_tester 
