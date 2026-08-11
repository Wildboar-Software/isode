-- EAN Defined Types

-- 
--
--
-- 
--
--
--
--




EAN DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "";
#endif
%}

BEGIN

PRINTER	print

-- P1: EAN stores the Content of the message after the MPDU, not inside it

MPDU ::=
	CHOICE {
	    [0]
		IMPLICIT UserMPDU,

		ServiceMPDU
	}

ServiceMPDU ::=
	CHOICE {
	    [1]
		IMPLICIT P1.DeliveryReportMPDU,

	    [2]
		IMPLICIT P1.ProbeMPDU
	}

UserMPDU ::=
	SEQUENCE { P1.UMPDUEnvelope }


-- P2: EAN considers the Body OPTIONAL

UAPDU ::=
	CHOICE {
	    [0]
		IMPLICIT IM-UAPDU,

	    [1]
		IMPLICIT P2.SR-UAPDU
	}

IM-UAPDU ::=
	SEQUENCE {
	    heading
		P2.Heading,

	    body
		P2.Body
		OPTIONAL
	}

END
