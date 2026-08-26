# others/pingpong — Asynchronous connect test

Exercises ISODE's async connection establishment:

```text
pingpong address1 address2
```

Same address → connect to self and exit. Different addresses → run two copies
that interconnect. See `READ-ME` for the extra `TNetListen` parameter needed
above the transport layer.
