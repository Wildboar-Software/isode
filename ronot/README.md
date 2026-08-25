# ronot — RO-BIND and RO-UNBIND on ACSE

Routines that map Remote Operations Notation BIND / UNBIND macros onto ACSE (A-ASSOCIATE / A-RELEASE). This was pulled out of the Directory library so Quipu and other ASEs could bind without hand-rolling ACSE.

See the original design notes in `READ-ME`. BIND/UNBIND onto *RTSE* are not provided.

Typical entry points: `RoAsynBindRequest`, `RoBindInit`, `RoBindResponse`, `RoUnBindRequest`, and the retry variants. They encode bind arguments as presentation elements, call `acsap`, and return `RoBINDindication`s rather than raw `RoSAPindication`s.

`ronot.py` / `ry.py` are ASN.1 modules (PEPY inputs, not Python). The long-term intent recorded in `READ-ME` was to fold this into `rosap` or ROSY; that never happened in this tree.
