-- dse.py - X.500 PresentationAddress syntax

-- 
--
--
-- 
--
--
--
--

DSE DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "";
#endif
%}

BEGIN

PSAPaddr ::=
	SEQUENCE {
	    pSelector[0]
		OCTET STRING
		OPTIONAL,

	    sSelector[1]
		OCTET STRING
		OPTIONAL,

	    tSelector[2]
		OCTET STRING
		OPTIONAL,

	    nAddress[3]
		SET OF
		    OCTET STRING
	}

END
