#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include "pengda.h"


int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Generating random instances...\n");
        printf("Enter the circuit board size MAX_X MAX_Y:");
        scanf("%d %d", &max_x, &max_y);
        printf("Enter the number of points NUM_PT:");
        scanf("%d", &num_pt);
        printf("Enter the number of random instances to be generated:");
        scanf("%d", &num_instance);
        writeToInstanceFile();
        return 1;
    }
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        inputfile = i + 1;
        else if (strcmp(argv[i], "-o") == 0)
        outputfile = i + 1;
    }
    if (inputfile == 0)
    {
        /* invalid command line options */
        return -1;
    }
	if ((fp = fopen(argv[inputfile], "r")) == NULL)
    {
        /* open file error */
        exit(-2);
    }
    while (fscanf(fp, "%d", &max_x) != 1)
    {
        if (ferror(fp))
        {
            /* read error */
            fclose(fp);
            exit(-3);
        }
        if (feof(fp))
        {
            /* no integer to read */
            fclose(fp);
            exit(-4);
        }
        fscanf(fp, "%*[^\n]"); /* skip the rest of line */
    }
    if (fscanf(fp, "%d", &max_y) != 1)
    {
        /* max_y not following max_x */
        fclose(fp);
        exit(-5);
    }
    while (fscanf(fp, "%d", &num_pt) != 1)
    {
        if (ferror(fp))
        {
            /* read error */
            fclose(fp);
            exit(-6);
        }
        if (feof(fp))
        {
            /* no integer to read */
            fclose(fp);
            exit(-7);
        }
        fscanf(fp, "%*[^\n]"); /* skip the rest of line */
    }
    x_coordinate = (int *)malloc(num_pt * sizeof(int));
    y_coordinate = (int *)malloc(num_pt * sizeof(int));
    for (i = 0; i < num_pt; i++) {
        while (fscanf(fp, "%d", &x_coordinate[i]) != 1)
        {
            if (ferror(fp))
            {
                /* read error */
                fclose(fp);
                exit(-8);
            }
            if (feof(fp))
            {
                /* no integer to read */
                fclose(fp);
                exit(-9);
            }
            fscanf(fp, "%*[^\n]"); /* skip the rest of line */
        }
        if (fscanf(fp, "%d", &y_coordinate[i]) != 1)
        {
            /* y_coordinate not following x_coordinate */
            fclose(fp);
            exit(-10);
        }
    }
    fclose(fp);
    if (outputfile > 0)
    {
        FILE *fp;
        if ((fp = fopen(argv[outputfile], "w")) == NULL)
        {
            /* open file error */
            exit(-2);
        }
        fprintf(fp, "##################################################\n");
        fprintf(fp, "#%s\n", argv[inputfile]);
        fprintf(fp, "#area [0, MAX_X] x [0, MAX_Y]\n");
        fprintf(fp, "%d\t%d\n", max_x, max_y);
        fprintf(fp, "#number of points NUM_PT\n");
        fprintf(fp, "%d\n", num_pt);
        fprintf(fp, "#coordinates\n");
        for (i = 0; i < num_pt; i++)
        {
            fprintf(fp, "%d\t%d\n", x_coordinate[i], y_coordinate[i]);
        }
        fprintf(fp, "#end of instance\n");
        fclose(fp);
    }
    else {
        printf("##################################################\n");
        printf("#%s\n", argv[inputfile]);
        printf("#area [0, MAX_X] x [0, MAX_Y]\n");
        printf("%d\t%d\n", max_x, max_y);
        printf("#number of points NUM_PT\n");
        printf("%d\n", num_pt);
        printf("#coordinates\n");
        for (i = 0; i < num_pt; i++)
        {
            printf("%d\t%d\n", x_coordinate[i], y_coordinate[i]);
        }
        printf("#end of instance\n");
    }
    int* tree = (int *)malloc(num_pt * 3 * sizeof(int));
    for (i = 0; i<num_pt; i++) {
        tree[i * 3] = -1;
    }
    struct point* nodes = (struct point *)malloc(num_pt*sizeof(struct point));
    for (int i = 0; i<num_pt; i++) {
        nodes[i].idx = i;
        nodes[i].indegree = 0;
        nodes[i].parent = NULL;
        nodes[i].x = x_coordinate[i];
        nodes[i].y = y_coordinate[i];
        nodes[i].overlap = -1;
        nodes[i].layout = -1;
        nodes[i].ol0 = -1;
        nodes[i].ol1 = -1;
        nodes[i].childrenLayout0 = -1;
        nodes[i].childrenLayout1 = -1;
    }
    mst(tree, x_coordinate, y_coordinate, num_pt,argv[inputfile], nodes);
    int totalWeight = 0;
    fp = fopen(argv[inputfile], "a");
    for (i = 1; i<num_pt; i++) {
        int from = tree[i * 3 + 1], to = tree[i * 3];
        int temp = abs(x_coordinate[from] - x_coordinate[to]) + abs(y_coordinate[from] - y_coordinate[to]);
        totalWeight += temp;
    }
    
    int overlap = rst(&nodes[0]);
    fprintf(fp, "# layouts of the edges of the MST by Prim's algorithm:\n");
    printLayout(&nodes[0], fp);
    fprintf(fp, "# Total weight of the MST is %d\n", totalWeight);
    printf("# Total weight of the MST is %d\n", totalWeight);
    fprintf(fp, "# Total overlap of the layout is %d\n", overlap);
    printf("# Total overlap of the layout is %d\n", overlap);
    fprintf(fp, "# reduction is %.2f\n", (float)overlap / totalWeight);
    printf("# reduction is %.2f\n", (float)overlap / totalWeight);
    fclose(fp);
    return 0;
}



int printLayout(struct point* root, FILE* fp) {
    if (root->indegree == 0) {
        return 0;
    }
    for (int i = 0; i<root->indegree; i++) {
        struct point* child = root->child[i];
        fprintf(fp, "(%d, %d) --> (%d, %d) --> (%d, %d)\n", root->x, root->y, child->sx, child->sy, child->x, child->y);
        printLayout(child, fp);
    }
    return 0;
}

int layChildren(struct point* root) {
    if (root->indegree == 0) {
        return 0;
    }
    int comb;
    if (root->layout == 0) {
        comb = root->childrenLayout0;
    }
    else {
        comb = root->childrenLayout1;
    }
    for (int j = 0; j<root->indegree; j++) {
        int layout = comb & (1 << j);
        if (layout == 0) {
            root->child[j]->layout = 0;
            root->child[j]->sx = root->x;
            root->child[j]->sy = root->child[j]->y;
        }
        else {
            root->child[j]->sy = root->y;
            root->child[j]->sx = root->child[j]->x;
            root->child[j]->layout = 1;
        }
        layChildren(root->child[j]);
    }
    return 0;
}

int parentOverlapChildren(struct point* root) {
    int parentX = root->sx - root->x;
    int parentY = root->sy - root->y;
    int maxPos = 0, minNeg = 0;
    struct point* child;
    if (parentX != 0) {
        if (parentX>0) {
            for (int i = 0; i<root->indegree; i++) {
                child = root->child[i];
                int temp = child->sx - root->x;
                if (temp >0) {
                    if (temp >= parentX) {
                        return parentX;
                    }
                    else {
                        maxPos = max(temp, maxPos);
                    }
                }
            }
            return maxPos;
        }
        else {
            for (int i = 0; i<root->indegree; i++) {
                child = root->child[i];
                int temp = child->sx - root->x;
                if (temp < 0) {
                    if (temp <= parentX) {
                        return -parentX;
                    }
                    else {
                        minNeg = min(temp, minNeg);
                    }
                }
            }
            return -minNeg;
        }
    }
    if (parentY != 0) {
        if (parentY>0) {
            for (int i = 0; i<root->indegree; i++) {
                child = root->child[i];
                int temp = child->sy - root->y;
                if (temp >0) {
                    if (temp >= parentY) {
                        return parentY;
                    }
                    else {
                        maxPos = max(temp, maxPos);
                    }
                }
            }
            return maxPos;
        }
        else {
            for (int i = 0; i<root->indegree; i++) {
                child = root->child[i];
                int temp = child->sy - root->y;
                if (temp < 0) {
                    if (temp <= parentY) {
                        return -parentY;
                    }
                    else {
                        minNeg = min(temp, minNeg);
                    }
                }
            }
            return -minNeg;
        }
    }
    return 0;
}

int ifExist(int* end, int index, int e){
    for (int i=0;i<e;i++){
        if (end[i] == index)
            return 1;
    }
    return 0;
}
int writeToFile(int count, int* start, int* end, int* weight, char* filename){
    FILE* fp = fopen(filename, "a");
    int total = 0;
    fprintf(fp,"\n");
    fprintf(fp, "# edges of the MST by Prim's algorithm:\n");
    for (int i=1;i<count;i++){
        fprintf(fp, "%d  %d  %d\n", end[i]+1, start[i]+1, weight[i]);
        total += weight[i];
    }
    fprintf(fp, "#total weight of the MST by Prim's algorithm is %d\n", total);
    fclose(fp);
    return 0;
}
int mst(int* tree, int* x, int* y, int count, char* filename,struct point* nodes) {
 
    tree[0] = 0;
    tree[2] = 0;
    int start[count], end[count], degree[count], weight[count];
    int minWeight, s, e, d, w;
    start[0] = -1;
    end[0] = 0;
    degree[0] = 0;
    
    for (int i=1;i<count;i++){
       
        minWeight = 100000;
        for (int j=1;j<count;j++){
            if (ifExist(end,j,i))
                continue;
            for (int k=0;k<i;k++){
                int vi = end[k];
                
                int dy = abs(y[vi]-y[j]);
                int dx = abs(x[vi]-x[j]);
                w = dy + dx;
             
                if (w > minWeight)
                    continue;
                
                if (w == minWeight){
                    int ody = abs(y[s] - y[e]);
                    if (dy<ody)
                        continue;
                    else if (dy==ody){
                       
                        if (max(x[j], x[vi])<=max(x[s], x[e]))
                            continue;
                    }
                }
                minWeight = w;
                s = end[k];
                e = j;
                d = degree[k] + 1;
            }
	}
        tree[i * 3] = e;
        tree[i * 3 + 1] = s;
        nodes[s].indegree += 1;
        int temp = nodes[s].indegree;
        nodes[s].child[temp - 1] = &nodes[e];
        nodes[e].parent = &nodes[s];
        end[i] = e;
        start[i] = s;
        degree[i] = d;
        weight[i] = minWeight;
    }
    writeToFile(count, start, end, weight, filename);
    return 0;
}

int rst(struct point * root) {
	
    if (root->indegree == 0) {
        if (root->layout == 0)
        root->ol0 = 0;
        else
        root->ol1 = 0;
        return 0;
    }
    int conbin = (int)(pow(2.0, root->indegree));
    int maxComb = 0;
    int maxOverlap = 0;
    for (int i = 0; i<conbin; i++) {
        int combOverlap = 0;
        for (int j = 0; j<root->indegree; j++) {
            int laid = i & (1 << j);
            if (laid == 0) {
                root->child[j]->layout = 0;
                root->child[j]->sx = root->x;
                root->child[j]->sy = root->child[j]->y;
            }
            else {
                root->child[j]->sy = root->y;
                root->child[j]->sx = root->child[j]->x;
                root->child[j]->layout = 1;
            }
            if (root->child[j]->layout == 0 && root->child[j]->ol0 != -1)
            combOverlap += root->child[j]->ol0;
            else if (root->child[j]->layout == 1 && root->child[j]->ol1 != -1)
            combOverlap += root->child[j]->ol1;
            else
            combOverlap += rst(root->child[j]);
        }
        int posOverlap = 0, negOverlap = 0, maxPos = 0, minNeg = 0;
        for (int j = 0; j<root->indegree; j++) {
            struct point* child = root->child[j];
            int temp = child->sx - root->x;
            if (temp > 0) {
                posOverlap += temp;
                maxPos = max(maxPos, temp);
            }
            else {
                negOverlap += temp;
                minNeg = min(minNeg, temp);
            }
        }
        
        combOverlap += posOverlap - maxPos;
        combOverlap += abs(negOverlap - minNeg);

        posOverlap = 0; negOverlap = 0; maxPos = 0; minNeg = 0;
        for (int j = 0; j<root->indegree; j++) {
            struct point* child = root->child[j];
            int temp = child->sy - root->y;
            if (temp > 0) {
                posOverlap += temp;
                maxPos = max(maxPos, temp);
            }
            else {
                negOverlap += temp;
                minNeg = min(minNeg, temp);
            }
        }
        combOverlap += posOverlap - maxPos;
        combOverlap += abs(negOverlap - minNeg);
        // After we find the conbination of children lay out that has the largest overlap,
        if (root->parent != NULL) {
            combOverlap += parentOverlapChildren(root);
        }
        if (combOverlap > maxOverlap) {
            maxOverlap = combOverlap;
            maxComb = i;
        }
    }
    if (root->layout == 0) {
        root->ol0 = maxOverlap;
        root->childrenLayout0 = maxComb;
    }
    else {
        root->ol1 = maxOverlap;
        root->childrenLayout1 = maxComb;
    }
    if (root->parent == NULL) {
        layChildren(root);
    }
    return maxOverlap;
}


void writeToInstanceFile() {
    srand(time(NULL));
    for (int i = 1; i <= num_instance; i++)
    {
        char filename[30];
        sprintf(filename, "instance%03d_%03d.txt", num_pt, i);
        FILE *inFile = fopen(filename, "w");
        fprintf(inFile,
                "#%s\n#area [0, MAX_X] x [0, MAX_Y]\n%d\t%d\n#number of points NUM_PT\n%d\n#coordinates\n",
                filename, max_x, max_y, num_pt);
        for (int i = 0; i < num_pt; i++)
        {
            int x = rand() % (max_x + 1);
            int y = rand() % (max_y + 1);
            fprintf(inFile, "%d\t%d\n", x, y);
        }
        fprintf(inFile, "#end of instance");
        fclose(inFile);
    }
}

