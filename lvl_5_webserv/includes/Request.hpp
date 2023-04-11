#ifndef REQUEST_HPP
# define REQUEST_HPP

typedef enum methods {
    GET,
    POST,
    DELETE,
}           e_methods;

class Request {
    public:
        e_methods getMethod(void) const { return this->method; };

    private:
        e_methods method;
};

#endif // REQUEST_HPP