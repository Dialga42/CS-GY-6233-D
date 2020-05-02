#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int n,*trace,*hash;
struct NODE
{
	int val,pre,next;
};
void moveFront(int *first,int *last,int now,struct NODE *list)
{
	int *pre=&list[now].pre,*next=&list[now].next;
	list[*pre].next=*next;
	if (now!=*last)
		list[*next].pre=*pre;
	else
		*last=*pre;
	list[*first].pre=now;
	*next=*first;
	*first=now;
	*pre=-1;
}
int LRU(int f,int k)//Using a stack (with a doubly linked list)
{
	memset(hash,-1,(1<<k)*sizeof(int));
	struct NODE *list=malloc(f*sizeof(struct NODE));
	int HP=0,first=-1,last=-1,res=0;
	for (int i=0;i<n;i++)
	{
		if (hash[trace[i]]!=-1)
		{
			int now=hash[trace[i]];
			if (first!=now)
				moveFront(&first,&last,now,list);
		}
		else
		{
			++res;
			if (HP!=f)
			{
				list[HP]=(struct NODE){trace[i],-1,first};
				if (HP==0)
					last=HP;
				else
					list[first].pre=HP;
				first=HP;
				hash[trace[i]]=HP;
				++HP;
			}
			else
			{
				hash[list[last].val]=-1;
				moveFront(&first,&last,last,list);
				list[first].val=trace[i];
				hash[trace[i]]=first;
			}
		}
	}
	free(list);
	return res;
}
int optimal(int f,int k)
{
	memset(hash,-1,(1<<k)*sizeof(int));
	int res=0;
	int *list=malloc(f*sizeof(int)),cap=0;
	for (int i=0;i<n;i++)
	{
		if (hash[trace[i]]==-1)
		{
			++res;
			if (cap==f)
			{	
				int max=0,id=-1;
				for (int j=0;j<cap;j++)
				{
					int t=i+1,flag=0;
					for (;t<n;t++)
						if (trace[t]==list[j])
						{
							flag=1;
							if (max<t-i)
							{
								max=t-i;
								id=j;
							}
							break;
						}
					if (flag==0&&t==n)
					{
						id=j;
						break;
					}
				}
				hash[list[id]]=-1;
				list[id]=trace[i];
				hash[trace[i]]=id;
			}
			else
			{
				list[cap]=trace[i];
				hash[trace[i]]=cap;
				++cap;
			}
		}
		else
			hash[trace[i]]=i;
	}
	free(list);
	return res;
}
int main(int argc,char* argv[])
{
	if (argc!=3)
	{
		printf("Invalid arguments!\n");
		return -1;
	}
	n=atoi(argv[1]);
	const int k=atoi(argv[2]);
	if (n<16||k<3)
	{
		printf("Invalid arguments!\n");
		return -1;
	}
	hash=(int*)malloc((1<<k)*sizeof(int));
	//printf("%d\n",LRU(3,k));
	//printf("%d\n",optimal(3,k));
	trace=(int*)malloc(n*sizeof(int));
	srand((unsigned)time(NULL));
	for (int i=0;i<n;i++)
		trace[i]=rand()%(1<<k);
	printf("f\tLRU\toptimal\n");
	for (int f=4;f<=(1<<k);f++)
	{
		printf("%02d\t",f);
		printf("%02d\t",LRU(f,k));
		printf("%02d\n",optimal(f,k));
	}
	free(hash);
	return 0;
}