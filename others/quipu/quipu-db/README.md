# others/quipu/quipu-db — Example EDB trees

Sample Quipu databases donated by COSINE PARADISE. The DSA will not start with
an empty database: you need an EDB that contains the DSA's own entry
(`mydsaname`).

| Tree | Use |
| --- | --- |
| `organisation/` | First DSA of an organisation |
| `national/` | First DSA of a country |
| `non-root/` | Organisation DSA that does not hold slave copies of ROOT/country EDBs |

Each subtree has its own `READ-ME`. These are starting points to copy into the
installed Quipu database directory, not live production data.
