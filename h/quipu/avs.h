/* avs.h - */

#ifndef _AVS_H_
#define _AVS_H_

typedef struct avseqcomp {      /* attribute may have multiple values   */
	/* respresents SET OF AttributeValue    */
	attrVal      	avseq_av;
	struct avseqcomp    *avseq_next;
} avseqcomp, *AV_Sequence;

#endif