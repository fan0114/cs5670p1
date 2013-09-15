/* iScissor.cpp */
/* Main file for implementing project 1.  See TODO statments below
 * (see also correlation.cpp and iScissor.h for additional TODOs) */

#include <assert.h>
#include <iostream>
#include <fstream>

#include "correlation.h"
#include "iScissor.h"
#include "PriorityQueue.h"

const double linkLengths[8] = {1.0, SQRT2, 1.0, SQRT2, 1.0, SQRT2, 1.0, SQRT2};

// two inlined routines that may help;

inline Node& NODE(Node* n, int i, int j, int width) {
    return *(n + j * width + i);
}

inline unsigned char PIXEL(const unsigned char* p, int i, int j, int c, int width) {
    return *(p + 3 * (j * width + i) + c);
}

/************************ TODO 1 ***************************
 *InitNodeBuf
 *	INPUT:
 *		img:	a RGB image of size imgWidth by imgHeight;
 *		nodes:	a allocated buffer of Nodes of the same size, one node corresponds to a pixel in img;
 *  OUPUT:
 *      initializes the column, row, and linkCost fields of each node in the node buffer.
 */

void InitNodeBuf(Node* nodes, const unsigned char* img, int imgWidth, int imgHeight) {
    //printf("lolol TODO: %s:%d\n", __FILE__, __LINE__); 
    //printf("InitNodeBuf start\n");
    int x, y;
    int n;
    int imgRGBSize = imgWidth * imgHeight * 3;
    int imgSize = imgWidth*imgHeight;
    double *D_RGB = new double [imgRGBSize];
    double *Dlink = new double[8 * imgSize];
    double maxD;

    for (n = 0; n < 8; n++) {
        image_filter(D_RGB, img, NULL, imgWidth, imgHeight,
                &kernels[n][0], 3, 3, 1, 0);
        for (y = 0; y < imgHeight; y++)
            for (x = 0; x < imgWidth; x++) {
                Dlink[n * imgSize + y * imgWidth + x] =
                        sqrt((D_RGB[3 * (y * imgWidth + x) + 0] * D_RGB[3 * (y * imgWidth + x) + 0] +
                        D_RGB[3 * (y * imgWidth + x) + 1] * D_RGB[3 * (y * imgWidth + x) + 1] +
                        D_RGB[3 * (y * imgWidth + x) + 2] * D_RGB[3 * (y * imgWidth + x) + 2]) / 3);
            }
    }//count D(link)

    maxD = Dlink[0];
    for (y = 0; y < 8; y++) {
        for (x = 0; x < imgSize; x++) {
            if (Dlink[y * imgSize + x] > maxD)
                maxD = Dlink[y * imgSize + x];
        }
    }//count maxDlink

    for (y = 0; y < imgHeight; y++)
        for (x = 0; x < imgWidth; x++) {
            nodes[y * imgWidth + x].row = y;
            nodes[y * imgWidth + x].column = x;
            for (n = 0; n < 8; n++) {
                nodes[y * imgWidth + x].linkCost[n] =
                        (maxD - Dlink[n * imgSize + y * imgWidth + x]) * linkLengths[n];
            }
        }
    //printf("InitNodeBuf end\n");

}

/************************ END OF TODO 1 ***************************/

static int offsetToLinkIndex(int dx, int dy) {
    int indices[9] = {3, 2, 1, 4, -1, 0, 5, 6, 7};
    int tmp_idx = (dy + 1) * 3 + (dx + 1);
    assert(tmp_idx >= 0 && tmp_idx < 9 && tmp_idx != 4);
    return indices[tmp_idx];
}

/************************ TODO 4 ***************************
 *LiveWireDP:
 *	INPUT:
 *		seedX, seedY:	seed position in nodes
 *		nodes:			node buffer of size width by height;
 *      width, height:  dimensions of the node buffer;
 *		selection:		if selection != NULL, search path only in the subset of nodes[j*width+i] if selection[j*width+i] = 1;
 *						otherwise, search in the whole set of nodes.
 *		numExpanded:		compute the only the first numExpanded number of nodes; (for debugging)
 *	OUTPUT:
 *		computes the minimum path tree from the seed node, by assigning
 *		the prevNode field of each node to its predecessor along the minimum
 *		cost path from the seed to that node.
 */

int numNodesInPath(Node * node) {
    int count = 0;
    while (node->prevNode != NULL) {
        count++;
        node = node->prevNode;
    }
    return count;
}

void LiveWireDP(int seedX, int seedY, Node* nodes, int width, int height, const unsigned char* selection, int numExpanded) {
    //printf("TODO: %s:%d\n", __FILE__, __LINE__); 
    //FILE* log;
    //log = fopen("log.txt", "w");
    //printf("LiveWireDP start numExpanded: %d\n",numExpanded);
    if (selection != NULL) {
        printf("LiveWireDP selection mode\n");
    }
    int nodeSize = width*height;
    CTypedPtrHeap<Node> pq;
    int x, y;
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            nodes[width * y + x].state = INITIAL;
        }
    }
    nodes[width * seedY + seedX].totalCost = 0;
    nodes[width * seedY + seedX].prevNode = NULL;
    pq.Insert(&(nodes[width * seedY + seedX]));
    Node * r[8];
    Node* q;
    int qy, qx, i, j;
    double newCost;
    int count = 0;
    while (!pq.IsEmpty() && count < numExpanded) {
        count++;
        q = pq.ExtractMin();
        qy = q->row;
        qx = q->column;
        if (selection != NULL) {
            if (selection[width * qy + qx] != 1) {
                goto endloop;
            }
        }
        q->state = EXPANDED;
        //fprintf(log, "qy:%d qx: %d qptr:%x totalcost:%f\n", qy, qx, q, q->totalCost);

        //memset(r, NULL, 8);
        for (j = 0; j < 8; j++) {
            r[j] = NULL;
        }

        if ((width * (qy - 1) + qx - 1) >= 0 && (width * (qy - 1) + qx - 1) < nodeSize) {
            r[3] = &nodes[width * (qy - 1) + qx - 1];
        }
        if ((width * (qy - 1) + qx) >= 0 && (width * (qy - 1) + qx) < nodeSize) {
            r[2] = &nodes[width * (qy - 1) + qx];
        }
        if ((width * (qy - 1) + qx + 1) >= 0 && (width * (qy - 1) + qx + 1) < nodeSize) {
            r[1] = &nodes[width * (qy - 1) + qx + 1];
        }
        if ((width * (qy) + qx - 1) >= 0 && (width * (qy) + qx - 1) < nodeSize) {
            r[4] = &nodes[width * (qy) + qx - 1];
        }
        if ((width * (qy) + qx + 1) >= 0 && (width * (qy) + qx + 1) < nodeSize) {
            r[0] = &nodes[width * (qy) + qx + 1];
        }
        if ((width * (qy + 1) + qx - 1) >= 0 && (width * (qy + 1) + qx - 1) < nodeSize) {
            r[5] = &nodes[width * (qy + 1) + qx - 1];
        }
        if ((width * (qy + 1) + qx) >= 0 && (width * (qy + 1) + qx) < nodeSize) {
            r[6] = &nodes[width * (qy + 1) + qx];
        }
        if ((width * (qy + 1) + qx + 1) >= 0 && (width * (qy + 1) + qx + 1) < nodeSize) {
            r[7] = &nodes[width * (qy + 1) + qx + 1];
        }
        for (i = 0; i < 8; i++) {
            if (r[i] != NULL) {
                //fprintf(log, "\tr[%d] %x \n", i, r[i]);
                //fflush(log);
                if ((r[i]->state) != EXPANDED) {
                    if ((r[i]->state) == INITIAL) {
                        r[i]->totalCost = (q->totalCost) + (q->linkCost[i]);
                        //fprintf(log, "\t\tadd\n");
                        r[i]->state = ACTIVE;
                        r[i]->prevNode = q;
                        pq.Insert(r[i]);
                    } else if ((r[i]->state) == ACTIVE) {
                        //fprintf(log, "\t\trenew\n");
                        newCost = (q->totalCost) + (q->linkCost[i]);
                        if (newCost < (r[i]->totalCost)) {
                            r[i]->totalCost = newCost;
                            r[i]->prevNode = q;
                            pq.Update(r[i]);
                            //extra credit: the 6th whistle
                        } else if (newCost = (r[i]->totalCost)) {
                            //calculate real path length
                            if ((numNodesInPath(q) + 1) < numNodesInPath(r[i])) {
                                r[i]->totalCost = newCost;
                                r[i]->prevNode = q;
                                pq.Update(r[i]);
                            }
                        }
                    }
                }
            }
        }

endloop:
        ;
    }
    //fclose(log);
    //free(&pq);
    //printf("LiveWireDP end\n");

}
/************************ END OF TODO 4 ***************************/

/************************ TODO 5 ***************************
 *MinimumPath:
 *	INPUT:
 *		nodes:				a node buffer of size width by height;
 *		width, height:		dimensions of the node buffer;
 *		freePtX, freePtY:	an input node position;
 *	OUTPUT:
 *		insert a list of nodes along the minimum cost path from the seed node to the input node.
 *		Notice that the seed node in the buffer has a NULL predecessor.
 *		And you want to insert a *pointer* to the Node into path, e.g.,
 *		insert nodes+j*width+i (or &(nodes[j*width+i])) if you want to insert node at (i,j), instead of nodes[nodes+j*width+i]!!!
 *		after the procedure, the seed should be the head of path and the input code should be the tail
 */

void MinimumPath(CTypedPtrDblList <Node>* path, int freePtX, int freePtY, Node* nodes, int width, int height) {
    //printf("TODO: %s:%d\n", __FILE__, __LINE__); 
    //printf("MinimumPath start\n");
    //fflush(stdout);
    Node* inputNode = &(nodes[width * freePtY + freePtX]);
    path->AddTail(inputNode);
    while (inputNode->prevNode != NULL) {
        path->AddPrev(path->GetHeadPtr(), inputNode->prevNode);
        inputNode = inputNode->prevNode;
    }
    int i;
    for (i = 0; i < width * height; i++) {

        nodes[i].state = INITIAL;
        nodes[i].prevNode = NULL;
        nodes[i].totalCost = 0;
    }
    //printf("MinimumPath end\n");

}
/************************ END OF TODO 5 ***************************/

/************************ An Extra Credit Item ***************************
 *SeeSnap:
 *	INPUT:
 *		img:				a RGB image buffer of size width by height;
 *		width, height:		dimensions of the image buffer;
 *		x,y:				an input seed position;
 *	OUTPUT:
 *		update the value of x,y to the closest edge based on local image information.
 */

void SeedSnap(int& x, int& y, unsigned char* img, int width, int height) {

    printf("SeedSnap in iScissor.cpp: to be implemented for extra credit!\n");
}

//generate a cost graph from original image and node buffer with all the link costs;

void MakeCostGraph(unsigned char* costGraph, const Node* nodes, const unsigned char* img, int imgWidth, int imgHeight) {
    int graphWidth = imgWidth * 3;
    int graphHeight = imgHeight * 3;
    int dgX = 3;
    int dgY = 3 * graphWidth;

    int i, j;
    for (j = 0; j < imgHeight; j++) {
        for (i = 0; i < imgWidth; i++) {
            int nodeIndex = j * imgWidth + i;
            int imgIndex = 3 * nodeIndex;
            int costIndex = 3 * ((3 * j + 1) * graphWidth + (3 * i + 1));

            const Node* node = nodes + nodeIndex;
            const unsigned char* pxl = img + imgIndex;
            unsigned char* cst = costGraph + costIndex;

            cst[0] = pxl[0];
            cst[1] = pxl[1];
            cst[2] = pxl[2];

            //r,g,b channels are grad info in seperate channels;
            DigitizeCost(cst + dgX, node->linkCost[0]);
            DigitizeCost(cst - dgY + dgX, node->linkCost[1]);
            DigitizeCost(cst - dgY, node->linkCost[2]);
            DigitizeCost(cst - dgY - dgX, node->linkCost[3]);
            DigitizeCost(cst - dgX, node->linkCost[4]);
            DigitizeCost(cst + dgY - dgX, node->linkCost[5]);
            DigitizeCost(cst + dgY, node->linkCost[6]);
            DigitizeCost(cst + dgY + dgX, node->linkCost[7]);
        }
    }
}

