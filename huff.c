#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <string.h>

/******************************
*[Piyush Agarwal]
*[huff.c]: [7/14/18].
*[agarw145@purdue.edu]
*Compiling: [gcc -Werror -lm -Wall -O3 huff.c -o huff]
*******************************/

void swap(int* low, int* high, char* c1, char* c2)
{
    int temp = *low;
    *low = *high;
    *high = temp;
	char tc = *c1;
	*c1 = *c2;
	*c2 = tc;
}

int partition (int* c_count, char* c_list, int low, int high)
{
    int pivot = c_count[high];    // pivot
    int i = (low - 1);  // Index of smaller element
 	int j;
    for (j = low; j <= high- 1; j++)
    {
        if (c_count[j] >= pivot)
        {
            i++;    // increment index of smaller element
            swap(&c_count[i], &c_count[j], &c_list[i], &c_list[j]);
        }
    }
    swap(&c_count[i + 1], &c_count[high], &c_list[i+1], &c_list[high]);
    return (i + 1);
}
// quicksort function to sort the char list acc to freqency of char.
void quickSort(int* c_count, char* c_list, int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, c_list[p] is now
           at right place */
        int pi = partition(c_count, c_list, low, high);
 
        // Separately sort elements before partition and after partition
        quickSort(c_count, c_list, low, pi - 1);
        quickSort(c_count, c_list, pi + 1, high);
    }
}
// Get char and their freq. Also pick an unused char as pseudo-eof char.
int Get_char_count(char *Filename, char *c_list, int *c_count, unsigned char *ps_eof)
{
	FILE *infile;
	char buf;
	int i=0; 
	int index=0;
	int new_char=1;
	infile=fopen(Filename,"r");
	if(infile == NULL) // check if file exists and is readable.
		{
		fprintf(stderr, "\n Error opening file %s \n", Filename);
		return 1;
		}
	else
	{
	while(fscanf(infile, "%c", &buf)!=EOF) // read file until EOF.
		{new_char=1; 
		for(i=0; i<256 && new_char==1; i++)
			{
			if (*(c_list+i)==buf)
				{
				*(c_count+i) +=1; 
				new_char=0;
				//break;
				}
			}
		if(new_char==1)
			{
			*(c_list+index)=buf;
			*(c_count+index)=1;
			index=index+1;
			}
		}
// Determine the pseudo-eof character. start from ASCII count 1 until unused char is found.
	int i=1;
	int found_ps_eof=0;
	*ps_eof=1;
	while(!found_ps_eof)
		{
		found_ps_eof=1;
		for(i=1; i<index; i++)
			{
			if(*(c_list+i)==*ps_eof)
				{
				*ps_eof=*ps_eof+1;
				found_ps_eof=0;
				}
			}
		}
		*(c_list+index)=*ps_eof;
		*(c_count+index)=1;
		//printf("The pseudo eof char chosen is %c, %d \n",*(c_list+index),*(c_list+index) );
		index=index+1;
	fclose(infile);
	/*printf("**** Char count is as follows: \n");
	for(i=0; i<index; i++)
		{
		printf("char: %c count: %d \n",*(c_list+i),*(c_count+i));
		}*/
	}
return(index);
}

struct node 
{
    int char_count;
	char char_value;
    struct node *left;
    struct node *right;
};
// Create a forest of all nodes.
struct node* create_Forest(int num_chars, char *c_list, int *c_count)
{
	struct node* node = (struct node*)malloc(((num_chars*2)-1)*sizeof(struct node));
	int i;
	for (i=0; i<num_chars-1;i++)
		{
		(node+i)->char_count = -1;
		(node+i)->left = NULL;
		(node+i)->right = NULL;
		}
	for (i=num_chars-1; i<(2*num_chars)-1;i++)
		{
		(node+i)->char_count = c_count[i+1-num_chars];
		(node+i)->char_value = c_list[i+1-num_chars];
		(node+i)->left = NULL;
		(node+i)->right = NULL;
		}
	return(node);
}
// Print the contents of all trees in forest.
void print_Forest(int num_chars, struct node* root) //debug only
{
	int i;
	for(i=num_chars-1; i<(2*num_chars)-1;i++)
		{
		printf("The %d tree is: \n", i);
		printf("character is %c, ", (root+i)->char_value );
		printf("Count is %d \n", (root+i)->char_count );
		}
}
// Generate the coding tree
void build_Coding_Tree(int num_chars, struct node* root)
{
	int i, sum_nodes, idx;
	for(i=(2*num_chars)-2; i>=0; i-=2)
		{
		idx=(i/2)-1;
		sum_nodes= (root+i)->char_count + (root+i-1)->char_count;
		while((root+idx+1)->char_count > sum_nodes)
			{ 
			(root+idx)->char_count=(root+idx+1)->char_count;
			(root+idx)->char_value=(root+idx+1)->char_value;
			(root+idx)->left=(root+idx+1)->left;
			(root+idx)->right=(root+idx+1)->right;
			idx+=1;
			}
		(root+idx)->char_count= (root+i)->char_count + (root+i-1)->char_count;
		(root+idx)->right= (root+i-1);
		(root+idx)->left= (root+i);
		}
}

void print_Coding_Tree(int num_chars, struct node* root) // debug only
{
	int i;
	for(i=0; i<(2*num_chars)-1;i++)
		{
		printf("The %d node is: ", i);
		printf("character is %c, ", (root+i)->char_value );
		printf("Count is %d \n", (root+i)->char_count );
		}
}
// get depth of tree. Number of cols in coding table = depth+1
void get_depth(int *depth, int top, struct node* root )
{  
	if(root->left)
		{
		get_depth(depth, top+1, root->left);
		}
	if(root->right)
		{
		get_depth(depth, top+1, root->right);
		}
	if(!(root->left) && !(root->right))
		{
		if(top > *depth)
		*depth=top;
		}
}
// Build coding table from the coding tree. #rows=256, #cols=depth+1
void build_Table(int depth, int top, int *code, struct node* root, int *codes)
{  int i,index;
	if(root->left)
		{
		*(code+top)=0;
		build_Table(depth, top+1, code, root->left, codes);
		}
	if(root->right)
		{
		*(code+top)=1;
		build_Table(depth, top+1, code, root->right, codes);
		}
	if(!(root->left) && !(root->right))
		{
		//debug printf("%c: %d ", root->char_value, root->char_value);
		if(root->char_value>0)
		{index=root->char_value; }
		else
		{index=root->char_value+256; }
		for(i=0; i<top; i++)
			{
			//debug printf("%d", *(code+i));
			*(codes+index*(depth+1)+i)=*(code+i);
			}*(codes+index*(depth+1)+i)=-1; //assign -1 at end of code. 
		//debug printf("\n");
		}
}
// This function prints the coding table. <for debugging only>
void print_Table(int depth, int *codes) // debug
{
	int i,j;
printf("------ Printing table -----------\n");
	for(i=0; i<256; i++)
		{printf("%d: ", i);
		for(j=0; j<depth+1; j++)
			{printf("%d ", *(codes+i*(depth+1)+j)); 
		}printf("\n");}
}
// Write compressed data to output file
int write_huff(int num_chars, char *inFilename, char *outFilename, int depth, int* codes, char *c_list, int *c_count, unsigned char *ps_eof)
{
	FILE *infile;
	FILE *outfile;
	unsigned char buf, char_to_write=0;
	int i=0, bit_count=0; 
	infile=fopen(inFilename,"r");
	if(infile == NULL) 
		{
		fprintf(stderr, "\n Error opening input file %s \n", inFilename);
		return 1;
		}
	outfile=fopen(outFilename,"w");
	if(outfile==NULL)
		{
		fprintf(stderr, "\n Error opening output file %s \n", outFilename);
		return 1;
		}
	// write header information.
	fprintf(outfile,"%c%d\n",*ps_eof,num_chars); //pseudo-eof and #unique characters 
	while(i<num_chars)
		{
		fprintf(outfile,"%c%d\n",*(c_list+i),*(c_count+i));
		i++;
		}
	while(fscanf(infile, "%c", &buf)!=EOF) // read file one char at a time.
		{ 
		i=0;
		while(*(codes+buf*(depth+1)+i) != -1) //
			{
			char_to_write=(char_to_write*2) + *(codes+buf*(depth+1)+i);
			bit_count++;
			i++;
			if(bit_count==8)
				{
				fprintf(outfile, "%c", char_to_write);
				bit_count=0;
				char_to_write=0;
				}
			}
		}
// write pseudo EOF character. 2 times
    buf=*ps_eof;
		i=0;
		while(*(codes+buf*(depth+1)+i) != -1) //
			{
			char_to_write=(char_to_write*2) + *(codes+buf*(depth+1)+i);
			bit_count++;
			i++;
			if(bit_count==8)
				{
				fprintf(outfile, "%c", char_to_write);
				bit_count=0;
				char_to_write=0;
				}
			}
    buf=*ps_eof;
		i=0;
		while(*(codes+buf*(depth+1)+i) != -1) //
			{
			char_to_write=(char_to_write*2) + *(codes+buf*(depth+1)+i);
			bit_count++;
			i++;
			if(bit_count==8)
				{
				fprintf(outfile, "%c", char_to_write);
				bit_count=0;
				char_to_write=0;
				}
			}
fclose(infile);
fclose(outfile);
return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2) 
		{
		fprintf(stderr, "ERROR! Missing Input file :( \n");
		fprintf(stderr, "Usage: ./huff.c <File to compress> \n");
      	return EXIT_FAILURE;
      	}

   //Read Input file to get count of each ascii character.
   clock_t Start = 0;
   clock_t End = 0;
   double Comp_Time = 0;
   Start = clock();
   int len=strlen(argv[1]);
   char out_file[len+5];
   strcpy(out_file,argv[1]);
   strcat(out_file, ".huff");
   char *char_list = (char *) malloc(256*sizeof(char));
   int *char_count = (int *) malloc(256*sizeof(int));
   unsigned char ps_eof=0;
   int num_chars=Get_char_count(argv[1], char_list, char_count, &ps_eof);
   quickSort(char_count, char_list, 0, num_chars-1);
   struct node *root = create_Forest(num_chars, char_list, char_count);
	int i;
	/*printf(" **** list after sorting **** \n");
	for(i=0; i<num_chars; i++)
		{
		printf("char: %c c: %d count: %d \n",*(char_list+i),*(char_list+i),char_count[i]);
		}*/
   build_Coding_Tree(num_chars, root);
   int depth=0;
   get_depth(&depth, 0, root );
   int *codes = (int*) calloc(256*(depth+1),sizeof(int));
   for(i=0; i<256; i++)
	{
	*(codes+i*(depth+1))=-1; //initialize first value of all code to -1.
	}
   int code[depth];
   build_Table(depth, 0, &code[0], root, codes);
   write_huff(num_chars, argv[1], out_file, depth, codes, char_list, char_count, &ps_eof);
   //debug printf("depth of tree is %d\n", depth);
	free(codes);
	//free(char_count);
   End = clock();
   Comp_Time += difftime(End, Start);
   printf("Compression time: %le\n", Comp_Time/CLOCKS_PER_SEC);
   return EXIT_SUCCESS;
}
