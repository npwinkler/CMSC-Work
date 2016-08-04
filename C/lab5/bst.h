/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 13 */
/*Lab 5 BST header*/

#ifndef BST_H
#define BST_H

struct tree_node { 
    unsigned short value; 
    struct tree_node * left; 
    struct tree_node * right; 
}; 
typedef struct tree_node Node; 
  
struct bst { 
    Node * root; 
}; 
typedef struct bst BST; 

/* Adds a postive short value to the tree */
BST * bst_insert(BST * tree, unsigned short newValue);
Node * bst_nodeInsert(Node * node, unsigned short newValue);

/* Prints all the nodes in the order (i.e. from smallest value in tree to the largest value in order). 
   Each value should be followed by a newline. */ 
void bst_traverseInOrder(BST * tree); 
void bst_NodeTraverseInOrder(Node * node);

/* returns "True" (i.e. 1) if findMe is located somewhere in the tree, otherwise "False" (i.e. 0). */ 
int bst_isValueInTree(BST * tree, unsigned short findMe);

#endif /*BST_H*/
