/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 13 */
/*Lab 5 BST source*/

#include <stdio.h>
#include <stdlib.h>
#include "bst.h"

/* Adds a postive short value to the tree */
BST * bst_insert(BST * tree, unsigned short newValue) {
	if(tree==NULL) { 
        tree = (BST *)malloc(sizeof(BST)); 
        tree->root=(Node *)malloc(sizeof(Node)); 
        (tree->root)->value = newValue; 
        (tree->root)->right = NULL; 
        (tree->root)->left = NULL; 
    	}
	else
		tree->root = bst_nodeInsert(tree->root, newValue);
	return tree;
}

Node * bst_nodeInsert(Node * node, unsigned short newValue) {
	if(node==NULL) { 
        node = (Node *)malloc(sizeof(Node)); 
        node->value=newValue; 
        node->left = node->right = NULL; 
        return node; 
    } 
    else { 
        if(newValue < node->value) 
            node->left = bst_nodeInsert(node->left, newValue); 
        if(newValue > node->value) 
            node->right = bst_nodeInsert(node->right, newValue); 
    } 
    return node; 
} 

/* Prints all the nodes in the order (i.e. from smallest value in tree to the largest value in order). 
   Each value should be followed by a newline. */ 
void bst_traverseInOrder(BST * tree) {
	if(tree!=NULL)
		bst_NodeTraverseInOrder(tree->root);
}

void bst_NodeTraverseInOrder(Node * node) {
	if(node!=NULL) { /* does nothing if passed a null node */
		if( (node->left == NULL)&&(node->right == NULL)) /*single leaf*/
			printf("%d\n", node->value);
		else { /*has children */
			bst_NodeTraverseInOrder(node->left);
			printf("%d\n", node->value);
			bst_NodeTraverseInOrder(node->right);
		}
	}
}


/* returns "True" (i.e. 1) if findMe is located somewhere in the tree, otherwise "False" (i.e. 0). */ 
int bst_isValueInTree(BST * tree, unsigned short findMe) { 
    if(tree == NULL) 
        return 0; 
    if(tree->root == NULL) 
        return 0; 
    Node * curr = tree->root; 
    while(curr!=NULL) {
    if(curr->value == findMe) 
        return 1; 
    else if(findMe < curr->value) 
        curr = curr->left; 
    else if (findMe > curr->value)
        curr = curr->right;
    }
    return 0;
}
