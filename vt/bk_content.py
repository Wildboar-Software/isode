-- VTPM: encode/decode BKQ/BKR PDU

-- 
--
--
-- 
--
--
--
--

VT  DEFINITIONS ::=
%{

#include "vtpm.h"
#include "sector5.h"
#include  <stdio.h>

#undef PEPYPARM
#define PEPYPARM int *
%}

BEGIN

SECTIONS build unbuild none

----------------------------------------------------------------------------

BKQ-pdu	 ::= CHOICE <<1>>

{
	bkqpdu[8] IMPLICIT BKQcontent [[p (PEPYPARM)parm]]
}

BKR-pdu
::= CHOICE <<1>>
{
	bkrpdu[9] IMPLICIT BKRcontent [[p (PEPYPARM)parm]]
}

ExplicitPointer 
%{
	EXPL_PTR	*arg = (EXPL_PTR *)parm;
%}
	::= SEQUENCE
{
	x [0] IMPLICIT INTEGER 
		[[i arg->xval ]] 
		OPTIONAL <<arg->xval != NULLCOORD>>,
	y [1] IMPLICIT INTEGER 
		[[i arg->yval ]] 
	    OPTIONAL <<arg->yval != NULLCOORD>>,
	z [2] IMPLICIT INTEGER 
		[[i arg->zval ]] 
	    OPTIONAL <<arg->zval != NULLCOORD>>
}

--------------------------------------------------------------------------------

BKQcontent 
%{
	BRcnt	*arg = (BRcnt *)parm;
%}
	::= SEQUENCE 
{
	token INTEGER [[i arg->BKQcont.token_val]]
	{
		initiator(0),
		acceptor (1),
		accChoice(2) 
	}
	OPTIONAL <<arg->BKQcont.token_val != NOBKTOK >>,
	ExplicitPointer [[p (PEPYPARM)&(arg->BKQcont.ExplPtr)]]
}

--------------------------------------------------------------------------------

BKRcontent 
%{
	BRcnt *arg = (BRcnt *)parm;
%}
	::= SEQUENCE 
{
	token INTEGER [[i arg->BKRcont.token_val]]
	{
		initiator(0),
		acceptor (1)
	}
	OPTIONAL <<arg->BKRcont.token_val != NOBKTOK >>,

	ExplicitPointer [[p (PEPYPARM)&(arg->BKRcont.ExplPtr)]]
}

--------------------------------------------------------------------------------
--------------------------------------------------------------------------------

END

