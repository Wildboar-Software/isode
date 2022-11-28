# CHANGELOG

## 8.3 - UTF8String support

This release adds minimal decoding support for `UTF8String` values. This was
implemented because, in the process of integration testing with
[Meerkat DSA](https://wildboar-software.github.io/directory/), it was discovered
that the `dish` client would reject results that included `UTF8String` values.
In addition to this, directory strings that are not `TeletexString`s will be
encoded as `UTF8String`s rather than `PrintableString`s.

Note that no actual UTF-8 decoding is actually performed at all, and the
`DirectoryString` comparison functions will NOT correctly compare all UTF-8
strings. Because of how the internal API for string values works, it would be
really difficult to implement UTF-8 encoding, comparison, sorting, etc. This
implementation just treats all `UTF8String`s like any opaque null-terminated
sequence of bytes. This is strategically lazy: the point wasn't to overhaul
ISODE to support all of the latest features of X.500 directories, but rather,
just for it to not crash / fail / reject etc. just because it encounters a
(common, if not preferred) `DirectoryString` alternative. To clarify, multi-byte
characters will not be uppercased for comparison correctly, leading to some
strange matching outcomes. Sorry.
