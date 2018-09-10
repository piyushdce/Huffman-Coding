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
*Compiling: [gcc -Werror -lm -Wall -O3 unhuff.c -o unhuff]
*******************************/

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
		/*(root+idx)->char_value= (root+i-1)->char_value; */
		(root+idx)->right= (root+i-1);
		(root+idx)->left= (root+i);
		}
}
// Read header to get the #char (i.e. leaf nodes in coding tree) and pseudo-eof char.
int get_num_chars(FILE *infile, unsigned char *ps_eof)
{
	int num_ch;
	char newline;
	fscanf(infile, "%c", ps_eof);
	fscanf(infile, "%d", &num_ch);
	fscanf(infile, "%c", &newline);
return num_ch;
}
// Read header to get the char frequency. After this build coding tree.
int get_char_count(FILE *infile, int num_chars, char *c_list, int *c_count)
{
	char next_char;
	int char_count;
	char newline;
	int i;
	for(i=0; i<num_chars; i++)
	{
	fscanf(infile, "%c", &next_char);
	fscanf(infile, "%d", &char_count);
	fscanf(infile, "%c", &newline);
	//printf("char: %c count:%d \n", next_char,char_count);
	*(c_list+i)=next_char;
	*(c_count+i)=char_count;
	}
return 0;
}
// Start putting uncompressed data to file.
int unhuff(FILE *infile, FILE *outfile, struct node* root, unsigned char *ps_eof)
{
    struct node* curr_node = (struct node*)malloc(sizeof(struct node));
    curr_node = root;
	unsigned char buf;
	int i=0;
	int read_byte[8];
	int chars_read=0;
	int got_eof=0;
	while(!got_eof)
		{
		fscanf(infile, "%c", &buf);
		chars_read++;
		for(i=0; i<8; i++)
			{
			read_byte[i]=buf%2;
			buf=buf/2;
			}
		for(i=7; i>=0; i--)
			{
			if(read_byte[i]==0)
				{
				curr_node=curr_node->left;
				}
			if(read_byte[i]==1)
				{
				curr_node=curr_node->right;
				}
			if(!(curr_node->left) && !(curr_node->right)) //reached leaf, print character.
				{
				if(curr_node->char_value==*ps_eof)
					{
					got_eof=1;
					curr_node=root;
					}
				else
					{
					fprintf(outfile, "%c", curr_node->char_value);
					curr_node=root;
					}
				}
			}
		}
	//free(curr_node);
return 0;
}

int main(int argc, char *argv[])
{
   if (argc != 2) {
      fprintf(stderr, "ERROR! Missing Input file :( \n");
      fprintf(stderr, "Usage: ./unhuff.c <File to compress> \n");
      return EXIT_FAILURE;
   }
   clock_t Start = 0;
   clock_t End = 0;
   double uncomp_Time = 0;
   Start = clock();
	int num_chars;
	FILE *infile;
	infile=fopen(argv[1],"r");
	if(infile == NULL) 
		{
		fprintf(stderr, "\n Error opening input file %s \n", argv[1]);
		return 1;
		}
   int len=strlen(argv[1]);
   char out_file[len+7];
   strcpy(out_file,argv[1]);
   strcat(out_file, ".unhuff");
	FILE *outfile;
	outfile=fopen(out_file,"w");
	if(outfile == NULL) 
		{
		fprintf(stderr, "\n Error opening input file %s \n", out_file);
		return 1;
		}
	unsigned char ps_eof=0;
	num_chars=get_num_chars(infile, &ps_eof);
	char *char_list = (char *) malloc(num_chars*sizeof(char));
	int *char_count = (int *) malloc(num_chars*sizeof(int));
	get_char_count(infile,num_chars, char_list, char_count);
    struct node *root = create_Forest(num_chars, char_list, char_count);
	free(char_list);
	free(char_count);
	build_Coding_Tree(num_chars, root);
	unhuff(infile, outfile, root, &ps_eof);
	fclose(infile);
	fclose(outfile);
    End = clock();
    uncomp_Time += difftime(End, Start);
    printf("UnCompression time: %le\n", uncomp_Time/CLOCKS_PER_SEC);
    return EXIT_SUCCESS;
}




