-- P3 Defined Types (stub)

-- 
--
--
-- 
--
--
--
--




P3 DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "";
#endif
%}

BEGIN

PRINTER	print

DeliverEnvelope ::=
	SET {
	    [0]
		IMPLICIT P1.ContentType,

	    originator
		P1.ORName,

	    original[1]
		IMPLICIT P1.EncodedInformationTypes,

		P1.Priority DEFAULT normal,

	    [2]
		IMPLICIT DeliveryFlags,

	    otherRecipients[3]
		IMPLICIT SEQUENCE OF P1.ORName
		OPTIONAL,

	    thisRecipient[4]
		IMPLICIT P1.ORName,

	    intendedRecipient[5]
		IMPLICIT P1.ORName
		OPTIONAL,

	    converted[6]
		IMPLICIT P1.EncodedInformationTypes,

	    submission[7]
		IMPLICIT P1.Time
	}

DeliveryFlags ::=
	BITSTRING { conversionProhibited(2) }

END
