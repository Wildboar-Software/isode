# others/X — X11 over ISO transport

Experimental mapping of the X protocol onto the ISO transport service (X11 R3 + ISODE 6 TSAP). `server/` is a replacement OS layer for an X server that listens on well-known T-SAPs; `client/` replaces Xlib connection code. `-DISOCONN` selects ISO transport.

See `READ-ME` and `X-STANDARDS`. This is not a modern X port.
