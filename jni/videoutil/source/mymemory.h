typedef struct NODE
{
  void *alloc;
  struct NODE *next;
}Node;//使用链表记录申请的内存
static Node head;
int total=0;//记录未释放的内存（malloc一次，自增，free一次，自减）
static void *FreeAll(Node *p)
{
   //递归释放p结点（包括后续结点）
    if(p==NULL) return NULL;//递归出口
    if(p->next!=NULL)
    p->next=(Node *)FreeAll(p->next);
    free(p->alloc);
    free(p);
    total-=2;//free2次，自减2
    return NULL;  

}
void *MMALLOC(int size)
{
  static Node *p;
  static char f;//f用以标记是否初始化（仅一次初始化）
  if(size<=0) exit(-1);
  if(f==0){
    p=&head;
    p->alloc=NULL;
    f=1;
  }
//生成结点并返回
  p->next=(Node *)malloc(sizeof(Node));
  p=p->next;
  p->alloc=malloc(size);
  p->next=NULL;
  total+=2;//malloc2次，自增2
  return p->alloc;
}
int MFREE(void *tmp){//释放tmp指向的结点，p==NULL时释放全部；
  Node *pa,*pb;
  if(tmp==NULL){
     FreeAll(head.next);
     return 0;
  }
  pa=&head;
free(tmp);
        // pb->next=pa->next;
        // free(pa);//free2次，自减2
         total-=1;

  return 1;
}