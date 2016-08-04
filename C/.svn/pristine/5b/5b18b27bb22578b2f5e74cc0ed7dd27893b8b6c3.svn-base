/* Nolan Winkler, nolanwinkler */
/* CS 152, Summer 13 */
/*Lab 5 */

#include <stdio.h>
#include <stdlib.h>
#include "checkit.h"
#include "bst.h"

unsigned short convertToShort(char * binString);

int main(int argc, char * argv[]) {
FILE * fp = fopen(argv[1], "r");
if(fp!=NULL) {
char currLine[17];
fgets(currLine, 17, fp);
BST * progTree = NULL;
unsigned short currNum;
while(!feof(fp)){
	currNum = convertToShort(currLine);
	progTree = bst_insert(progTree,currNum);
	fgets(currLine,17,fp);
}
bst_traverseInOrder(progTree);
}
fclose(fp);
/*string to short assignments */
char zero[] = 			"0000000000000000";
char one[] = 			"0000000000000001";
char sixfivefivethreefive[] = 	"1111111111111111";
char onethreeseven[] = "0000000010001001";
/*BST assignment */
BST * myTree = NULL;
/*string to short tests */
checkit_int(convertToShort(zero),0);
checkit_int(convertToShort(one),1);
checkit_int(convertToShort(sixfivefivethreefive),65535);
checkit_int(convertToShort(onethreeseven), 137);
/*bst tests */
checkit_int(bst_isValueInTree(myTree, 8),0);
myTree = bst_insert(myTree, 8);
checkit_int(bst_isValueInTree(myTree, 8),1);
checkit_int(bst_isValueInTree(myTree, 5),0);
checkit_int(bst_isValueInTree(myTree, 6),0);
/*bst_traverseInOrder(myTree);*/
myTree = bst_insert(myTree, 1);
myTree = bst_insert(myTree, 3);
myTree = bst_insert(myTree,6);
myTree = bst_insert(myTree,4);
myTree = bst_insert(myTree,7);
myTree = bst_insert(myTree,10);
myTree = bst_insert(myTree,14);
myTree = bst_insert(myTree,13);
bst_traverseInOrder(myTree);
checkit_int(bst_isValueInTree(myTree, 6),1);
checkit_int(bst_isValueInTree(myTree, 13),1);
return 0;
}

unsigned short convertToShort(char * binString) {
	unsigned short result = 0; /* 00000000 00000000 */
	int i = 15;
	for(i=0;i<16;i++) { /*binstring[i] is the (15-i)th bit */
		if(binString[i] == '1') /*set the (15-i)th bit */
			result|=1<<(15-i);
	}
	return result;
}
