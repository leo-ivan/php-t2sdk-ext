#ifndef T2SDK_T2CONNECTION_H
#define T2SDK_T2CONNECTION_H

class T2Connection {
public:
    T2Connection(char *ini_file);
    void connect();

private:
    char *ini_file;
};

#endif /* T2SDK_T2CONNECTION_H */