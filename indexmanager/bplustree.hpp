
#ifndef _INDEX_MANAGER_H
#define _INDEX_MANAGER_H

#include<iostream>
#include<fstream>
#include<string>
using namespace std;

#define NULL 0
#define BUFFERSIZE 4096


template<class T>
struct node{
	int block_num; 
	bool is_leaf;
	int father;	
	int brother;
	int record_num;
	int inner_next[339];
	int leaf_address[339];
	T key[339];
	int next;
};

template<class T>
class btree{
private:
	int root;
	int total_num;
	string tableName;
	string attributeName;
	int indexType;
	int keyLength;
	int N;
public:
	btree(string tableName,string attributeName,int root,int totalNum,int indexType,int keyLength);
	~btree();

	int getRoot();
	bool getExisted();

	bool existed_btree(); 
	bool is_leaf(const node<T>* inode); 
	bool is_root(const node<T>* inode);
	int compare(const T &key1,const T &key2);
	void init_new_node(int inode);

	int find_leaf(const T value);
	bool is_in_leaf(node<T>* leaf,const T value);   

	void insert_in_leaf(const T value,int leaf,int fileaddress);  

	void insert_in_node(const T value,int p,int child);

	void insert(const T value,int fileaddress);
	void copy_leaf(const T value,int leaf,int fileaddress,int new_node);   

	void insert_in_parent(int N1,T k,int N2);
	T copy_pnode(const T &value,int N1,int child,int new_node); 

	void print_node(node<T>* p);
	void print_tree(node<T>* r);

	void delete_in_node(const T &k,int p_block);
	void delete_key(T key);
	void merge(int node1,int bro_node);
	void redistribute(int node1,int node2);
	int find_pos(const T &k,node<T>* p);
	void delete_entry(const T &k,int inode);
	int find_neighbor(int p);

	int find_pos_in_parent(int p);

	int getTotal_num();
	string getTableName();
	string getAttributeName();

	void readBlock(int rblock,node<T>* rnode);
	void writeBlock(int wblock,node<T>* inode);

	void writeToCatalog(int* wroot,int* wtotal_num,string* table,string* attribute,int* indexType,int* keyLength);
	int select(T select_key);
};


template<class T>
btree<T>::btree(string tableName,string attributeName,int root,int totalNum,int indexType,int keyLength){
	this->tableName=tableName;
	this->attributeName=attributeName;
	this->root=root;
	this->total_num=totalNum;	
	this->indexType=indexType;
	this->keyLength=keyLength;
	if(indexType==0||indexType==1)
		this->N=(BUFFERSIZE-21)/12;
	else
		this->N=(BUFFERSIZE-21)/40;
}

template<class T>
btree<T>::~btree(){

}

template<class T>
int btree<T>::getRoot(){
	return root;
}

template<class T>
bool btree<T>::getExisted(){
	return existed;
}

template<class T>
int btree<T>::getTotal_num(){
	return total_num;
}

template<class T>
string btree<T>::getTableName(){
	return tableName;
}

template<class T>
string btree<T>::getAttributeName(){
	return attributeName;
}

template<class T>
bool btree<T>::existed_btree(){
	if(existed==true)
		return true;
	else
		return false;
}

template<class T>
bool btree<T>::is_leaf(const node<T>* inode){
	if(inode->is_leaf==true)
		return true;
	else
		return false;
}

template<class T>
bool btree<T>::is_root(const node<T>* inode){
	if(inode->father==NULL)
		return true;
	else
		return false;
}

template<class T>
int btree<T>::compare(const T &key1,const T &key2){
	if(key1-key2>0)
		return 1;
	else if(key1-key2<0)
		return -1;
	else
		return 0;
}

template<class T>
void btree<T>::init_new_node(int inode_num){
	node<T>* inode=(node<T> *)malloc(BUFFERSIZE);
	inode->father=NULL;
	inode->brother=NULL;
	inode->record_num=0;
	inode->block_num=inode_num;
	inode->next=NULL;
	inode->is_leaf=false;
	for(int i=0;i<N;i++){
		inode->inner_next[i]=NULL;
		inode->leaf_address[i]=NULL;
		inode->key[i]=NULL;
	}
	writeBlock(inode_num,inode);
	free(inode);
}

template<class T>
int btree<T>::find_leaf(const T value){
	int i=0,next_read;
	bool f=false;
	int num=0;
	node<T>* current=(node<T> *)malloc(BUFFERSIZE);
	readBlock(root,current);
	while(is_leaf(current)==false){
		f=false;

		for(i=0;i<current->record_num;i++){
			if(compare(value,current->key[i])==-1){
				next_read=current->inner_next[i];
				readBlock(next_read,current);
				f=true;
				break;
			}
		}
		if(f==false&&current->record_num<N)
			readBlock(current->inner_next[current->record_num],current);
		else if(f==false&&current->record_num==N)
			readBlock(current->next,current);
	}
	num=current->block_num;
	free(current);
	return num;
}

template<class T>
bool btree<T>::is_in_leaf(node<T>* leaf,const T value){
	bool flag=false;
	for(int i=0;i<leaf->record_num;i++){
		if(compare(leaf->key[i],value)==0){
			flag=true;
			break;
		}
	}
	return flag;
}

template<class T>
void btree<T>::insert_in_leaf(const T value,int leaf,int fileaddress){
	int i,j;
	node<T>* temp=(node<T> *)malloc(BUFFERSIZE);
	readBlock(leaf,temp);
	for(i=0;i<temp->record_num;i++){
		if(compare(value,temp->key[i])==-1||compare(value,temp->key[i])==0)
			break;
	}

	for(j=temp->record_num;j>i;j--){
		temp->key[j]=temp->key[j-1];
		temp->leaf_address[j]=temp->leaf_address[j-1];
		temp->inner_next[j]=temp->inner_next[j-1];
	}
	temp->key[i]=value;
	temp->leaf_address[i]=fileaddress;
	temp->record_num++;
	writeBlock(leaf,temp);
	free(temp);
}



template<class T>
void btree<T>::copy_leaf(const T value,int leaf,int fileaddress,int new_node){
	struct for_copy_node{
		T copy_key[340];
		int copy_pointer[340];
	}copy_node;
	int i,j,i0,i1,leaf_next,leaf_father;
	
	node<T>* temp=(node<T>*)malloc(BUFFERSIZE);

	readBlock(leaf,temp);
	leaf_next=temp->next;

	for(int k=0;k<N;k++){
		copy_node.copy_key[k]=temp->key[k];
		copy_node.copy_pointer[k]=temp->leaf_address[k];
		temp->key[k]=NULL;
		temp->leaf_address[k]=NULL;
	}

	for(i=0;i<temp->record_num;i++){
		if(compare(value,copy_node.copy_key[i])==-1||compare(value,copy_node.copy_key[i])==0)
			break;
	}
	
	for(j=N;j>i;j--){
		copy_node.copy_key[j]=copy_node.copy_key[j-1];
		copy_node.copy_pointer[j]=copy_node.copy_pointer[j-1];
	}
	copy_node.copy_key[i]=value;
	copy_node.copy_pointer[i]=fileaddress;
		
	for(i=0;i<N/2+1;i++){
		temp->key[i]=copy_node.copy_key[i];
		temp->leaf_address[i]=copy_node.copy_pointer[i];
	}
	temp->next=new_node;
	leaf_father=temp->father;
	writeBlock(leaf,temp);

	readBlock(leaf_father,temp);
	i0=temp->inner_next[0];
	i1=temp->inner_next[1];

	readBlock(leaf,temp);
	if(leaf_father!=NULL&&leaf==i0)
		temp->brother=new_node;
	temp->record_num=N/2+1;
	writeBlock(leaf,temp);	
	
	readBlock(new_node,temp);
	for(i=N/2+1;i<=N;i++){
		temp->key[i-N/2-1]=copy_node.copy_key[i];
		temp->leaf_address[i-N/2-1]=copy_node.copy_pointer[i];
	}

	temp->next=leaf_next;
	temp->block_num=total_num+1;
	temp->father=leaf_father;
	temp->brother=leaf;
	temp->record_num=N-N/2;
	temp->is_leaf=true;
	writeBlock(new_node,temp);

	if(leaf_next!=NULL){
		readBlock(leaf_next,temp);
		temp->brother=new_node;
		writeBlock(leaf_next,temp);
	}
	
	total_num++;
	free(temp);
}

template<class T>
int btree<T>::find_neighbor(int p){
	node<T>* temp=(node<T>*)malloc(BUFFERSIZE);
	readBlock(p,temp);
	int parent=temp->father;
	int i,result=0;
	bool flag=false;
	if(parent==NULL)
		return NULL;

	readBlock(parent,temp);
	for(i=0;i<temp->record_num;i++){
		if(temp->inner_next[i]==p){
			flag=true;
			break;
		}
	}
	if(flag==false)
		return NULL;
	if(i<N-1)
		result=temp->inner_next[i+1];
	else
		result=temp->next;
	free(temp);
	return result;
}

template<class T>
T btree<T>::copy_pnode(const T &value,int N1,int child,int new_node){
	struct for_copy_parent{
		T key[340];
		int pointer[341];
	}p_node;
	int i,j,i0,i1,n_father,n1,new1,n0,new0;
	bool n_is_leaf;

	node<T>* temp=(node<T> *)malloc(BUFFERSIZE);
	readBlock(N1,temp);
	n_father=temp->father;
	n_is_leaf=temp->is_leaf;
	n1=temp->inner_next[1];
	n0=temp->inner_next[0];
	for(int k=0;k<N;k++){
		p_node.key[k]=temp->key[k];
		p_node.pointer[k]=temp->inner_next[k];
		temp->key[k]=NULL;
		temp->inner_next[k]=NULL;
	}

	for(i=0;i<N;i++){
		if(compare(value,p_node.key[i])==-1||compare(value,p_node.key[i])==0)
			break;
	}

	for(j=N;j>i;j--){
		p_node.key[j]=p_node.key[j-1];
	}
	for(j=N;j>i+1;j--){
		p_node.pointer[j]=p_node.pointer[j-1];
	}
	p_node.key[i]=value;
	p_node.pointer[i+1]=child;
	if(i==N)
		p_node.pointer[N]=temp->next;
	else
		p_node.pointer[N+1]=temp->next;

	for(i=0;i<N/2;i++){
		temp->key[i]=p_node.key[i];
		temp->inner_next[i]=p_node.pointer[i];
	}
	temp->record_num=N/2;

	temp->inner_next[N/2]=p_node.pointer[N/2];
	int x=temp->inner_next[N/2];
	writeBlock(N1,temp);

	if(n_father!=NULL){
		readBlock(n_father,temp);
		i0=temp->inner_next[0];
		i1=temp->inner_next[1];
		if(N1==i0){
			readBlock(N1,temp);
			temp->brother=i1;
			writeBlock(N1,temp);
		}
	}
	
	readBlock(new_node,temp);
	new0=temp->inner_next[0];
	new1=temp->inner_next[1];
	for(i=N/2+1;i<=N;i++){
		temp->key[i-N/2-1]=p_node.key[i];
	}
	temp->brother=N1;
	temp->father=n_father;
	temp->block_num=total_num+1;
	temp->record_num=N-N/2;
	temp->is_leaf=n_is_leaf;
	writeBlock(new_node,temp);

	for(i=N/2+1;i<=N+1;i++){
		readBlock(new_node,temp);
		temp->inner_next[i-N/2-1]=p_node.pointer[i];
		writeBlock(new_node,temp);
		readBlock(temp->inner_next[i-N/2-1],temp);
		temp->father=new_node;
		writeBlock(p_node.pointer[i],temp);
	}
	
	int neighbor=find_neighbor(N1);
	if(neighbor!=NULL){
		readBlock(neighbor,temp);
		temp->brother=new_node;
		writeBlock(neighbor,temp);
	}			
	
	readBlock(new0,temp);
	temp->brother=new1;
	writeBlock(new0,temp);
	readBlock(n0,temp);
	temp->brother=n1;
	writeBlock(n0,temp);

	readBlock(x,temp);
	temp->father=N1;
	writeBlock(x,temp);

	T k=p_node.key[N/2];
	total_num++;	
	free(temp);
	return k;
}

template<class T>
void btree<T>::insert_in_node(const T value,int p,int child){
	int i,j;
	node<T>* temp=(node<T> *)malloc(BUFFERSIZE);
	readBlock(p,temp);
	int t_num=temp->record_num;
	temp->record_num++;
	for(i=0;i<t_num;i++){
		if(compare(value,temp->key[i])==-1||compare(value,temp->key[i])==0)
			break;
	}
	if(i<t_num){
		if(t_num==N-1)
			temp->next=temp->inner_next[t_num];
		else
			temp->inner_next[t_num+1]=temp->inner_next[t_num];

		for(j=t_num;j>i;j--){
			temp->key[j]=temp->key[j-1];
		}
		for(j=t_num;j>i+1;j--){
			temp->inner_next[j]=temp->inner_next[j-1];
		}
		temp->key[i]=value;
		temp->inner_next[i+1]=child;
		int p1=temp->inner_next[1];
		int p0=temp->inner_next[0];
		
		if(is_leaf(temp)==false){
			writeBlock(p,temp);
			readBlock(p0,temp);
			temp->brother=p1;
			writeBlock(p0,temp);
			readBlock(p,temp);
		}
	}
	else{
		if(t_num+1==N)
			temp->next=child;
		else
			temp->inner_next[t_num+1]=child;
		temp->key[t_num]=value;		
	}
	
	writeBlock(p,temp);
	free(temp);
}

template<class T>
void btree<T>::insert_in_parent(int N1,T k,int N2){
	node<T>* temp=(node<T> *)malloc(BUFFERSIZE);
	int p;
	readBlock(N1,temp);
	p=temp->father;
	if(is_root(temp)){
		int r=total_num+1;
		init_new_node(r);
		for(int i=0;i<N;i++){
			temp->inner_next[i]=NULL;
			temp->leaf_address[i]=NULL;
			temp->key[i]=NULL;
		}
		temp->next=NULL;
		temp->key[0]=k;
		temp->inner_next[0]=N1;
		temp->inner_next[1]=N2;
		temp->father=NULL;
		temp->brother=NULL;
		temp->record_num=1;
		temp->block_num=r;
		temp->is_leaf=false;
		writeBlock(r,temp);

		readBlock(N1,temp);
		temp->brother=N2;
		temp->father=r;
		writeBlock(N1,temp);
		readBlock(N2,temp);
		temp->father=r;
		writeBlock(N2,temp);

		root=r;
		total_num++;
		free(temp);
		return;
	}
	readBlock(p,temp);
	if(temp->record_num<=N-1){
		insert_in_node(k,p,N2);
		return;
	}
	else{
		int new_node=total_num+1;
		init_new_node(new_node);
		T k2=copy_pnode(k,p,N2,new_node);
		insert_in_parent(p,k2,new_node);
	}
}


template<class T>
void btree<T>::insert(const T value,int fileaddress){
	int leaf_block_num=find_leaf(value);
	node<T>* temp=(node<T> *)malloc(BUFFERSIZE);
	readBlock(leaf_block_num,temp);
	if(is_in_leaf(temp,value)==true)
		return;
	
	if(temp->record_num<N)
		insert_in_leaf(value,leaf_block_num,fileaddress);
	else{
		int new_node=total_num+1;
		init_new_node(new_node);
		copy_leaf(value,leaf_block_num,fileaddress,new_node);
		readBlock(new_node,temp);
		T key_min=temp->key[0];		
		insert_in_parent(leaf_block_num,key_min,new_node);
	}
	free(temp);
}

template<class T>
void btree<T>::delete_in_node(const T &k,int p_block){
	int i,j,pos;
	node<T>* p=(node<T>*)malloc(BUFFERSIZE);
	node<T>* bonus=(node<T>*)malloc(BUFFERSIZE);
	readBlock(p_block,p);
	pos=find_pos(k,p);
	i=pos;

	for(j=i;j<p->record_num-1;j++){
		p->key[j]=p->key[j+1];
		p->leaf_address[j]=p->leaf_address[j+1];
	}
	
//	if(pos!=0){
		int x;
		if(is_leaf(bonus))
			x=(N+1)/2;
		else
			x=N/2;
		if(pos==0&&is_leaf(p)==false&&bonus->record_num<x)
			p->inner_next[0]=p->inner_next[1];

		for(j=i+1;j<p->record_num-1;j++)
			p->inner_next[j]=p->inner_next[j+1];
		readBlock(p->inner_next[0],bonus);

//	}
//	else{
//		for(j=i;j<p->record_num-1;j++)
//			p->inner_next[j]=p->inner_next[j+1];
//	}

	if(pos!=p->record_num-1){
		if(p->record_num==N)
			p->inner_next[p->record_num-1]=p->next;
		else
			p->inner_next[p->record_num-1]=p->inner_next[p->record_num];
	}
	p->key[p->record_num-1]=NULL;
	p->leaf_address[p->record_num-1]=NULL;
	p->record_num--;
	writeBlock(p_block,p);
	free(p);
	free(bonus);
}

template<class T>
void btree<T>::delete_entry(const T &k,int inode){
	int temp,a;
	node<T>* L=(node<T>*)malloc(BUFFERSIZE);
	node<T>* bonus=(node<T>*)malloc(BUFFERSIZE);
	readBlock(inode,L);
	
	if(is_root(L)&&L->record_num==1){
		readBlock(L->inner_next[0],bonus);
		int first_child_num=bonus->record_num;

		readBlock(root,bonus);
		if(first_child_num<=0)			
			root=bonus->inner_next[1];
		else
			root=bonus->inner_next[0];
		readBlock(root,bonus);
		bonus->father=NULL;
		bonus->brother=NULL;
		writeBlock(root,bonus);
		return;
	}
	readBlock(L->brother,bonus);
	if(is_leaf(L)){
		temp=(N+1)/2;
		a=1;
	}
	else{
		a=0;
		temp=N/2;
	}
	if(L->record_num>=temp+1||is_root(L))
		delete_in_node(k,inode);
	else if(bonus->record_num + L->record_num <=N+a){
		delete_in_node(k,inode);
		merge(inode,L->brother);
	}
	else{
		delete_in_node(k,inode);
		redistribute(inode,L->brother);	
	}
	free(L);
	free(bonus);
}

template<class T>
void btree<T>::delete_key(T key){
	int rblock=find_leaf(key);
	node<T>* L=(node<T> *)malloc(BUFFERSIZE);
	readBlock(rblock,L);
	if(is_in_leaf(L,key)==false)
		return;
	delete_entry(key,rblock);
	free(L);
}

template<class T>
int btree<T>::find_pos(const T &k,node<T>* p){
	int i;
	for(i=0;i<p->record_num;i++){
		if(compare(k,p->key[i])==0)
			break;
	}
	return i;
}


template<class T>
int btree<T>::find_pos_in_parent(int p){
	int i;
	node<T>* temp=(node<T> *)malloc(BUFFERSIZE);
	readBlock(p,temp);
	int parent=temp->father;
	readBlock(parent,temp);
	for(i=0;i<temp->record_num;i++){
		if(temp->inner_next[i]==p)
			break;
	}
	free(temp);
	return i;
}

template<class T>
void btree<T>::merge(int node1,int bro_node){
	node<T>* L=(node<T>*)malloc(BUFFERSIZE);
	node<T>* bro=(node<T>*)malloc(BUFFERSIZE);
	node<T>* f=(node<T>*)malloc(BUFFERSIZE);
	readBlock(node1,L);
	readBlock(bro_node,bro);
	readBlock(L->father,f);
	int bro_num=bro->record_num;
	int fa=bro->father;
	T k;
	int pos=find_pos_in_parent(node1);
	int i=pos;

	if(i!=0){
		k=f->key[i-1];	
		if(is_leaf(L)){
			for(int i=0;i<L->record_num;i++){
				bro->key[bro_num+i]=L->key[i];
				bro->leaf_address[bro_num+i]=L->leaf_address[i];
			}
			bro->record_num=bro->record_num+L->record_num;
		}
		else{
			bro->key[bro_num]=k;

			for(int i=0;i<L->record_num;i++){
				bro->key[bro_num+i+1]=L->key[i];
				bro->inner_next[bro_num+i+1]=L->inner_next[i];
			}	
			if(bro->record_num+L->record_num==N-1)
				bro->next=L->inner_next[L->record_num];
			else
				bro->inner_next[bro_num+L->record_num+1]=L->inner_next[L->record_num];
			bro->record_num=bro->record_num+L->record_num+1;
		}
	}
	else{
		k=f->key[0];
		if(is_leaf(L)){
			for(int i=bro_num-1;i>=0;i--){
				bro->key[L->record_num+i]=bro->key[i];
				bro->leaf_address[L->record_num+i]=bro->leaf_address[i];
			}
			for(int i=0;i<L->record_num;i++){
				bro->key[i]=L->key[i];
				bro->leaf_address[i]=L->leaf_address[i];
			}
			
			bro->record_num=bro->record_num+L->record_num;
		}
		else{
			if(bro->record_num+L->record_num==N-1)
				bro->next=bro->inner_next[bro_num];
			else
				bro->inner_next[bro_num+L->record_num+1]=bro->inner_next[bro_num];

			for(int i=bro_num-1;i>=0;i--){
				bro->key[L->record_num+i+1]=bro->key[i];
				bro->inner_next[L->record_num+i+1]=bro->inner_next[i];
			}
			bro->key[L->record_num]=k;
			for(int i=0;i<L->record_num;i++){
				bro->key[i]=L->key[i];
				bro->inner_next[i]=L->inner_next[i];
			}
			if(L->record_num==0)
				bro->inner_next[0]=L->inner_next[L->record_num];
			bro->record_num=bro_num+L->record_num+1;			
		}
	}
	
	if(is_leaf(L)){
		if(pos!=0){
			if(L->next!=NULL){
				readBlock(L->next,f);
				f->brother=bro_node;
				writeBlock(L->next,f);
				readBlock(L->father,f);
			}
			bro->next=L->next;	
			if(bro_node==f->inner_next[0])
				bro->brother=L->next;
		}
		else
			bro->brother=bro->next;
	}
	else{
		if(pos!=0){
			int neigh=find_neighbor(node1);
			if(neigh!=NULL){
				readBlock(neigh,f);
				f->brother=bro_node;
				writeBlock(neigh,f);
				readBlock(L->father,f);
			}
			if(bro_node==f->inner_next[0])
				bro->brother=f->inner_next[1];
		}
		else{
			int temp=find_neighbor(bro_node);
			if(temp!=NULL)
				bro->brother=temp;
		}
	}

	writeBlock(node1,L);
	writeBlock(bro_node,bro);
	free(L);
	free(bro);
	free(f);
	delete_entry(k,fa);
}

template<class T>
void btree<T>::redistribute(int node1,int node2){
	int pos;
	node<T>* p1=(node<T>*)malloc(BUFFERSIZE);
	node<T>* p2=(node<T>*)malloc(BUFFERSIZE);
	node<T>* f=(node<T>*)malloc(BUFFERSIZE);
	readBlock(node1,p1);
	readBlock(node2,p2);
	readBlock(p1->father,f);
	if(node1!=f->inner_next[0]){
		if(p1->is_leaf==true){
			for(int i=1;i<=p1->record_num;i++){
				p1->key[i]=p1->key[i-1];
				p1->leaf_address[i]=p1->leaf_address[i-1];
			}
			p1->key[0]=p2->key[p2->record_num-1];
			p1->leaf_address[0]=p2->leaf_address[p2->record_num-1];
			p1->record_num++;
			p2->key[p2->record_num-1]=NULL;
			p2->leaf_address[p2->record_num-1]=NULL;
			p2->record_num--;
			int pos=find_pos_in_parent(node1);
			f->key[pos-1]=p1->key[0];
		}
		else{
			for(int i=p1->record_num;i>0;i--){
				p1->key[i]=p1->key[i-1];
				p1->inner_next[i]=p1->inner_next[i-1];
			}	
			if(p1->record_num==0)
				p1->inner_next[1]=p1->inner_next[0];
			int pos=find_pos_in_parent(node1);
			p1->key[0]=f->key[pos-1];
			if(p2->record_num<N)
				p1->inner_next[0]=p2->inner_next[p2->record_num];
			else
				p1->inner_next[0]=p2->next;
			f->key[pos-1]=p2->key[p2->record_num-1];
			p2->key[p2->record_num-1]=NULL;
			if(p2->record_num<N)
				p2->inner_next[p2->record_num]=NULL;
			else
				p2->next=NULL;
			p1->record_num++;
			p2->record_num--;
		}
	}
	else{
		if(p1->is_leaf==true){
			p1->key[p1->record_num]=p2->key[0];
			p1->leaf_address[p1->record_num]=p2->leaf_address[0];
			p1->record_num++;
			for(int i=0;i<p2->record_num-1;i++){
				p2->key[i]=p2->key[i+1];
				p2->leaf_address[i]=p2->leaf_address[i+1];
			}
			p2->leaf_address[p2->record_num-1]=NULL;
			p2->key[p2->record_num-1]=NULL;
			p2->record_num--;
			int pos=find_pos_in_parent(node2);
			f->key[pos-1]=p2->key[0];
		}
		else{
			pos=find_pos_in_parent(node2);
			p1->key[p1->record_num]=f->key[pos-1];
			p1->inner_next[p1->record_num+1]=p2->inner_next[0];
			f->key[pos-1]=p2->key[0];
			for(int i=0;i<p2->record_num-1;i++){
				p2->key[i]=p2->key[i+1];
				p2->inner_next[i]=p2->inner_next[i+1];
			}
			if(p2->record_num==N)
				p2->inner_next[p2->record_num-1]=p2->next;
			else
				p2->inner_next[p2->record_num-1]=p2->inner_next[p2->record_num];
			p2->inner_next[p2->record_num]=NULL;
			p2->key[p2->record_num-1]=NULL;
			p1->record_num++;
			p2->record_num--;
		}
	}
	writeBlock(node1,p1);
	writeBlock(node2,p2);
	writeBlock(p1->father,f);
}

template<class T>
void btree<T>::print_node(node<T>* p){
	if(p==NULL)
		return;
	int i=0;
	for(i=0;i<p->record_num;i++){
		cout<<p->key[i]<<" ";
	}
	cout<<endl;
}

template<class T>
void btree<T>::print_tree(node<T>* r){
	print_node(r);
	if(r==NULL)
		return;
	if(is_leaf(r))
		return;
	for(int i=0;i<r->record_num;i++){
		print_tree(r->inner_next[i]);
	}
	if(r->record_num<N)
		print_tree(r->inner_next[r->record_num]);
	else if(r->record_num==N)
		print_tree(r->next);
}


template<class T>
void btree<T>::readBlock(int rblock,node<T>* inode){	
	string index_file=tableName+"_"+attributeName+".txt";
	const char* file_name=index_file.c_str();
	char buf[BUFFERSIZE]; 
	char* p=buf;
	fstream file(file_name,ios::binary|ios::in|ios::out);
	int offset=(rblock-1)*BUFFERSIZE;
	file.seekg(offset,ios::beg);
	file.read(buf,BUFFERSIZE);
	file.close();
	memcpy(&(inode->block_num),p,4);
	p+=4;
	memcpy(&(inode->is_leaf),p,1);
	p+=1;
	memcpy(&(inode->father),p,4);
	p+=4;
	memcpy(&(inode->brother),p,4);
	p+=4;
	memcpy(&(inode->record_num),p,4);
	p+=4;
	for(int i=0;i<inode->record_num;i++){
		memcpy(&(inode->inner_next[i]),p,4);
		p+=4;memcpy(&(inode->leaf_address[i]),p,4);
		p+=4;memcpy(&(inode->key[i]),p,4);
		p+=keyLength;
	}
	if(inode->record_num!=N&&inode->is_leaf==false){
		memcpy(&(inode->inner_next[inode->record_num]),p,4);
	}
	else{
		memcpy(&(inode->next),p,4);
	}
	inode->block_num=rblock;
}


template<class T>
void btree<T>::writeBlock(int wblock,node<T>* inode){
	string index_file=tableName+"_"+attributeName+".txt";
	const char* file_name=index_file.c_str();
	fstream file(file_name,ios::binary|ios::in|ios::out);
	char buf[BUFFERSIZE];
	for(int i=0;i<BUFFERSIZE;i++)
		buf[i]=NULL;
	char* p=buf;
	memcpy(p,&(inode->block_num),4);
	p+=4;
	memcpy(p,&(inode->is_leaf),1);
	p+=1;
	memcpy(p,&(inode->father),4);
	p+=4;
	memcpy(p,&(inode->brother),4);
	p+=4;
	memcpy(p,&(inode->record_num),4);
	p+=4;
	for(int i=0;i<inode->record_num;i++){
		memcpy(p,&(inode->inner_next[i]),4);
		p+=4;memcpy(p,&(inode->leaf_address[i]),4);
		p+=4;memcpy(p,&(inode->key[i]),4);
		p+=keyLength;
	}
	if(inode->record_num!=N&&inode->is_leaf==false){
		memcpy(p,&(inode->inner_next[inode->record_num]),4);
	}
	else{
		memcpy(p,&(inode->next),4);
	}
	int offset=(wblock-1)*BUFFERSIZE;
	file.seekp(offset,ios::beg);
	file.write(buf,BUFFERSIZE);
	file.close();
}

template<class T>
int btree<T>::select(T select_key){
	int block_pos=find_leaf(select_key);
	int address=-1,i=0;
	node<T>* temp=(node<T>*)malloc(BUFFERSIZE);
	readBlock(block_pos,temp);
	for(i=0;i<temp->record_num;i++){
		if(compare(select_key,temp->key[i])==0)
			break;
	}
	if(i<temp->record_num){
		address=temp->leaf_address[i];
		return address;
	}
	else{
		return -1;
	}
}

#endif
