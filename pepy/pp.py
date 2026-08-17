-- pp.py - generic pretty-printer
PRETTY DEFINITIONS ::=

%{
#include <stdlib.h>
%}

BEGIN

PRINTER print

Printer ::=
    ANY

END

%{

int main (int argc, char **argv, char **envp) {
    exit (PY_pp (argc, argv, envp, print_PRETTY_Printer));
    return 0;
}

%}
