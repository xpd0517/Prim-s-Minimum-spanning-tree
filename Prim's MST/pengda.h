#define max(a,b) (a>b?a:b)
#define min(a,b) (a>b?b:a)

struct point {
    int idx;
    struct point *parent;
    int distance;
    struct point *child[7];
    int indegree;
    int x, y;
    int sx, sy;
    int overlap;
    int layout;
    int ol0, ol1;
    int childrenLayout0, childrenLayout1;
};
FILE *fp;
int max_x, max_y, num_pt, num_instance;
int *x_coordinate, *y_coordinate;
int inputfile = 0, outputfile = 0;
int i;
int printLayout(struct point* root, FILE* fp);
int layChildren(struct point* root);
int parentOverlapChildren(struct point* root);
int ifExist(int* end, int index, int e);
int writeToFile(int count, int* start, int* end, int* weight, char* filename);
int mst(int* tree, int* x, int* y, int count, char* filename,struct point* nodes);
int rst(struct point * root);
void writeToInstanceFile();
