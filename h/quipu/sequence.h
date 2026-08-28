/* sequence.h - */

struct dua_sequence {
	char * ds_name;
	struct dua_seq_entry *ds_data;
	struct dua_seq_entry *ds_last;
	struct dua_sequence  *ds_next;
};

#define ds_alloc() (struct dua_sequence *) smalloc(sizeof (struct dua_sequence))
#define NULL_DS ((struct dua_sequence *)0)
DN sequence_dn (const int y);

struct dua_seq_entry {
	DN	de_name;
	struct  dua_seq_entry *de_next;
};

#define de_alloc() (struct dua_seq_entry *) smalloc(sizeof (struct dua_seq_entry))
#define NULL_DE ((struct dua_seq_entry *)0)
