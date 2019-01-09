// C implementation of a kd tree [related: AOJ-DSL_2_C Range Search (kD Tree)]
// Given datapoints {(id, x, y)}, we construct a kd tree and process range search queries.

#include <stdio.h>
#include <stdlib.h>
#define REP(i,c) for(int i = 0; i<c; i++)
#define REPsf(i,s, f) for(int i = s; i<f; i++)
#define READ(x) scanf(" %d",&x)
#define READC(x) scanf(" %c",&x)
#define READN(x) int x; scanf(" %d",&x)
#include <string.h>
#include <limits.h>
#include <math.h>

#define XAxis 0
#define YAxis 1


int id_collection[500000];//////クエリ領域に含まれるデータ点をリストアップしたもの
int leftcur = 0; ///// id_collectionの長さ



struct point{
    
    int id;
    int x;
    int y;
    
};


struct Node{
    ///// node of kd tree.
    
    struct point location;
    
    struct Node *leftChild;
    struct Node *rightChild;
    
    int depth;
    
    
    ////// leftmost = minimum value of x among the points belonging to the subtree starting from this node.
    ////// downmost = minimum value of y among the points belonging to the subtree starting from this node.
    ////// etc...
    
    int leftmost;
    int rightmost;
    int upmost;
    int downmost;
    
    
};


//////////////////// utils ////////////////////

int comparex(const void *a, const void *b){
    
    return  ((struct point *)a)->x - ((struct point *)b)->x;
    
}

int comparey(const void *a, const void *b){
    
    return  ((struct point *)a)->y - ((struct point *)b)->y;
    
}

int compare(const void *a, const void *b){
    
    return *(int *)a - *(int *)b;
    
}



int min(int a, int b){
    if (a>b) return b;
    return a;
}


int max(int a, int b){
    if (a<b) return b;
    return a;
}

//////////////////////////////////////////////////


struct Node *kdtree(struct point *pointlist, int right, int depth){   ////// [0.....right]についてkd treeを作る。
    
    if (right < 0){
        return NULL;
    }
    
    if (right  == 0){
        
        struct Node *new_node;
        
        new_node = malloc(sizeof(struct Node));
        
        new_node->location = pointlist[right];
        
        new_node->leftChild = NULL;
        new_node->rightChild = NULL;
        
        new_node->depth = depth;
        
        new_node->leftmost = new_node->location.x;
        new_node->rightmost = new_node->location.x;
        new_node->upmost = new_node->location.y;
        new_node->downmost = new_node->location.y;
        
        return new_node;
        
        //葉になっているので、子の中の最大値、最小値には自分自身を入れておく。
        
    }
    
    int axis = depth%2;    /// 軸が順に選択されるようにする。
    
    struct Node *new_node;
    
    new_node = malloc(sizeof(struct Node));
    
    if (axis == XAxis){
        qsort(pointlist,right+1, sizeof(struct point), comparex); ///要素数はright + 1;
        new_node->leftmost = pointlist[0].x;
        new_node->rightmost = pointlist[right].x; //// x軸方向にカットする場合、qsortの結果を使えばx方向について最大、最小値を取得できる。
    }else if (axis == YAxis){
        qsort(pointlist,right+1, sizeof(struct point), comparey);
        new_node->downmost = pointlist[0].y;
        new_node->upmost = pointlist[right].y;
    }
    
    int median = (right)/2;
    
    new_node->location = pointlist[median];
    new_node->depth = depth;
    
    new_node->rightChild = kdtree(pointlist+median+1, right-(median+1), depth +1);
    new_node->leftChild = kdtree(pointlist,median, depth +1);
    
    
    
    //// y軸方向の最大、最小値をノードと紐づけたい。
    //// x軸に切った場合、次の階層ではyで切るので、下のノードでは最大、最小値が確定している。
    //// なので、leftChildとrightChildを生成した時点で一つ潜って値を拾ってくる。ただし、NULLの場合のケアに注意。
    
    if (axis == XAxis){
        
        if (new_node->rightChild != NULL && new_node->leftChild != NULL){
            new_node->upmost = max(new_node->rightChild->upmost,new_node->leftChild->upmost);
            new_node->downmost = min(new_node->rightChild->downmost,new_node->leftChild->downmost);
            
        }else if (new_node->rightChild != NULL){
            new_node->upmost = new_node->rightChild->upmost;
            new_node->downmost = new_node->rightChild->downmost;
            
        }else if (new_node->leftChild != NULL){
            new_node->upmost = new_node->leftChild->upmost;
            new_node->downmost = new_node->leftChild->downmost;
        }else{////葉だった場合。
            new_node->upmost = new_node->location.y;
            new_node->downmost =  new_node->location.y;
        }
        
    }else{
        
        if (new_node->rightChild != NULL && new_node->leftChild != NULL){
            new_node->rightmost = max(new_node->rightChild->rightmost,new_node->leftChild->rightmost);
            new_node->leftmost = min(new_node->rightChild->leftmost,new_node->leftChild->leftmost);
            
        }else if (new_node->rightChild != NULL){
            new_node->rightmost = new_node->rightChild->rightmost;
            new_node->leftmost = new_node->rightChild->leftmost;
            
        }else if (new_node->leftChild != NULL){
            new_node->rightmost = new_node->leftChild->rightmost;
            new_node->leftmost = new_node->leftChild->leftmost;
        }else{
            new_node->rightmost = new_node->location.x;
            new_node->leftmost =  new_node->location.x;
        }
        
    }
    
    return new_node;
    
}



int is_contained(struct Node *region, int sx, int tx, int sy, int ty){
    ///// regionノードから始まるsubtreeの要素が、指定領域に全て収まっているか否かを返す。
    ///// 各ノードに、自分以下の子の最大、最小値をもたせているので、それを参照するだけで判定できる。
    
    

    int leftmost = region->leftmost;
    int rightmost = region->rightmost;
    int upmost = region->upmost;
    int downmost = region->downmost;
    
    
    
    if (leftmost < sx || rightmost > tx || upmost > ty || downmost < sy) return 0;
    
    return 1;
}


void ReportSubtree(struct Node *region){
    /////// regionをrootとするsubtreeに含まれる点をid_collectionに追加する。
    
    if (region->leftChild == NULL && (region->rightChild == NULL)){
        id_collection[leftcur] = (region->location).id;
        leftcur +=1;
        return;
    }
    
    if (region->leftChild != NULL) ReportSubtree(region->leftChild);
    if (region->rightChild != NULL) ReportSubtree(region->rightChild);
    
    return;
}



void SearchKDtree(struct Node *v, int sx, int tx, int sy, int ty){
    ///// 探索の枝刈りをどう行うか。
    ///// ノードx以下の最大値がsxより小さいならば、そのノードは除外できる.....などを考えればよさそう。
    
    
    if (v->rightmost < sx || v->leftmost > tx || v->downmost > ty || v->upmost < sy) return;
    
    
    if (v->leftChild == NULL && v->rightChild == NULL){
        
        if (sx <= v->location.x && sy <= v->location.y && tx >= v->location.x && ty >= v->location.y){
            id_collection[leftcur] = (v->location).id;
            leftcur +=1;
        }
        return;
    }
    
    if (v->leftChild != NULL){
        
        if (is_contained(v->leftChild, sx, tx, sy, ty)){
            ReportSubtree(v->leftChild);
        }else{
            SearchKDtree(v->leftChild,sx, tx, sy, ty);
        }
    }
    
    if (v->rightChild != NULL){
        
        if (is_contained(v->rightChild, sx, tx, sy, ty)){
            ReportSubtree(v->rightChild);
        }else{
            SearchKDtree(v->rightChild,sx, tx, sy, ty);
        }
    }
    return;
}


/*
void reportDepth(struct Node *rootNode){
    ///// 木のバランスを確認する用。DFSでデータ点のdepthを出力。
    
    if (rootNode == NULL){
        return;
    }
    
    if (rootNode->leftChild == NULL && rootNode->rightChild == NULL){
        printf("%d\n",rootNode->depth);
    }
    
    reportDepth(rootNode->leftChild);
    reportDepth(rootNode->rightChild);
    
    
}
 */


int main(){
    
    READN(n);
    
    int x, y;

    struct point datapoints[500000];
    
    REP(i,n){
        READ(x); READ(y);
        datapoints[i].x = x;
        datapoints[i].y = y;
        datapoints[i].id = i;
    }
    
    struct Node *rootNode;
    
    rootNode = kdtree(datapoints,n-1, 0);   ///// datapointsからkd treeを構成。
    
//    reportDepth(rootNode);
    
    
    
    
    int sx, tx, sy, ty;   //// クエリの領域を指定。sx <= x <= tx　かつ sy <= y <= ty に含まれるデータ点を返す。
    
    READN(q);
    
    REP(i, q){
        
        READ(sx);READ(tx);READ(sy);READ(ty);
        
        SearchKDtree(rootNode,sx,tx,sy,ty);   //// 条件を満たす点をid_collectionに格納する。
        
        qsort(id_collection,leftcur,sizeof(int),compare);
        
        REP(i,leftcur){
            printf("%d\n",id_collection[i]);
        }
        
        printf("\n");
        
        leftcur = 0;
    }
    
    return 0;
}
