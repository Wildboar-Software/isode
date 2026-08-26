# others/cl — Connectionless ISODE (does not compile)

Archive of ISODECL from the HULA project: ROS over a UNIT-DATA stack with UDP as
the network provider. Based on ISODE 4.0 and
**explicitly marked as not compiling** with this tree.

Layer replacements vs the connection-oriented stack:

| Directory | Replaces |
| --- | --- |
| `tusap/` | `tsap/` |
| `susap/` | `ssap/` |
| `pusap2/` | `psap2/` |
| `acusap/` | `acsap/` |
| `rousap/` | `rosap/` |
| `hulatest/` | `imisc/`-style test services |
| `h/` | Extra headers |

See `READ-ME`. Left as a place marker; it was a candidate to drop from later
releases.
