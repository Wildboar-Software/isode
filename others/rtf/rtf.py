-- rtf.py - RTF definitions

-- 
--
--
-- 
--
--
--
--

RTF DEFINITIONS ::=

BEGIN

Request ::=
    SEQUENCE {
	user[0]
	    IMPLICIT IA5String,

	password[1]
	    IMPLICIT IA5String,

	file[2]
	    IMPLICIT IA5String
    }

END
