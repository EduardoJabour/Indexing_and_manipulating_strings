typedef struct registry{
	int key;
	int *occurrences;
	int num_occurrences;
	struct registry *next;
} Registry, *RegPointer;

#define M 97
typedef RegPointer Index[M];

#define MAX_LINE_LENGHT 16

int index_createfrom(const char *key_file, const char *text_file, Index **idx);
int index_get(const Index *idx, const char *key, int **occurrences, int *num_occurrences);
int index_put(Index *idx, const char *key);
int index_print(const Index *idx);
