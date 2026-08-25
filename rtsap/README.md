# rtsap — Reliable Transfer Service

`librtsap` implements OSI Reliable Transfer (RTS / X.410-style): associations that move large APDUs with checkpointing and a two-way-alternate “turn”. Historic X.400 (1984) used this; Directory DSP can too.

Volume 1 (`doc/volume1.pdf`) documents the API. ROS (`rosap`) can sit on RTS instead of presentation.

`rt2ps*.c` maps RTS onto presentation; `rt2ss*.c` maps it onto session. `rts.py` is the ASN.1 module (PEPY input, not Python). `others/rtf` is a small file-transfer demo that exercises RTS upcalls.
