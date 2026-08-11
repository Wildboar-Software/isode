/* dua.h - */





#define default_common_args \
	{ \
		{       /* service controls */ \
			0, \
			SVC_PRIO_MED, \
			SVC_NOTIMELIMIT, \
			SVC_NOSIZELIMIT, \
			SVC_REFSCOPE_NONE \
		}, \
		NULLDN,      /* Common arg - requestor DN */ \
		{       /* op_progress */ \
			OP_PHASE_NOTDEFINED, \
			OP_PHASE_NOTDEFINED, \
		}, \
		CA_NO_ALIASDEREFERENCED, \
		(struct security_parms *) NULL, \
		(struct signature *) NULL, \
		(struct extension *) NULL, \
	}
